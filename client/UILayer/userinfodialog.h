#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include "unresizewindowsbackgruond.h"
#include "customizequickwidget.h"
#include <QQuickItem>
#include "constants.h"

enum class DialogType{
    MYSELF,
    STRANGER,
    FRIEND,
    SENT
};
enum class ChangeInfoType{
    USERINFO,
    IMAGE
};

class UserInfoDialog : public UnresizeWindowsBackgruond
{
    Q_OBJECT
public:
    UserInfoDialog(QWidget *parent=nullptr,DialogType type=DialogType::MYSELF);
    void setInfo(QString account,QString name,QString email,QString createTime,Sex sex);
    void showInfo(QString text,captionType type,int duration);
    void changeInfoSuccess(ChangeInfoType type=ChangeInfoType::USERINFO);
    void changeType(DialogType type);
    void setLoadding(bool val);

    int index2SearchDia=-1;
    bool loading_=false;

private slots:
    void onClose();
    void onChange(QString name,QString email,int sex);
    void onModifyAvator();
    void onTalk(QString account);
    void onAdd(QString account,QString name);
    void onCheckImage();
    void onModifyBackImage(int width,int height);
    void onDelete(QString target);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
    DialogType type_;
    QString currentAccount_="";
signals:
    void readyClose();
    void changeInfoSig(QString account,QString name,QString email,int sex);
    void updateImageSig(QString id,QImage image,bool compressed);
    void talkSig(QString account);
    void doAdd(QString target,QString content,int index2SearchDia);
    void checkImageSig(QString id);//查看高清原图
    void deleteFriendSig(QString target);
};

#endif // USERINFODIALOG_H
