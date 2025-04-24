#include "friendrequestdialog.h"
#include <QVBoxLayout>

FriendRequestDialog::FriendRequestDialog(QWidget *parent,QString account,QString name)
    :FramlessDialog(parent),account_(account),name_(name)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->setSource(QUrl("qrc:/mainWindow/FriendRequest.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(reject()));
    connect(interfaceItem_,SIGNAL(sendSig(QString)),this,SLOT(onApply(QString)));

    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);

    setMinimumSize(400,600);
    setMaximumSize(400,600);
    QMetaObject::invokeMethod(interfaceItem_,"setInfo",Q_ARG(QVariant,name),Q_ARG(QVariant,account));
}

void FriendRequestDialog::onApply(QString content)
{
    content_=content;
    accept();
}
