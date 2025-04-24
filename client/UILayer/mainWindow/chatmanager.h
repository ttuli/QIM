#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QWidget>
#include <QStackedWidget>
#include <QSplitter>
#include "UILayer/customizequickwidget.h"
#include <QQuickItem>
#include <QResizeEvent>
#include <QCache>
#include "UILayer/Model/chatlistmodel.h"
#include "chatdialog.h"

class ChatManager : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentA READ currentA WRITE setCurrentA NOTIFY currentAChanged FINAL)
public:
    explicit ChatManager(QWidget *parent = nullptr,ChatListModel *chatListModel=nullptr);
    ~ChatManager();
    void setChatDialog(ChatDialog *dia);
    void removeChatDialog(ChatDialog *dia);

public slots:
    QString currentA(){return currentA_;}
    void setCurrentA(const QString &val){currentA_=val;emit currentAChanged();}

    void onTalk(QString account,QString name);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QStackedWidget *pageControl_;

    CustomizeQuickWidget *chatList_;
    QQuickItem *chatListItem_;

    ChatListModel *model_;

    QString currentA_="";
signals:
    void currentAChanged();
    void createChatDialogSig(QString account,QString name);
};

#endif // CHATMANAGER_H
