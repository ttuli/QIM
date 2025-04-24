#include "chatmanager.h"
#include <QHBoxLayout>
#include <QQmlContext>
#include "constants.h"

ChatManager::ChatManager(QWidget *parent,ChatListModel *chatListModel)
    : QWidget{parent},model_(chatListModel)
{
    chatList_=new CustomizeQuickWidget(this);
    chatList_->rootContext()->setContextProperty("chatListModel",model_);
    chatList_->rootContext()->setContextProperty("manager",this);
    chatList_->setSource(QUrl("qrc:/ChatState/ChatList.qml"));
    chatList_->setMinimumWidth(130);
    chatList_->setMaximumWidth(width()/3);
    chatListItem_=chatList_->rootObject();

    pageControl_=new QStackedWidget(this);

    QSplitter *splitter=new QSplitter(this);
    splitter->setHandleWidth(1);
    splitter->setChildrenCollapsible(0);
    splitter->addWidget(chatList_);
    splitter->addWidget(pageControl_);

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(splitter);
}

ChatManager::~ChatManager()
{
    while(pageControl_->count()){
        pageControl_->removeWidget(pageControl_->widget(0));
    }
}

void ChatManager::setChatDialog(ChatDialog *dia)
{
    if(pageControl_->indexOf(dia)==-1){
        pageControl_->addWidget(dia);
    }
    pageControl_->setCurrentWidget(dia);
    currentChatDialogAccount=dia->getCurrentAccount();
}

void ChatManager::removeChatDialog(ChatDialog *dia)
{
    int index=pageControl_->indexOf(dia);
    if(index!=-1){
        pageControl_->widget(index)->deleteLater();
    }
    pageControl_->removeWidget(dia);
}

void ChatManager::onTalk(QString account, QString name)
{
    emit createChatDialogSig(account,name);
}

void ChatManager::resizeEvent(QResizeEvent *event)
{
    chatList_->setMaximumWidth(event->size().width()/3);
}
