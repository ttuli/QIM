package filehandler

import (
	constant "IMserver/Constant"
	pb "IMserver/IM_protobuf"
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"os"
	"sync"

	"google.golang.org/protobuf/proto"
)

type FileHandler struct {
	FileName_        string
	CurrentFileSize_ int64
	TotalFileSize_   int64
	Conn_            net.Conn
	File_            *os.File
	mutex_           sync.Mutex
}

func NewFileHandler(conn net.Conn) *FileHandler {
	return &FileHandler{
		FileName_:        "",
		CurrentFileSize_: -1,
		TotalFileSize_:   -1,
		Conn_:            conn,
		File_:            nil,
	}
}

func (fileHandler *FileHandler) HandleFileTransfer(request *pb.FileStruct) {
	if request == nil {
		fileHandler.Conn_.Close()
		return
	}
	respond := &pb.FileStruct{
		Id: request.GetId(),
	}
	if request.GetInfo() != nil {
		fileHandler.TotalFileSize_ = request.GetInfo().GetFileSize()
		fileHandler.CurrentFileSize_ = 0
		respond.RequestType = &pb.FileStruct_Respond{
			Respond: &pb.FileRespond{
				Result: true,
				Type:   pb.TransferType_TRANSFER_TYPE_FILE_INFO,
			},
		}
		fileHandler.File_, _ = os.OpenFile("Files/"+request.GetId(), os.O_WRONLY|os.O_CREATE|os.O_APPEND, 0666)
	} else if request.GetTransfer() != nil {
		if request.GetTransfer().GetStatus() == pb.TransferStatus_TRANSFER_STATUS_CANCELED || fileHandler.File_ == nil {
			fileHandler.CurrentFileSize_ = 0
			fileHandler.TotalFileSize_ = 0
			respond.RequestType = &pb.FileStruct_Respond{
				Respond: &pb.FileRespond{
					Result: true,
					Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
					Status: pb.TransferStatus_TRANSFER_STATUS_CANCELED,
				},
			}
			fileHandler.File_.Close()
			os.Remove("Files/" + request.GetId())
			fileHandler.File_ = nil
			fileHandler.SendRespond(mergeData_File(respond, constant.FILEMSG))
			return
		}

		fileHandler.CurrentFileSize_ += int64(len(request.GetTransfer().Data))
		_, err := fileHandler.File_.Write(request.GetTransfer().Data)
		respond.RequestType = &pb.FileStruct_Respond{
			Respond: &pb.FileRespond{
				Result: err == nil,
				Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
				Status: pb.TransferStatus_TRANSFER_STATUS_SENDING,
			},
		}

		if fileHandler.CurrentFileSize_ == fileHandler.TotalFileSize_ {
			fmt.Println("file transfer complete")
			respond.RequestType = &pb.FileStruct_Respond{
				Respond: &pb.FileRespond{
					Result: true,
					Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
					Status: pb.TransferStatus_TRANSFER_STATUS_COMPLETED,
				},
			}
			fileHandler.CurrentFileSize_ = 0
			fileHandler.TotalFileSize_ = 0
			fileHandler.File_.Close()
			fileHandler.File_ = nil
		}
	} else if request.GetRequest() != nil {
		if fileHandler.File_ == nil {
			var err error
			fileHandler.File_, err = os.OpenFile("Files/"+request.GetId(), os.O_RDONLY, 0666)
			if err != nil {
				fmt.Println("open file fail:", err)
				respond.RequestType = &pb.FileStruct_Respond{
					Respond: &pb.FileRespond{
						Result: false,
						Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
						Status: pb.TransferStatus_TRANSFER_STATUS_FAILED,
					},
				}
				fileHandler.SendRespond(mergeData_File(respond, constant.FILEMSG))
				return
			}
			fileInfo, err := os.Stat("Files/" + request.GetId())
			if err != nil {
				fmt.Println("get file info fail:", err)
				respond.RequestType = &pb.FileStruct_Respond{
					Respond: &pb.FileRespond{
						Result: false,
						Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
						Status: pb.TransferStatus_TRANSFER_STATUS_FAILED,
					},
				}
				fileHandler.SendRespond(mergeData_File(respond, constant.FILEMSG))
				return
			}
			f := &pb.FileStruct{
				Id: request.GetId(),
				RequestType: &pb.FileStruct_Info{
					Info: &pb.FileInfo{
						FileSize: fileInfo.Size(),
					},
				},
			}
			fmt.Println("file size:", fileInfo.Size())
			fileHandler.SendRespond(mergeData_File(f, constant.FILEMSG))

			if fileInfo.Size() <= 1024*1024*10 {
				if fileInfo.Size() <= 1024*1024*10 {
					// 使用 bufio 来优化读取性能
					reader := bufio.NewReader(fileHandler.File_)
					buffer := make([]byte, 32*1024) // 32KB 的缓冲区大小，可以根据需求调整

					var chunks [][]byte

					for {
						n, err := reader.Read(buffer)
						if n > 0 {
							// 复制读取的数据块（只复制实际读取的部分）
							chunk := make([]byte, n)
							copy(chunk, buffer[:n])
							chunks = append(chunks, chunk)
						}

						if err == io.EOF {
							break
						}
						if err != nil {
							// 处理错误
							fmt.Printf("文件读取错误: %v", err)
							return
						}
					}

					// 合并所有数据块
					totalSize := 0
					for _, chunk := range chunks {
						totalSize += len(chunk)
					}

					allData := make([]byte, totalSize)
					offset := 0
					for _, chunk := range chunks {
						copy(allData[offset:], chunk)
						offset += len(chunk)
					}

					respond.RequestType = &pb.FileStruct_Request{
						Request: &pb.FileRequest{
							Data: allData,
						},
					}
					fileHandler.SendRespond(mergeData_File(respond, constant.FILEMSG))
				}
			}
		}
		fbyte := make([]byte, constant.SendChunkSize)

		_, err := fileHandler.File_.Read(fbyte)
		respond.RequestType = &pb.FileStruct_Request{
			Request: &pb.FileRequest{
				Data: fbyte,
			},
		}
		if err == io.EOF {
			respond.RequestType = &pb.FileStruct_Respond{
				Respond: &pb.FileRespond{
					Result: true,
					Type:   pb.TransferType_TRANSFER_TYPE_FILE_DATA,
					Status: pb.TransferStatus_TRANSFER_STATUS_COMPLETED,
				},
			}
			os.Remove("Files/" + request.GetId())
			fileHandler.File_.Close()
			fileHandler.File_ = nil
		}

	}
	fileHandler.SendRespond(mergeData_File(respond, constant.FILEMSG))
}

func (fileHandler *FileHandler) SendRespond(data []byte) {

	fileHandler.mutex_.Lock()
	defer fileHandler.mutex_.Unlock()
	_, err := fileHandler.Conn_.Write(data)
	if err != nil {
		fmt.Println("send respond fail:", err)
	}
}

func mergeData_File(respond *pb.FileStruct, headerType constant.HeaderType) []byte {
	data, err := proto.Marshal(respond)
	if err != nil {
		fmt.Println("Marshal fail:", err)
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
