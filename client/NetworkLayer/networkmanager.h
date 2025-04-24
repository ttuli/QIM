#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QQueue>
#include <QFutureWatcher>
#include <QEventLoop>
#include "constants.h"
#include "IM_login.pb.h"

class NetWorkTask{
public:
    NetWorkTask(uint32_t id,QString timestamp)
        :id_(id),timestamp_(timestamp)
    {}
    uint32_t id_;
    QString timestamp_;
};

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    bool isTimeout(QString timeStr);

public slots:
    uint32_t sendData(PullRequest &data, HeaderType type);
    void handleData();
    void init();
    void destory();

private:
    QTcpSocket *socket_;

    QByteArray currentData_;
    CustomHeader header_;
    QFutureWatcher<PullRespond> watcher_;
    QEventLoop loop_;
    QEventLoop exitLoop_;

    QVector<NetWorkTask> taskOrder_;
    QSet<uint32_t> ids_;
    QTimer *timeout_;
signals:
    void respondDataSig(PullRespond respond,uint32_t id);
    void serverPushSig(PullRespond respond);
};

#endif // NETWORKMANAGER_H
