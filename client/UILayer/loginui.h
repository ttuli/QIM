#ifndef LOGINUI_H
#define LOGINUI_H

#include <QWidget>
#include <QQuickItem>
#include "customizequickwidget.h"
#include "unresizewindowsbackgruond.h"
#include "constants.h"

#define WIDTH 340
#define HEIGHT 480

class LoginUI : public UnresizeWindowsBackgruond
{
    Q_OBJECT
public:
    explicit LoginUI(QWidget *parent = nullptr);
    void showCaption(QString text,captionType type,int duration);
    void setInfo(QString account,QString password,bool remberPwd,bool autoLogin,bool changeCheck);
    void writeInit(QString account,QString password);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
    QString currentAccount_="";
    bool remberPwd_,autoLogin_;

private:
    void readInit();

private slots:
    void onClose();
    void onRegister();
    void onLogin(QString account,QString password,bool remberPwd,bool autoLogin);

signals:
    void readyCloseSig();
    void registerSig();
    void loginSig(QString account,QString password);
    void successSig(QString account);
};

#endif // LOGINUI_H
