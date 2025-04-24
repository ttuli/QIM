#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include "UILayer/customizequickwidget.h"
#include <QQuickItem>
#include "UILayer/Model/chatmessagemodel.h"

class ChatDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isNotFriend READ isNotFriend WRITE setIsNotFriend NOTIFY isNotFriendChanged FINAL)
public:
    explicit ChatDialog(QWidget *parent = nullptr,QString account="",QString name="",QString loginName="");

    QString addMsg(ChatMessage message,int position=-1);
    void updateInfo(const QString &id,ChatMessageModel::Roles role,QVariant data,bool writeLocal=true);
    void setBlock(bool visible);
    void removeMsg(int position);
    void showInfo(QString content,captionType type,int duration);
    ChatMessage getMessage(QString id,int position=-1);

    QString getCurrentAccount(){return currentAccount_;}
    void setOffset(int val){
        if(hasPullAll)
            return;
        if(val==-1){
            hasPullAll=true;
            return;
        }
        currentOffset_+=val;
    }

public slots:
    void handleSendMsg(QString content,int type);
    void handleSendPic();
    void handleSendFile();
    void pullHistoryMsg();
    void handleCheckHdPic(QString id);
    void readyRemoveMsg(int position,QString uid);
    void handleFileCancel(QString id,int index);
    void handleFileDownLoad(QString id,int index);

    bool isNotFriend(){return isNotFriend_;}
    void setIsNotFriend(bool val){isNotFriend_=val;emit isNotFriendChanged();}
private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

    QString currentAccount_;
    QString currentName_;

    QString loginName_;

    int currentOffset_=0;
    bool hasPullAll=false;
    bool isNotFriend_=false;

    ChatMessageModel model_;
signals:
    void sendMsgSig(QString account,QString content,int type);
    void sendPicSig(QString target);
    void writeHistoryMsgSig(QString account,ChatMessage message);
    void loadHistoryMsg(QString account,int offset);
    void removeMsgSig(int position,QString uid);
    void updateChatListAfterDelSig(QString account,QString content,QDateTime timestamp);
    void checkHdPicSig(QString id);
    void sendFileSig(QString id,QString filepath);
    void cancelFileSig(QString id);
    void downloadFileSig(QString target,QString id,QString savePath);

    void isNotFriendChanged();
};

#endif // CHATDIALOG_H
