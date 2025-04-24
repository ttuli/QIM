#include "register_dialog.h"
#include <QQuickWidget>
#include <QQuickItem>
#include <QTimer>

RegisterDialog::RegisterDialog(QWidget *parent)
    :UnresizeWindowsBackgruond(parent)
{
    setWindowFlag(Qt::Dialog);
    setWindowModality(Qt::WindowModal);

    registerInterface_=new CustomizeQuickWidget(this);
    registerInterface_->setSource(QUrl("qrc:/Register.qml"));

    registerInterfaceItem_=registerInterface_->rootObject();
    connect(registerInterfaceItem_,SIGNAL(closeSig()),this,SLOT(close()));
    connect(registerInterfaceItem_,SIGNAL(registerSig(QString,QString,QString)),this,SLOT(doRegister(QString,QString,QString)));

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(registerInterface_);

    setMinimumSize(400,500);
    setMaximumSize(400,500);
}

RegisterDialog::~RegisterDialog()
{
}

void RegisterDialog::showInfo(QString content, captionType type, int duration)
{
    QMetaObject::invokeMethod(registerInterfaceItem_,"showInfo",
                              Q_ARG(QVariant,content)
                              ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
}


void RegisterDialog::doRegister(QString username, QString pwd, QString email)
{
    emit registerSig(username,pwd,email);
}

void RegisterDialog::onClose()
{
    QMetaObject::invokeMethod(registerInterfaceItem_,"init");
    close();
}

