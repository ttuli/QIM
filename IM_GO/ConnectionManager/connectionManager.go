package connectionmanager

import (
	constant "IMserver/Constant"
	fhandler "IMserver/FileHandler"
	pb "IMserver/IM_protobuf"
	handler "IMserver/RequestHandler"
	"unsafe"

	"encoding/binary"
	"fmt"
	"io"
	"net"
	"sync"

	proto "google.golang.org/protobuf/proto"
)

type ConnectionManager struct {
	connections map[string]*constant.Connection
	execRequest *handler.RequestHandler
	fileTasks   map[string]*fhandler.FileHandler
	mutex       sync.RWMutex
}

func NewConnectionManager() *ConnectionManager {
	return &ConnectionManager{
		connections: make(map[string]*constant.Connection),
		execRequest: handler.NewHandler(),
		fileTasks:   make(map[string]*fhandler.FileHandler),
	}
}

func (cm *ConnectionManager) CloseAllConnection() {
	cm.mutex.Lock()
	for k, v := range cm.connections {
		v.Conn_.Close()
		if v.KafkaConsumer != nil {
			v.KafkaConsumer.Close()
		}
		delete(cm.connections, k)
	}
	for _, v := range cm.fileTasks {
		v.HandleFileTransfer(nil)
		v.Conn_.Close()
	}
	cm.mutex.Unlock()
}

func (cm *ConnectionManager) AddConnection(conn net.Conn) {
	cm.mutex.Lock()
	defer cm.mutex.Unlock()

	uConn := &constant.Connection{
		Conn_:    conn,
		IsClosed: false,
	}

	cm.connections[conn.RemoteAddr().String()] = uConn
	fmt.Printf("新连接: %s\n", conn.RemoteAddr())
}
func (cm *ConnectionManager) getConnection(key string) *constant.Connection {
	return cm.connections[key]
}
func (cm *ConnectionManager) removeConnection(key string) {
	cm.mutex.Lock()
	cm.connections[key].Conn_.Close()
	cm.connections[key].IsClosed = true
	cm.execRequest.Logged_in_accounts[cm.connections[key].Id_] = false
	if cm.connections[key].KafkaConsumer != nil {
		cm.connections[key].KafkaConsumer.Close()
	}
	delete(cm.connections, key)
	cm.mutex.Unlock()
	fmt.Println("连接断开:", key)
}

func (manager *ConnectionManager) HandleConnection(conn net.Conn) {
	manager.AddConnection(conn)
	Conn := manager.getConnection(conn.RemoteAddr().String())

	header := constant.CustomHeader{}

	for {
		buffer := make([]byte, unsafe.Sizeof(header))
		n, err := conn.Read(buffer)
		if err != nil || n == 0 {
			if err == io.EOF {
				manager.removeConnection(conn.RemoteAddr().String())
				break
			}
			continue
		}

		header.Type_ = constant.HeaderType(binary.BigEndian.Uint32(buffer[:4]))
		header.DataSize_ = binary.BigEndian.Uint32(buffer[4:8])

		buffer = make([]byte, header.DataSize_)
		n, err = io.ReadFull(conn, buffer)
		if err != nil || n == 0 {
			if err == io.EOF {
				manager.removeConnection(conn.RemoteAddr().String())
				manager.fileTasks[conn.RemoteAddr().String()].HandleFileTransfer(nil)
				manager.mutex.Lock()
				delete(manager.fileTasks, conn.RemoteAddr().String())
				manager.mutex.Unlock()
				break
			}
			continue
		}

		if header.Type_ == constant.FILEMSG {
			request := &pb.FileStruct{}
			if err := proto.Unmarshal(buffer, request); err != nil {
				fmt.Printf("Unmarshal fail:%v", err)
				continue
			}
			if manager.fileTasks[conn.RemoteAddr().String()] == nil {
				manager.mutex.Lock()
				manager.fileTasks[conn.RemoteAddr().String()] = fhandler.NewFileHandler(conn)
				manager.mutex.Unlock()
			}
			manager.fileTasks[conn.RemoteAddr().String()].HandleFileTransfer(request)

		} else {
			request := &pb.PullRequest{}
			if err := proto.Unmarshal(buffer, request); err != nil {
				fmt.Printf("Unmarshal fail:%v", err)
				continue
			}
			go manager.execRequest.HandleData(&handler.SigngleRequest{
				Type_:    header.Type_,
				Request_: request,
				Conn_:    Conn,
			})
		}

	}
}
