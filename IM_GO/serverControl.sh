#!/bin/bash

# Start Zookeeper
echo "Starting Zookeeper..."
/root/share/kafka/bin/zookeeper-server-start.sh  /root/share/kafka/bin/config/zookeeper.properties &
if [ $? -ne 0 ]; then
    echo "Failed to start Zookeeper"
    exit 1
fi

# Wait for Zookeeper to start
sleep 3

# Start Kafka
echo "Starting Kafka..."
/root/share/kafka/bin/kafka-server-start.sh /root/share/kafka/bin/config/server.properties &
if [ $? -ne 0 ]; then
    echo "Failed to start Kafka"
    exit 1
fi

# Wait for Kafka to start
sleep 3

# Check if both services are running
zookeeper_status=$(jps | grep -i QuorumPeerMain)
kafka_status=$(jps | grep -i Kafka)

if [[ -n "$zookeeper_status" && -n "$kafka_status" ]]; then
    echo "Zookeeper and Kafka started successfully"
else
    echo "Failed to start Zookeeper or Kafka"
    exit 1
fi