package requesthandler

import (
	constant "IMserver/Constant"
	pb "IMserver/IM_protobuf"
	producermanager "IMserver/RequestHandler/KafkaProducers"
	dbmanager "IMserver/RequestHandler/LevelDB"
	mysqlmanager "IMserver/RequestHandler/MysqlManager"
	"database/sql"
	"encoding/binary"
	"errors"
	"io"
	"time"

	"context"
	"fmt"
	"log"

	redis "github.com/go-redis/redis/v8"
	kafka "github.com/segmentio/kafka-go"
	"github.com/syndtr/goleveldb/leveldb"
	"github.com/syndtr/goleveldb/leveldb/opt"
	"google.golang.org/protobuf/proto"
)

type SigngleRequest struct {
	Type_    constant.HeaderType
	Request_ *pb.PullRequest
	Conn_    *constant.Connection
}

type RequestHandler struct {
	mysqlHandler_      *mysqlmanager.MysqlManager
	producerManager_   *producermanager.ProducerManager
	redisHandler_      *redis.Client
	DBHandler_         *dbmanager.DBManager
	Logged_in_accounts map[string]bool
}

func (handler *RequestHandler) Close() {
	if handler.mysqlHandler_ != nil {
		handler.mysqlHandler_.Close()
	}
	if handler.producerManager_ != nil {
		handler.producerManager_.Close()
	}
	if handler.redisHandler_ != nil {
		handler.redisHandler_.Close()
	}
	if handler.DBHandler_ != nil {
		handler.DBHandler_.Close()
	}
}

func NewHandler() *RequestHandler {
	return &RequestHandler{
		mysqlHandler_:    mysqlmanager.NewMysqlManager(2),
		producerManager_: producermanager.NewProducerManager(2),
		redisHandler_: redis.NewClient(&redis.Options{
			Addr:     "127.0.0.1:6379",
			Password: "", // no password set
			DB:       0,
		}),
		DBHandler_:         dbmanager.NewDBManager("./imageDB"),
		Logged_in_accounts: make(map[string]bool),
	}
}

func (handler *RequestHandler) HandleData(singleRequest *SigngleRequest) {
	if handler.mysqlHandler_ == nil {
		fmt.Println("nil mysqlHandler")
		return
	}
	request := singleRequest.Request_
	if request.GetLoginRequest() != nil {
		handler.loginLogic(singleRequest)
	} else if request.GetRegisterRequest() != nil {
		handler.registerLogic(singleRequest)
	} else if request.GetUserInfo() != nil {
		handler.InfoLogic(singleRequest)
	} else if request.GetSearchRequest() != nil {
		handler.searchLogic(singleRequest)
	} else if request.GetAddRequest() != nil {
		handler.addLogic(singleRequest)
	} else if request.GetModifyRequest() != nil {
		handler.modifyInfoLogic(singleRequest)
	} else if request.GetImageRequest() != nil {
		handler.imageLogic(singleRequest)
	} else if request.GetUpdateImage() != nil {
		handler.updateImageLogic(singleRequest)
	} else if request.GetVerifyRequest() != nil {
		handler.verifyLogic(singleRequest)
	} else if request.GetFriendRequest() != nil {
		handler.pullFriendLogic(singleRequest)
	} else if request.GetMessageRequest() != nil {
		handler.messageLogic(singleRequest)
	} else if request.GetDeleteRequest() != nil {
		handler.deleteLogic(singleRequest)
	}
}

func (handler *RequestHandler) deleteLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	deleteR := request.GetDeleteRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	res := handler.mysqlHandler_.GetOnlyIsSuccess("delete from friends where (user_id=? and friend_id=?) or (user_id=? and friend_id=?);", singleRequest.Conn_.Id_,
		deleteR.Target, deleteR.Target, singleRequest.Conn_.Id_)
	if res {
		respond2 := &pb.PullRespond{
			RequestType: &pb.PullRespond_ServerPush{
				ServerPush: &pb.ServerPush{
					RequestType: &pb.ServerPush_DeleteRequest{
						DeleteRequest: &pb.DeleteFriendRequest{
							UserId: request.GetDeleteRequest().Target,
							Target: singleRequest.Conn_.Id_,
						},
					},
				},
			},
		}
		err := handler.producerManager_.HandleMessage(fmt.Sprintf("user-message-%s", deleteR.Target), []byte("delete"), mergeData(respond2, constant.MESSAGE))
		res = (err == nil)
		if err != nil {
			fmt.Println("delete error:", err)
		}
	}
	respond.GetSimpleRespond().Success = res
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) messageLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	message := request.GetMessageRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	if message.Type == pb.MessageRequest_Image {
		batch := new(leveldb.Batch)
		batch.Put([]byte(message.GetImageMessage().Md5), message.GetImageMessage().Data)
		batch.Put([]byte(message.GetImageMessage().Md5+"_hd"), message.GetImageMessage().DataHD)
		wo := &opt.WriteOptions{Sync: true}
		err := handler.DBHandler_.WriteImage(batch, wo)
		if err != nil {
			fmt.Println("messageLogic error:", err)
		}
		message.GetImageMessage().DataHD = nil
	}
	respond2 := &pb.PullRespond{
		RequestType: &pb.PullRespond_ServerPush{
			ServerPush: &pb.ServerPush{
				RequestType: &pb.ServerPush_MessageRequest{
					MessageRequest: &pb.MessageRequest{
						Target:      singleRequest.Conn_.Id_,
						Type:        message.Type,
						RequestType: message.RequestType,
					},
				},
			},
		},
	}
	err := handler.producerManager_.HandleMessage(fmt.Sprintf("user-message-%s", message.Target), []byte("message"),
		mergeData(respond2, constant.MESSAGE))
	respond.GetSimpleRespond().Success = (err == nil)
	if err != nil {
		respond.GetSimpleRespond().Msg = err.Error()
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) pullFriendLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	friendr := request.GetFriendRequest()
	userArray := []*pb.UserInfoRespond{}

	query := "SELECT u.account,u.name,u.email,u.createtime,u.sex " +
		"FROM userinfo u " +
		"JOIN friends f ON u.account = f.friend_id " +
		"WHERE f.user_id = ? AND f.status = 2;"
	result := handler.mysqlHandler_.ExecMysqlTask(query, friendr.UserId)

	var account, name, email, createtime string
	var sex bool
	for result.Next() {
		err := result.Scan(&account, &name, &email, &createtime, &sex)
		if err != nil {
			log.Println("pullFriendLogic error:", err)
		} else {
			userArray = append(userArray, &pb.UserInfoRespond{
				Account:    account,
				Username:   name,
				Email:      email,
				Createtime: createtime,
				Sex:        sex,
			})
		}
	}

	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_FriendRespond{
			FriendRespond: &pb.FriendRespond{
				Infos: userArray,
			},
		},
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) verifyLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	verify := request.GetVerifyRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	if verify.Vtype == pb.VerifyRequest_FRIEND {
		var query string
		if verify.Type != constant.Ignore {
			query = "update friends set status=? where user_id in (?,?) and friend_id in (?,?);"
			respond.GetSimpleRespond().Success = handler.mysqlHandler_.GetOnlyIsSuccess(query, verify.Type, verify.UserId, singleRequest.Conn_.Id_,
				verify.UserId, singleRequest.Conn_.Id_)
			tar := verify.UserId
			verify.UserId = singleRequest.Conn_.Id_

			respond2 := &pb.PullRespond{
				RequestType: &pb.PullRespond_ServerPush{
					ServerPush: &pb.ServerPush{
						RequestType: &pb.ServerPush_VerifyRequest{
							VerifyRequest: verify,
						},
					},
				},
			}
			handler.producerManager_.HandleMessage(fmt.Sprintf("user-message-%s", tar), []byte("apply"), mergeData(respond2, constant.MESSAGE))
		} else {
			query = "update friends set status=? where user_id=? and friend_id=?;"
			respond.GetSimpleRespond().Success = handler.mysqlHandler_.GetOnlyIsSuccess(query, verify.Type, singleRequest.Conn_.Id_,
				verify.UserId)
		}
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) updateImageLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	updateR := request.GetUpdateImage()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	batch := new(leveldb.Batch)
	if updateR.Data != nil {
		batch.Put([]byte(updateR.GetUrl()), updateR.Data)
	}
	if updateR.DataHD != nil {
		batch.Put([]byte(updateR.GetUrl()+"_hd"), updateR.DataHD)
	}
	wo := &opt.WriteOptions{Sync: true}
	err := handler.DBHandler_.WriteImage(batch, wo)

	respond.GetSimpleRespond().Success = (err == nil)
	if err != nil {
		fmt.Println("image error:", err)
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) imageLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	image := request.GetImageRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_ImageRespond{
			ImageRespond: &pb.ImageRespond{
				Data: []byte(""),
			},
		},
	}
	imageData, err := handler.DBHandler_.GetImage(image.Url)
	if err != nil {
		fmt.Println("image error:", err, image.Url)
	}

	respond.GetImageRespond().Data = imageData
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) modifyInfoLogic(singleRequest *SigngleRequest) {
	modify := singleRequest.Request_.GetModifyRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	query := "update userinfo set name=?,email=?,sex=? where account=?;"
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	handler.redisHandler_.HSet(ctx, modify.UserId, "name", modify.Name, "email", modify.Email, "sex", modify.Sex)
	respond.GetSimpleRespond().Success = handler.mysqlHandler_.GetOnlyIsSuccess(query, modify.Name, modify.Email, modify.Sex, modify.UserId)

	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func (handler *RequestHandler) addLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	add := request.GetAddRequest()
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: &pb.SimpleRespond{
				Success: false,
				Msg:     "",
			},
		},
	}
	respond2 := &pb.PullRespond{
		RequestType: &pb.PullRespond_ServerPush{
			ServerPush: &pb.ServerPush{
				RequestType: &pb.ServerPush_AddRequest{
					AddRequest: &pb.AddRequest{
						UserId:              add.FriendId,
						FriendId:            add.UserId,
						VerificationMessage: add.VerificationMessage,
					},
				},
			},
		},
	}
	data := mergeData(respond2, singleRequest.Type_)
	tx, err := handler.mysqlHandler_.Connections_.Begin()
	if err != nil {
		fmt.Println(err)
	} else {
		defer tx.Rollback()
		query := "call AddFriendship(?,?,?,?);"
		_, merror := tx.Exec(query, singleRequest.Conn_.Id_,
			add.FriendId, constant.Sent, constant.Pending)
		if merror == nil {
			err = handler.producerManager_.HandleMessage(fmt.Sprintf("user-message-%s", add.FriendId), []byte("apply"), data)
			if err == nil {
				respond.GetSimpleRespond().Success = true
				tx.Commit()
			} else {
				tx.Rollback()
				fmt.Println(err)
			}
		} else {
			tx.Rollback()
		}
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func consumeUserMessages(conn *constant.Connection) {
	ctx := context.Background()
	defer conn.KafkaConsumer.Close()
	for {
		message, err := conn.KafkaConsumer.ReadMessage(ctx)

		if err != nil {
			fmt.Println(err)
			if kafkaErr, ok := err.(kafka.Error); ok && kafkaErr == kafka.NotCoordinatorForGroup {
				fmt.Println("Coordinator changed, retrying...")
				time.Sleep(1 * time.Second) // 等待协调器稳定
				continue
			}
			if errors.Is(err, io.EOF) {
				return
			}
			continue
		}
		// 将消息推送到用户连接
		SendRespond(conn, message.Value)
	}
}

func (handler *RequestHandler) searchLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	search := request.GetSearchRequest()
	if search.Type == pb.SearchRequest_FRIEND {

		var query string
		var result *sql.Rows
		if search.Mode == pb.SearchRequest_ACCOUNT {
			query = "SELECT u.account, u.name,u.email,u.createtime,u.sex FROM userinfo u WHERE u.account = ? " +
				"AND u.account <> ? " +
				"AND NOT EXISTS ( " +
				"SELECT 1 FROM friends f " +
				"WHERE ((f.user_id = ? AND f.friend_id = u.account) " +
				"OR (f.user_id = u.account AND f.friend_id = ?)) AND (f.status in (1,2))) " +
				"AND NOT EXISTS( " +
				"SELECT 1 FROM friends f2 JOIN friends f3 " +
				"ON f2.user_id = f3.friend_id AND f2.friend_id = f3.user_id " +
				"WHERE f2.user_id = ? AND f2.friend_id = u.account " +
				"AND f2.status = 1 AND f3.status = 4) LIMIT 30 OFFSET ?;"
			result = handler.mysqlHandler_.ExecMysqlTask(query, search.Content, singleRequest.Conn_.Id_,
				singleRequest.Conn_.Id_, singleRequest.Conn_.Id_, singleRequest.Conn_.Id_, search.Offset)
		} else if search.Mode == pb.SearchRequest_NAME {
			query = "SELECT u.account,u.name,u.email,u.createtime,u.sex " +
				"FROM userinfo u " +
				"WHERE u.name LIKE '%" + search.Content + "%' " +
				"AND u.account <> ? " +
				"AND NOT EXISTS ( " +
				"SELECT 1 FROM friends f " +
				"WHERE ((f.user_id = ? AND f.friend_id = u.account) " +
				"OR (f.user_id = u.account AND f.friend_id = ?)) AND (f.status in (1,2))) " +
				"AND NOT EXISTS( " +
				"SELECT 1 FROM friends f2 JOIN friends f3 " +
				"ON f2.user_id = f3.friend_id AND f2.friend_id = f3.user_id " +
				"WHERE f2.user_id = ? AND f2.friend_id = u.account " +
				"AND f2.status = 1 AND f3.status = 4) LIMIT 30 OFFSET ?;"
			result = handler.mysqlHandler_.ExecMysqlTask(query, singleRequest.Conn_.Id_,
				singleRequest.Conn_.Id_, singleRequest.Conn_.Id_, singleRequest.Conn_.Id_, search.Offset)
		}

		if result == nil {
			return
		}
		defer result.Close()
		var account, name, email, createtime string
		var sex bool

		userArray := []*pb.UserInfoRespond{}
		ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
		defer cancel()
		for result.Next() {
			err := result.Scan(&account, &name, &email, &createtime, &sex)
			if err != nil {
				log.Println("login error:", err)
			} else {
				userArray = append(userArray, &pb.UserInfoRespond{
					Account:    account,
					Username:   name,
					Email:      email,
					Createtime: createtime,
					Sex:        sex,
				})
				handler.redisHandler_.HSet(ctx, account, "name", name, "email", email,
					"createtime", createtime, "sex", sex)
			}
		}

		respond := &pb.PullRespond{
			RequestType: &pb.PullRespond_SearchRespond{
				SearchRespond: &pb.SearchRespond{
					Info: userArray,
				},
			},
		}
		SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))

	} else if search.Type == pb.SearchRequest_GROUP {

	}
}

// 登录
func (handler *RequestHandler) loginLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	login := request.GetLoginRequest()
	ctx := context.Background()

	loginResp := &pb.SimpleRespond{
		Success: true,
		Msg:     "",
	}
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: loginResp,
		},
	}

	rpwd, err := handler.redisHandler_.HGet(ctx, login.Account, "password").Result()
	if err == nil {
		loginResp.Success = (rpwd == login.Password)
	} else {
		query := "select password,name,email,createtime,sex from userinfo where account=?;"
		result := handler.mysqlHandler_.ExecMysqlTask(query, login.Account)
		if result == nil {
			return
		}
		defer result.Close()

		var pwd, name, email, createtime, sex string

		for result.Next() {
			err := result.Scan(&pwd, &name, &email, &createtime, &sex)
			if err != nil {
				log.Println("login error:", err)
				respond.GetSimpleRespond().Success = false
			}
		}
		if pwd != request.GetLoginRequest().Password {
			respond.GetSimpleRespond().Success = false
		}

		handler.redisHandler_.HSet(ctx, login.Account, "name", name, "email", email,
			"createtime", createtime, "sex", sex)
	}

	if handler.Logged_in_accounts[login.Account] {
		respond.GetSimpleRespond().Success = false
		respond.GetSimpleRespond().Msg = "logged"
	}

	if respond.GetSimpleRespond().Success {
		handler.Logged_in_accounts[login.Account] = true
		reader := kafka.NewReader(kafka.ReaderConfig{
			Brokers:  []string{"127.0.0.1:9092"},
			Topic:    fmt.Sprintf("user-message-%s", login.Account), // 用户消息队列
			GroupID:  fmt.Sprintf("im-server-%s", login.Account),    // 服务器实例ID
			MinBytes: 1,
			MaxBytes: 10e6,
		})
		reader.SetOffset(0)
		singleRequest.Conn_.KafkaConsumer = reader
		singleRequest.Conn_.Id_ = login.Account

		go consumeUserMessages(singleRequest.Conn_)
	}

	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
} //登录

// 注册
func (handler *RequestHandler) registerLogic(singleRequest *SigngleRequest) {
	request := singleRequest.Request_
	register := request.GetRegisterRequest()
	loginResp := &pb.SimpleRespond{
		Success: false,
		Msg:     "",
	}
	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_SimpleRespond{
			SimpleRespond: loginResp,
		},
	}
	result := handler.mysqlHandler_.ExecMysqlTask("CALL insert_userinfo(?, ?, ?);",
		register.Username, register.Password, register.Email)
	if result == nil {
		return
	}
	defer result.Close()
	var insert_id string
	if result.Next() {
		err := result.Scan(&insert_id)
		if err != nil {
			log.Println("register error:", err)
			respond.GetSimpleRespond().Success = false
		} else {
			respond.GetSimpleRespond().Success = true
			respond.GetSimpleRespond().Msg = insert_id
		}
	}

	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))

} //注册

func (handler *RequestHandler) InfoLogic(singleRequest *SigngleRequest) {
	acc := singleRequest.Request_.GetUserInfo().Account

	info := &pb.UserInfoRespond{
		Account: acc,
	}
	var name, email, createtime string
	var sex bool
	var err error
	ctx := context.Background()

	name, err = handler.redisHandler_.HGet(ctx, acc, "name").Result()
	email = handler.redisHandler_.HGet(ctx, acc, "email").Val()
	createtime = handler.redisHandler_.HGet(ctx, acc, "createtime").Val()
	if handler.redisHandler_.HGet(ctx, acc, "sex").Val() == "0" {
		sex = false
	} else {
		sex = true
	}

	if err == redis.Nil || err != nil {
		result := handler.mysqlHandler_.ExecMysqlTask("select name,email,createtime,sex from userinfo where account=?;",
			acc)

		if result == nil {
			return
		}
		defer result.Close()
		if result.Next() {
			err := result.Scan(&name, &email, &createtime, &sex)
			if err != nil {
				log.Println("userinfo error:", err)
			} else {
				info.Username = name
				info.Email = email
				info.Createtime = createtime
				info.Sex = sex
			}
		}
	} else {
		info.Username = name
		info.Email = email
		info.Createtime = createtime
		info.Sex = sex
	}

	respond := &pb.PullRespond{
		RequestType: &pb.PullRespond_UserInfo{
			UserInfo: info,
		},
	}
	SendRespond(singleRequest.Conn_, mergeData(respond, singleRequest.Type_))
}

func SendRespond(conn *constant.Connection, data []byte) {

	conn.Mutex_.Lock()
	defer conn.Mutex_.Unlock()
	if !conn.IsClosed && conn.Conn_ != nil {
		conn.Conn_.Write(data)
	}
}

func mergeData(respond *pb.PullRespond, headerType constant.HeaderType) []byte {
	data, err := proto.Marshal(respond)
	if err != nil {
		log.Println("Marshal fail:", err)
		return nil
	}
	h := constant.CustomHeader{
		Type_:     headerType,
		DataSize_: (uint32)(len(data)),
	}
	head := make([]byte, 8)
	binary.BigEndian.PutUint32(head[0:4], uint32(h.Type_))
	binary.BigEndian.PutUint32(head[4:8], uint32(h.DataSize_))
	return append(head, data...)
}
