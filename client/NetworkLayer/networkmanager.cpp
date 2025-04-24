#include "networkmanager.h"
#include "constants.h"
#include <QSharedPointer>
#include <QDateTime>
#include <QRandomGenerator>

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
{
}

void NetworkManager::init()
{
    socket_=new QTcpSocket(this);
    connect(socket_,&QTcpSocket::connected,[this]{
        IM_connectState=true;
    });
    connect(socket_,&QTcpSocket::errorOccurred,[this]{
        IM_connectState=false;
        if(socket_->state()!=QAbstractSocket::ConnectingState&&socket_->state()!=QAbstractSocket::ConnectedState)
            socket_->connectToHost(IM_serverAddr,IM_port);
    });
    connect(socket_,&QTcpSocket::readyRead,this,&NetworkManager::handleData);
    socket_->connectToHost(IM_serverAddr,IM_port);

    timeout_=new QTimer(this);
    timeout_->setInterval(1000);
    connect(timeout_,&QTimer::timeout,this,[&]{
        if(taskOrder_.size()&&isTimeout(taskOrder_.front().timestamp_)){
            emit respondDataSig(PullRespond(),taskOrder_.front().id_);
            ids_.remove(taskOrder_.front().id_);
            taskOrder_.pop_front();
        }
        if(taskOrder_.isEmpty())
            timeout_->stop();
    });

    header_.dataSize_=-1;
}

void NetworkManager::destory()
{
    timeout_->stop();
    socket_->close();
    delete this;
}

uint32_t NetworkManager::sendData(PullRequest &data, HeaderType type)
{
    QByteArray bytes;
    uint32_t randomId=QRandomGenerator::global()->generate();
    while(ids_.contains(randomId)){
        randomId=QRandomGenerator::global()->generate();
    }
    std::string sdata;
    data.SerializeToString(&sdata);
    CustomHeader h;
    h.type_=randomId;
    h.dataSize_=sdata.size();

    QByteArray header;
    QDataStream stream(&header,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<h.type_<<h.dataSize_;
    bytes=header+QByteArray::fromStdString(sdata);

    QString timestamp=QDateTime::currentDateTime().toString("m-s-z");
    ids_.insert(randomId);
    taskOrder_.append(NetWorkTask(randomId,timestamp));
    QSharedPointer<QByteArray> dataCopy=QSharedPointer<QByteArray>::create(std::move(bytes));

    if(socket_->state()==QAbstractSocket::ConnectedState){
        socket_->write(*dataCopy);
    }
    if(!timeout_->isActive())
        timeout_->start();
    return randomId;
}


void NetworkManager::handleData()
{
    if(header_.dataSize_==-1&&socket_->bytesAvailable()<sizeof(CustomHeader))
        return;
    if(header_.dataSize_==-1){
        QByteArray header=socket_->read(sizeof(CustomHeader));
        QDataStream stream(&header,QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream>>header_.type_>>header_.dataSize_;
    }
    int resByte=header_.dataSize_-currentData_.size();
    int availableByte=socket_->bytesAvailable();
    currentData_+=socket_->read((availableByte>=resByte?resByte:availableByte));

    if(currentData_.size()==header_.dataSize_){
        PullRespond res;
        res.ParseFromString(currentData_.toStdString());
        timeout_->stop();
        if(!res.has_server_push()){
            emit respondDataSig(res,header_.type_);
            for(int i=0;i<taskOrder_.size();++i){
                if(taskOrder_[i].id_==header_.type_){
                    taskOrder_.removeAt(i);
                    ids_.remove(header_.type_);
                    break;
                }
            }
        } else {
            emit serverPushSig(res);
        }
        header_.dataSize_=-1;
        header_.type_=0;
        currentData_.clear();
        if(socket_->bytesAvailable())
            handleData();
    }
}

bool NetworkManager::isTimeout(QString timeStr)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QStringList parts = timeStr.split('-');
    if (parts.size() != 3) {
        return true;
    }

    int minute = parts[0].toInt();
    int second = parts[1].toInt();
    int msec = parts[2].toInt();

    QDateTime pastTime = QDateTime::currentDateTime();
    pastTime.setTime(QTime(pastTime.time().hour(), minute, second, msec));

    if (currentTime.time().minute() < minute &&
        currentTime.time().minute() == 0 && minute == 59) {
        // 如果当前是0分，而过去是59分，说明跨越了小时边界
        pastTime = pastTime.addSecs(-3600);
    }

    qint64 diffMSecs = pastTime.msecsTo(currentTime);

    return diffMSecs > MAX_WAIT_TIME;
}
