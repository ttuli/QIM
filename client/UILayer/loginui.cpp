#include "loginui.h"
#include <QHBoxLayout>
#include <QSettings>
#include <QTimer>
#include <QFile>

LoginUI::LoginUI(QWidget *parent)
    : UnresizeWindowsBackgruond{parent}
{
    interface_=new CustomizeQuickWidget(this);
    interface_->setSource(QUrl("qrc:/Login.qml"));
    interfaceItem_=interface_->rootObject();
    interfaceItem_->setProperty("maxWaitTime",MAX_WAIT_TIME);
    connect(interfaceItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(onClose()));
    connect(interfaceItem_,SIGNAL(registerSig()),this,SLOT(onRegister()));
    connect(interfaceItem_,SIGNAL(loginSig(QString,QString,bool,bool)),this,SLOT(onLogin(QString,QString,bool,bool)));

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->addWidget(interface_);
    qhb->setContentsMargins(0,0,0,0);

    setMinimumSize(WIDTH,HEIGHT);
    setMaximumSize(WIDTH,HEIGHT);
    setWindowFlag(Qt::Window);

    QTimer::singleShot(200,[this]{
        readInit();
    });
}

void LoginUI::showCaption(QString text,captionType type,int duration)
{
    QMetaObject::invokeMethod(interfaceItem_,"showCaption",
                              Q_ARG(QVariant,text)
                              ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
    if(type==captionType::SUCCESS){
        emit successSig(currentAccount_);
        close();
    }
}

void LoginUI::setInfo(QString account, QString password, bool remberPwd, bool autoLogin, bool changeCheck)
{
    QMetaObject::invokeMethod(interfaceItem_,"setInfo",
                              Q_ARG(QVariant,account)
                              ,Q_ARG(QVariant,password)
                              ,Q_ARG(QVariant,remberPwd)
                              ,Q_ARG(QVariant,autoLogin)
                              ,Q_ARG(QVariant,changeCheck));
}

void LoginUI::readInit()
{
    QFile file("user.ini");
    file.open(QIODevice::Append);
    file.close();
    QSettings setting("user.ini");

    QMetaObject::invokeMethod(interfaceItem_,"setInfo",
                              Q_ARG(QVariant,QByteArray::fromBase64(setting.value("account").toByteArray()))
                              ,Q_ARG(QVariant,QByteArray::fromBase64(setting.value("password").toByteArray()))
                              ,Q_ARG(QVariant,setting.value("rember").toBool())
                              ,Q_ARG(QVariant,setting.value("auto").toBool())
                              ,Q_ARG(QVariant,true));
}

void LoginUI::writeInit(QString account, QString password)
{
    if (!remberPwd_)
        password = "";
    QSettings setting("user.ini");
    setting.setValue("account",account.toUtf8().toBase64());
    setting.setValue("password",password.toUtf8().toBase64());
    setting.setValue("rember",remberPwd_);
    setting.setValue("auto",autoLogin_);
}

void LoginUI::onClose()
{
    emit readyCloseSig();
}

void LoginUI::onRegister()
{
    emit registerSig();
}

void LoginUI::onLogin(QString account, QString password, bool remberPwd, bool autoLogin)
{
    currentAccount_=account;
    remberPwd_=remberPwd;
    autoLogin_=autoLogin;
    emit loginSig(account,password);
}
