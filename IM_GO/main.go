package main

import (
	connectionmanager "IMserver/ConnectionManager"
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	listener, err := net.Listen("tcp", ":50051")
	if err != nil {
		log.Fatalf("fail to start server:%v", err)
	}

	stop := false
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	manager := connectionmanager.NewConnectionManager()
	go func() {
		for !stop {
			conn, err := listener.Accept()
			if err != nil {
				log.Println("accept fail!")
				continue
			}
			go manager.HandleConnection(conn)
		}
	}()
	log.Println("server start")

	defer listener.Close()
	defer manager.CloseAllConnection()
	<-sigChan
	stop = true
	log.Println("server stop")
}
