#ifndef MAINBUSINESS_H
#define MAINBUSINESS_H

#include <QObject>
#include <QMap>
#include <QThread>
#include "IM_login.pb.h"
#include "constants.h"
#include "NetworkLayer/networkmanager.h"

class MainBusiness : public QObject
{
    Q_OBJECT
public:
    explicit MainBusiness(QObject *parent = nullptr);
    ~MainBusiness();
    void sendData(PullRequest &data,HeaderType type,RequestCallbcak callbacks);
    void getUserInfoWithCallback(QString account,HeaderType type,RequestCallbcak callback);

public slots:

private slots:
    void handleRespondData(PullRespond respond,uint32_t id);
    void handleServerPush(PullRespond respond);

private:
    NetworkManager *network_;
    QThread *thread_;
    QMap<uint32_t,RequestCallbcak> tasks_;

signals:
    void serverPushSig(PullRespond respond);
};

#endif // MAINBUSINESS_H
