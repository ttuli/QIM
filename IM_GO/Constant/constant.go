package constant

import (
	"net"
	"sync"

	kafka "github.com/segmentio/kafka-go"
)

const (
	LOGIN = iota
	REGISTER
	PULLFRIEND
	USERINFO
	SEARCH
	ADD
	MESSAGE
	INFOCHANGE
	IMAGE_UPDATE
	IMAGE_GET
	VERIFY
	SENDMSG
	FILEMSG
	DELETEMSG
)

var SendChunkSize = 1024 * 1024 * 3

type HeaderType uint32

type CustomHeader struct {
	Type_     HeaderType
	DataSize_ uint32
}

type Connection struct {
	Conn_         net.Conn
	KafkaConsumer *kafka.Reader
	Id_           string
	Mutex_        sync.Mutex
	IsClosed      bool
}

type ApplyStatus int

const (
	NONE = iota
	Pending
	Accepted
	Rejected
	Sent
	Ignore
	Deleted
)
