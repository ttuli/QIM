#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QCache>
#include <QQueue>
#include <QSoundEffect>
#include "loginui.h"
#include "userinfodialog.h"
#include "register_dialog.h"
#include "BusinessLayer/mainbusiness.h"
#include "mainWindow/mainwindow.h"
#include "Model/searchmodel.h"
#include "mainWindow/searchdialog.h"
#include "mainWindow/friendmanager.h"
#include "Model/friendverifymodel.h"
#include "Model/friendlistmodel.h"
#include "Model/chatlistmodel.h"
#include "mainWindow/chatdialog.h"
#include "PictureViewer/pictureviewmanager.h"
#include "FileManager/filemanager.h"

class UIManager : public QObject
{
    Q_OBJECT
public:
    explicit UIManager(QObject *parent = nullptr);
    ~UIManager();

    void start();
    void pullFriends(QString account);
    void pullSelfUserInfo();
private:
    QPointer<LoginUI> login_;
    RegisterDialog *register_;

    /*聊天窗口LRU*/
    QList<QString> enterOrder_;
    QMap<QString,ChatDialog*> chatDialogCache_;
    size_t totalSize;

    size_t calculateSize();
    /**/

    QPointer<MainWindow> mainWindow_;
    QPointer<FriendManager> friendManager_;
    FriendVerifyModel *friendVerifyModel_;
    FriendListModel *friendListModel_;
    QPointer<ChatManager> chatManager_;
    ChatListModel *chatListModel_;

    QSoundEffect *soundEffect_F_;
    QSoundEffect *soundEffect_MSG_;

    QPointer<SearchDialog> searchDialog_;
    SearchModel_F *searchModel_;

    MainBusiness *business_;

    QMap<QString,UserInfoDialog*> infoDialog_;
    PictureViewManager *picviewManager_;

    FileManager *fileManager_;
    QThread *fileThread_;

    QQueue<PullRespond> msgStore_;//窗口显示前接收的消息
private slots:
    void doClose();
    void onMainWindowShow();
    void handleLogin(QString account,QString password);
    void handleRegister(QString name,QString password,QString email);
    void onUserInfoDialog(QString account,DialogType type,int index2SearchDia=-1);
    void handleSearch(QString type, int mode, QString content,int offs);
    void handleAdd(QString target,QString content,int index);
    void handleServerPush(PullRespond respond);
    void handleChangeInfo(QString account,QString name,QString email,int sex);
    void handleUpdateImage(QString id,QImage image,bool compressed);
    void handleUpdateRelationship(QString target,int type);
    void handleDeleteFriend(QString account);
    void handleTalk(QString account);
    void handleSendFile(QString target,QString path);
    void handleSendFileMsg(QString target,ChatMessage msg);
    void handleSendMsg(QString target,QString content,int type);
    void handleSendPic(QString target);
    void handleSendPicData(QString target,QString md5,QImage image,QImage compressImage,PullRequest request);
    void createChatDialog(QString account,QString name,bool changeChatDia=true);
    void handleUpdateChatListAfterDel(QString account,QString content,QDateTime timestamp);
    void handleReadHistoryChatMsg(QString target,int offset);
    void handleSaveImage(QString id,QString savepath);

signals:
};

#endif // UIMANAGER_H
