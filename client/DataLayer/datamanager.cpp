#include "datamanager.h"
#include "qobjectdefs.h"
#include <QEventLoop>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <functional>

DataManager::DataManager(QObject *parent)
    : QObject{parent},blankImage_(":/res/pexels-stywo-1054218.jpg")
{
    Info_.setMaxCost(1024*1024*50);
    imageCache_.setMaxCost(1024*1024*250);

    blankImageCompress_=blankImage_.scaledToHeight(ImageMaxLength,Qt::SmoothTransformation);

    sqlite_=new SqliteManager;
    thread_=new QThread(this);
    sqlite_->moveToThread(thread_);
    thread_->start();
    connect(sqlite_,&SqliteManager::loadedImageSig,[this](QString key,QImage image){
        addImageCache(key,image,false);
    });
    connect(sqlite_,&SqliteManager::loadVerifySig,[&](QList<verifyInfo> list){
        for(int i=0;i<list.size();i++){
            GeneralInfo *g=getInfo(list[i].account_);
            if(!g)
                continue;
            list[i].name_=g->userData.name_;
            list[i].state_=g->state_;
        }
        emit loadVerifyInfoSig(list);
    });
    connect(sqlite_,&SqliteManager::loadChatListSig,this,[&](QList<chatListInfo> list){
        for(int i=0;i<list.size();i++){
            GeneralInfo *g=getInfo(list[i].info_.userData.account_);
            if(!g)
                continue;
            list[i].info_.userData.name_=g->userData.name_;
            list[i].info_.state_=g->state_;
        }
        emit loadChatListSig(list);
        loop_.quit();
    });
    connect(sqlite_,&SqliteManager::loadUserInfoSig,this,[this](QString account,GeneralInfo info){
        Info_.insert(account,new GeneralInfo(info));
    });
}

DataManager::~DataManager()
{
    thread_->quit();
    thread_->wait();
}

DataManager *DataManager::getInstance()
{
    static DataManager obj;
    return &obj;
}

void DataManager::addInfo(QString account,GeneralInfoType type,ApplyState state,User &data)
{
    if(!Info_.contains(account)){
        GeneralInfo g;
        g.type_=type;
        g.state_=state;
        g.userData=std::move(data);
        Info_.insert(account,new GeneralInfo(g));
    } else {
        GeneralInfo *g=Info_.object(account);
        if(state!=ApplyState::NONE){
            g->state_=state;
        }
        g->userData=std::move(data);
    }
    QMetaObject::invokeMethod(sqlite_,"writeUserInfo",Q_ARG(GeneralInfo*,Info_.object(account)));
}

GeneralInfo *DataManager::getInfo(QString id,GeneralInfoType type)
{
    if(Info_.contains(id)&&Info_.object(id)->type_==type){
        return Info_.object(id);
    }
    return nullptr;
}

GeneralInfo *DataManager::syncGetInfo(QString id,SyncUserInfoCallback callback,GeneralInfoType type)
{
    GeneralInfo *tmp=getInfo(id,type);
    if(tmp)
        return tmp;
    if(getInfoQueue_[id].empty()){
        if(type==GeneralInfoType::FRIEND){
            emit getInfoSig(id,RequestCallbcak{
                [this,id](PullRespond respond){
                    if(respond.has_user_info()){
                        User user;
                        user.account_=id;
                        user.createTime_=QString::fromStdString(respond.user_info().createtime());
                        user.email_=QString::fromStdString(respond.user_info().email());
                        user.name_=QString::fromStdString(respond.user_info().username());
                        user.sex_=(Sex)respond.user_info().sex();
                        addInfo(id,GeneralInfoType::FRIEND,ApplyState::NONE,user);
                        QQueue<SyncUserInfoCallback> &queue=getInfoQueue_[id];
                        while(queue.size()){
                            if(queue.front().ifGet_)
                                queue.dequeue().ifGet_();
                            else queue.dequeue();
                        }
                    } else {
                        QQueue<SyncUserInfoCallback> &queue=getInfoQueue_[id];
                        while(queue.size()){
                            if(queue.front().ifNotGet_)
                                queue.dequeue().ifNotGet_();
                            else queue.dequeue();
                        }
                    }
                },
                [this,id]{
                    QQueue<SyncUserInfoCallback> &queue=getInfoQueue_[id];
                    while(queue.size()){
                        if(queue.front().ifNotGet_)
                            queue.dequeue().ifNotGet_();
                        else queue.dequeue();
                    }
                }
            });
        }
    }
    getInfoQueue_[id].enqueue(callback);
    return nullptr;
}

void DataManager::addImageCache(QString key,QImage image,bool writeLocal)
{
    if(QThread::currentThread()!=this->thread()){
        QMetaObject::invokeMethod(this,[=]{
            addImageCache(key,image,writeLocal);
        });
        return;
    }
    if(writeLocal){
        QMetaObject::invokeMethod(sqlite_,"writeImageInfo",Q_ARG(QString,key),Q_ARG(QImage,image));
    }
    if(image.isNull()){
        if(key.last(2)=="hd")
            image=blankImage_;
        else
            image=blankImageCompress_;
    }
    imageCache_.insert(key,new QImage(image));
}

void DataManager::addImageCache(QString key, QByteArray data)
{
    QEventLoop loop;
    QFuture<QImage> future=QtConcurrent::run([&]{
        QImage ima;
        ima.loadFromData(data);
        return ima;
    });
    QFutureWatcher<QImage> watcher;
    watcher.setFuture(future);
    connect(&watcher,&QFutureWatcher<QImage>::finished,&loop,&QEventLoop::quit);
    loop.exec();

    addImageCache(key,future.takeResult());
}

QImage DataManager::getImageCache(const QString &key)
{
    if(imageCache_.contains(key)){
        return *imageCache_.object(key);
    }
    if(key=="")
        return blankImage_;
    return QImage();
}

void DataManager::getImageWithCallback(const QString &key,std::function<void(QImage)> callback_local,RequestCallbcak callback)
{
    QImage image;
    QMetaObject::invokeMethod(sqlite_,"getSingleImage",Qt::BlockingQueuedConnection
                              ,Q_RETURN_ARG(QImage,image)
                              ,Q_ARG(const QString&,key));
    if(!image.isNull()){
        callback_local(image);
        return;
    }
    if(QThread::currentThread()!=this->thread()){
        QMetaObject::invokeMethod(this,[=]{
            emit getImageSig(key,callback);
        });
    } else {
        emit getImageSig(key,callback);
    }
}

bool DataManager::removeImageCache(const QString &key)
{
    return imageCache_.remove(key);
}

void DataManager::writeVerifyInfo(QList<verifyInfo> list)
{
    QMetaObject::invokeMethod(sqlite_,"writeVerifyInfo",Q_ARG(QList<verifyInfo>,list));
}

void DataManager::writeChatList(QList<chatListInfo> list)
{
    QMetaObject::invokeMethod(sqlite_,"writeChatList",Q_ARG(QList<chatListInfo>,list));
}

void DataManager::writeChatMessage(QString account, ChatMessage message)
{
    QMetaObject::invokeMethod(sqlite_,"writeChatMessage",Q_ARG(QString,account)
                              ,Q_ARG(ChatMessage,message));
}

void DataManager::readChatMessage(QString target, int offset,QVector<ChatMessage> &result)
{
    QMetaObject::invokeMethod(sqlite_,"readChatMessage",Qt::BlockingQueuedConnection
                              ,Q_ARG(QString,target)
                              ,Q_ARG(int,offset)
                              ,Q_ARG(QVector<ChatMessage>&,result));
}

bool DataManager::removeMsg(QString uid,std::function<void(bool)> callback)
{
    bool res=false;
    QMetaObject::invokeMethod(sqlite_,"removeChatMessage"
                              ,Q_ARG(QString,uid)
                              ,Q_ARG(std::function<void(bool)>,callback));
    return res;
}

void DataManager::initLocalDB(QString account)
{
    QMetaObject::invokeMethod(sqlite_,"init",Q_ARG(QString,account));
    QMetaObject::invokeMethod(sqlite_,"readUserInfo_init");
    QMetaObject::invokeMethod(sqlite_,"readImageInfo_init");
    QMetaObject::invokeMethod(sqlite_,"readVerifyInfo_init");
    QMetaObject::invokeMethod(sqlite_,"readChatList_init");
    loop_.exec();//确保读完历史数据再进行其他，防止争夺出现ui异常
}
