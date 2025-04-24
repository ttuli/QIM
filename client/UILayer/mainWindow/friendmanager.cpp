#include "friendmanager.h"
#include "UILayer/qmlsharedata.h"
#include <QHBoxLayout>
#include <QQmlContext>

FriendManager::FriendManager(QWidget *parent,FriendVerifyModel *model,FriendListModel *listmodel)
    : QWidget{parent},model_(model),listmodel_(listmodel)
{
    friendList_=new CustomizeQuickWidget(this);
    friendList_->rootContext()->setContextProperty("manager", this);
    friendList_->rootContext()->setContextProperty("friendListModel",listmodel);
    friendList_->rootContext()->setContextProperty("globalData",QmlShareData::getInstance());
    friendList_->setSource(QUrl("qrc:/mainWindow/FriendList.qml"));
    friendList_->setMaximumWidth(170);
    friendList_->setMinimumWidth(120);
    friendListItem_=friendList_->rootObject();

    applyInterface_=new CustomizeQuickWidget(this);
    applyInterface_->rootContext()->setContextProperty("friendVerifyModel",model);
    applyInterface_->rootContext()->setContextProperty("fmanager",this);
    applyInterface_->setSource(QUrl("qrc:/mainWindow/PendingRequest.qml"));

    blankPage_=new QWidget(this);
    blankPage_->setStyleSheet("background-color:#f9f7f7");

    pageControl_=new QStackedWidget(this);
    pageControl_->addWidget(blankPage_);
    pageControl_->addWidget(applyInterface_);
    pageControl_->setCurrentWidget(blankPage_);

    splitter_=new QSplitter(this);
    splitter_->setHandleWidth(1);
    splitter_->addWidget(friendList_);
    splitter_->addWidget(pageControl_);
    splitter_->setChildrenCollapsible(false);


    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(splitter_);

    connect(model_,&FriendVerifyModel::updateSig,this,&FriendManager::onPulseSet);
}

void FriendManager::onPulseSet()
{
    if(IM_Current_Window==currentWindow::FRIENDMANAGER
        &&pageControl_->currentWidget()==applyInterface_
        &&isActiveWindow()){
        model_->setPulseRemind();
        QmlShareData::getInstance()->setMsgnum_f_verify(-QmlShareData::getInstance()->msgnum_f_verify());
    } else {
        QmlShareData::getInstance()->setMsgnum_f_verify(1);
        if(!model_->soundEffect_->isPlaying())
            model_->soundEffect_->play();
    }
}

void FriendManager::setInfo(QString text, captionType type, int duration)
{
    QMetaObject::invokeMethod(applyInterface_->rootObject(),"showInfo",Q_ARG(QVariant,text)
                                  ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
}

void FriendManager::onIndexChanged(int position)
{
    if(position==(int)VERIFY){
        pageControl_->setCurrentWidget(applyInterface_);
        model_->setPulseRemind();
        QmlShareData::getInstance()->setMsgnum_f_verify(-QmlShareData::getInstance()->msgnum_f_verify());
    }
}

void FriendManager::onReact(QString target,int type)
{
    emit updateRelationship(target,type);
}

void FriendManager::onNewDialog(QString account)
{
    emit newUserInfoDialog(account);
}


