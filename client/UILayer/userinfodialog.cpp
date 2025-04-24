#include "userinfodialog.h"
#include "modifyavatordialog.h"
#include <QQmlContext>
#include <QHBoxLayout>
#include "confirmactiondialog.h"
#include "mainWindow/friendrequestdialog.h"

UserInfoDialog::UserInfoDialog(QWidget *parent,DialogType type)
    :UnresizeWindowsBackgruond(parent),type_(type)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("_MYSELF",(int)DialogType::MYSELF);
    interface_->rootContext()->setContextProperty("_FRIEND",(int)DialogType::FRIEND);
    interface_->rootContext()->setContextProperty("_STRANGER",(int)DialogType::STRANGER);
    interface_->rootContext()->setContextProperty("_SENT",(int)DialogType::SENT);
    interface_->setSource(QUrl("qrc:/UserInfoDialog.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(onClose()));
    connect(interfaceItem_,SIGNAL(changeInfoSig(QString,QString,int)),this,SLOT(onChange(QString,QString,int)));
    connect(interfaceItem_,SIGNAL(modifyAvatorSig()),this,SLOT(onModifyAvator()));
    connect(interfaceItem_,SIGNAL(talkSig(QString)),this,SLOT(onTalk(QString)));
    connect(interfaceItem_,SIGNAL(addSig(QString,QString)),this,SLOT(onAdd(QString,QString)));
    connect(interfaceItem_,SIGNAL(checkImageSig()),this,SLOT(onCheckImage()));
    connect(interfaceItem_,SIGNAL(modifyBackImaSig(int,int)),this,SLOT(onModifyBackImage(int,int)));
    connect(interfaceItem_,SIGNAL(deleteSig(QString)),this,SLOT(onDelete(QString)));

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(interface_);
    setMinimumSize(500,650);
    setMaximumSize(500,650);

    setWindowFlag(Qt::Window);

    QMetaObject::invokeMethod(interfaceItem_,"init",Q_ARG(QVariant,(int)type));
}

void UserInfoDialog::setInfo(QString account, QString name, QString email, QString createTime, Sex sex)
{
    currentAccount_=account;
    QMetaObject::invokeMethod(interfaceItem_,"setInfo",Q_ARG(QVariant,account)
                              ,Q_ARG(QVariant,name)
                              ,Q_ARG(QVariant,email)
                              ,Q_ARG(QVariant,createTime)
                              ,Q_ARG(QVariant,(bool)sex));
}

void UserInfoDialog::showInfo(QString text, captionType type, int duration)
{
    QMetaObject::invokeMethod(interfaceItem_,"showInfo"
                              ,Q_ARG(QVariant,text)
                              ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
}

void UserInfoDialog::changeInfoSuccess(ChangeInfoType type)
{
    if(type==ChangeInfoType::USERINFO)
        QMetaObject::invokeMethod(interfaceItem_,"changeInfoSuccess");
    if(type==ChangeInfoType::IMAGE)
        QMetaObject::invokeMethod(interfaceItem_,"refreshImage");
}

void UserInfoDialog::changeType(DialogType type)
{
    QMetaObject::invokeMethod(interfaceItem_,"init",Q_ARG(QVariant,(int)type));
}

void UserInfoDialog::setLoadding(bool val)
{
    interface_->rootObject()->setProperty("loading",val);
    loading_=val;
}

void UserInfoDialog::onClose()
{
    close();
    emit readyClose();
}

void UserInfoDialog::onChange(QString name, QString email, int sex)
{
    if(currentAccount_!="")
        emit changeInfoSig(currentAccount_,name,email,sex);
    else
        showInfo("修改失败!",captionType::FAIL,1200);
}

void UserInfoDialog::onModifyAvator()
{
    if(currentAccount_=="")
        return;
    ModifyAvatorDialog dia(this,QSize(390,390),"修改头像");
    if(dia.exec()==QDialog::Rejected)
        return;
    QMetaObject::invokeMethod(interfaceItem_,"waitForImaChange");

    emit updateImageSig(currentAccount_+"_avatar",dia.resultImage_,true);
}

void UserInfoDialog::onTalk(QString account)
{
    close();
    emit talkSig(account);
    emit readyClose();
}

void UserInfoDialog::onAdd(QString account,QString name)
{
    if(account==""||name=="")
        return;
    FriendRequestDialog dia(this,account,name);
    setLoadding(true);
    if(dia.exec()==QDialog::Accepted){
        showInfo("正在发送申请...",captionType::LOADDING,MAX_WAIT_TIME);
        emit doAdd(account,dia.content_,index2SearchDia);
    }else setLoadding(false);
}

void UserInfoDialog::onCheckImage()
{
    emit checkImageSig(currentAccount_+"_avatar_hd");
}

void UserInfoDialog::onModifyBackImage(int width, int height)
{
    ModifyAvatorDialog dia(this,QSize(width,height),"修改背景");
    if(dia.exec()==QDialog::Rejected)
        return;
    QMetaObject::invokeMethod(interfaceItem_,"waitForImaChange");

    emit updateImageSig(currentAccount_+"_background",dia.resultImage_,false);
}

void UserInfoDialog::onDelete(QString target)
{
    ConfirmActionDialog dia(this,"确定删除好友？",true,true);
    if(dia.exec()==QDialog::Accepted)
        emit deleteFriendSig(target);
}
