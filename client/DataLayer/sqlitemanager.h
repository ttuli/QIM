#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QtConcurrent>
#include <QSqlQuery>
#include <QCache>
#include <QImage>
#include <functional>
#include <QVector>
#include "constants.h"
#include "UILayer/Model/friendverifymodel.h"
#include "UILayer/Model/chatlistmodel.h"

class SqliteManager : public QObject
{
    Q_OBJECT
public:
    explicit SqliteManager(QObject *parent = nullptr);
    ~SqliteManager();

public slots:
    void writeUserInfo(GeneralInfo *info);
    void writeImageInfo(QString key,QImage image);
    void writeChatList(QList<chatListInfo> list);
    void writeChatMessage(QString account,ChatMessage message);
    QImage getSingleImage(const QString &key);
    void writeVerifyInfo(QList<verifyInfo> list);

    //初始化部分
    void init(QString account);
    void readUserInfo_init();
    void readImageInfo_init();
    void readVerifyInfo_init();
    void readChatList_init();
    //初始化部分

    void readChatMessage(QString target,int offset,QVector<ChatMessage> &results);
    bool removeChatMessage(QString uid,std::function<void(bool)> callback);

private:
    QSqlDatabase infoDB_;
    QSqlDatabase chatDB_;
    QSqlQuery infoQuery_;
    QSqlQuery imageQuery_;
    QSqlQuery chatMessageQuery_;

    QString currentAccount_="";
signals:
    void loadedImageSig(QString key,QImage image);
    void loadVerifySig(QList<verifyInfo> list);
    void loadUserInfoSig(QString account,GeneralInfo info);
    void loadChatListSig(QList<chatListInfo> list);
};

#endif // SQLITEMANAGER_H
