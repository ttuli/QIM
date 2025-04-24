#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QMap>
#include <QImage>
#include <QEventLoop>
#include <QCache>
#include <QQueue>
#include <QMap>
#include <functional>
#include <QThread>
#include "constants.h"
#include "sqlitemanager.h"
#include "UILayer/Model/friendverifymodel.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager(const DataManager&)=delete;
    DataManager operator =(const DataManager&)=delete;

    static DataManager* getInstance();

    void addInfo(QString account,GeneralInfoType type,ApplyState state,User &data);
    GeneralInfo* getInfo(QString id,GeneralInfoType type=GeneralInfoType::FRIEND);
    GeneralInfo* syncGetInfo(QString id,SyncUserInfoCallback callback,GeneralInfoType type=GeneralInfoType::FRIEND);

    void addImageCache(QString key,QImage image,bool writeLocal=true);
    void addImageCache(QString key,QByteArray data);
    QImage getImageCache(const QString &key);
    void getImageWithCallback(const QString &key,std::function<void(QImage)> callback_local,RequestCallbcak callback);
    bool removeImageCache(const QString &key);

    void writeVerifyInfo(QList<verifyInfo> list);
    void writeChatList(QList<chatListInfo> list);
    void writeChatMessage(QString account,ChatMessage message);
    void readChatMessage(QString target,int offset,QVector<ChatMessage> &result);

    bool removeMsg(QString uid,std::function<void(bool)> callback);

    void initLocalDB(QString account);

    QImage blankImage_;
    QImage blankImageCompress_;
private:
    QCache<QString,GeneralInfo> Info_;
    QCache<QString,QImage> imageCache_;
    //用户信息获取的队列，如果已经正在获取就会入队获取后一起执行,存放的是调用的函数
    QMap<QString,QQueue<SyncUserInfoCallback>> getInfoQueue_;

    DataManager(QObject *parent = nullptr);
    ~DataManager();

    QEventLoop loop_;
    SqliteManager *sqlite_;
    QThread *thread_;
signals:
    void getImageSig(QString id,RequestCallbcak callback);
    void getInfoSig(QString id,RequestCallbcak callback);

    void loadVerifyInfoSig(QList<verifyInfo> list);
    void loadChatListSig(QList<chatListInfo> list);
};

#endif // DATAMANAGER_H
