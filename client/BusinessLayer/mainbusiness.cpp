#include "mainbusiness.h"
#include "IM_login.pb.h"
#include "DataLayer/datamanager.h"
#include "constants.h"
#include <QDataStream>
#include <QFile>
#include <QDebug>

MainBusiness::MainBusiness(QObject *parent)
    : QObject{parent}
{
    network_=new NetworkManager();
    thread_=new QThread(this);
    network_->moveToThread(thread_);
    thread_->start();
    connect(network_,&NetworkManager::respondDataSig,this,&MainBusiness::handleRespondData);
    connect(network_,&NetworkManager::serverPushSig,this,&MainBusiness::handleServerPush);

    connect(DataManager::getInstance(),&DataManager::getImageSig,this,[this](QString id,RequestCallbcak callback){
        PullRequest req;
        req.mutable_image_request()->set_url(id.toStdString());
        sendData(req,IMAGE_GET,callback);
    },Qt::QueuedConnection);
    connect(DataManager::getInstance(),&DataManager::getInfoSig,[&](QString id,RequestCallbcak callback){
        PullRequest req;
        req.mutable_user_info()->set_account(id.toStdString());
        sendData(req,USERINFO,callback);
    });

    QMetaObject::invokeMethod(network_,"init");
}

MainBusiness::~MainBusiness()
{
    QMetaObject::invokeMethod(network_,"destory",Qt::BlockingQueuedConnection);
    thread_->quit();
    thread_->wait();
}

void MainBusiness::sendData(PullRequest &data, HeaderType type,RequestCallbcak callbacks)
{
    uint32_t id=0;
    QMetaObject::invokeMethod(network_,"sendData",Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(uint32_t,id)
                              ,Q_ARG(PullRequest&,data)
                              ,Q_ARG(HeaderType,type));
    tasks_[id]=callbacks;
}

void MainBusiness::getUserInfoWithCallback(QString account,HeaderType type,RequestCallbcak callback)
{
    PullRequest request;
    request.mutable_user_info()->set_account(account.toStdString());
    sendData(request,type,callback);
}

void MainBusiness::handleRespondData(PullRespond respond, uint32_t id)
{
    if(!tasks_.contains(id)){
        qDebug()<<"not contains task:"<<id;
        return;
    }
    if(!respond.ByteSizeLong()){
        if(tasks_[id].timeoutFunc_)
            tasks_[id].timeoutFunc_();
    } else {
        if(tasks_[id].successFunc_){
            tasks_[id].successFunc_(std::move(respond));
        }
    }
    tasks_.remove(id);
}

void MainBusiness::handleServerPush(PullRespond respond)
{
    emit serverPushSig(respond);
}
