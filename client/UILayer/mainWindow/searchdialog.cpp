#include "searchdialog.h"
#include "friendrequestdialog.h"
#include <QHBoxLayout>
#include <QQmlContext>

SearchDialog::SearchDialog(QWidget *parent,SearchModel_F *model)
    :WindowsBackground(parent),model_(model)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("searchModel",model);
    interface_->setSource(QUrl("qrc:/Search.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(close()));
    connect(interfaceItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(interfaceItem_,SIGNAL(searchSig(QString,int,QString,int)),this,SLOT(onSearch(QString,int,QString,int)));
    connect(interfaceItem_,SIGNAL(addSig(QString,QString,int)),this,SLOT(onAdd(QString,QString,int)));
    connect(interfaceItem_,SIGNAL(clearSig()),model,SLOT(clear()));
    connect(interfaceItem_,SIGNAL(userInfoSig(QString,int)),this,SLOT(onUserInfoSig(QString,int)));

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(interface_);

    setMinimumSize(640,480);
}

void SearchDialog::showInformation(QString msg, captionType type, int duration)
{
    QMetaObject::invokeMethod(interfaceItem_,"setMsg"
                              ,Q_ARG(QVariant,msg)
                              ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
}

void SearchDialog::setIsGetAll(bool val)
{
    isGetAll=val;
}

void SearchDialog::onSearch(QString type, int mode, QString content,int offs)
{
    if(!offs)isGetAll=false;
    if(isGetAll)return;
    emit doSearch(type,mode,content,offs*searchSingleTime);
}

void SearchDialog::onAdd(QString target,QString name, int index)
{
    FriendRequestDialog dia(this,target,name);
    if(dia.exec()==QDialog::Rejected)
        return;
    QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,true));
    emit doAdd(target,dia.content_,index);
}

void SearchDialog::onUserInfoSig(QString account,int index)
{
    emit newUserInfoDia(account,index);
}

void SearchDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(leftButton_){
        move(windowp_+event->globalPosition().toPoint()-cursorp_);
    }
}

void SearchDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        leftButton_=true;
        cursorp_=event->globalPosition().toPoint();
        windowp_=pos();
    }
}

void SearchDialog::mouseReleaseEvent(QMouseEvent *event)
{
    leftButton_=false;
}
