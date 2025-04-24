package kafkaproducers

import (
	"context"
	"errors"
	"fmt"
	"time"

	kafka "github.com/segmentio/kafka-go"
)

type ProducerManager struct {
	producers          []*kafka.Writer
	writerNB, writerLB int
}

func (pm *ProducerManager) Close() {
	if pm == nil {
		return
	}
	for _, producer := range pm.producers {
		if producer != nil {
			producer.Close()
		}
	}
}

func (pm *ProducerManager) HandleMessage(topic string, key, value []byte) error {
	if pm == nil {
		return errors.New("ProducerManager is nil")
	}

	conn, err := kafka.Dial("tcp", "127.0.0.1:9092")
	if err != nil {
		return fmt.Errorf("failed to dial leader: %w", err)
	}
	defer conn.Close()
	err = conn.CreateTopics(kafka.TopicConfig{
		Topic:             topic,
		NumPartitions:     1,
		ReplicationFactor: 1,
	})
	if err != nil {
		return fmt.Errorf("failed to create topic: %w", err)
	}

	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	pm.writerLB = (pm.writerLB + 1) % pm.writerNB
	return pm.producers[pm.writerLB].WriteMessages(ctx, kafka.Message{
		Key:   key,
		Value: value,
		Topic: topic,
	})
}

func NewProducerManager(writernb int) *ProducerManager {
	p := &ProducerManager{
		producers: make([]*kafka.Writer, 0),
		writerNB:  writernb,
		writerLB:  0,
	}
	for i := 0; i < writernb; i++ {
		p.producers = append(p.producers, kafka.NewWriter(kafka.WriterConfig{
			Brokers:  []string{""},
			Balancer: &kafka.LeastBytes{},
		}))
	}
	return p
}
