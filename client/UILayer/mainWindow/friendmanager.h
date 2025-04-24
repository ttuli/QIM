#ifndef FRIENDMANAGER_H
#define FRIENDMANAGER_H

#include <QWidget>
#include <QSplitter>
#include <QSet>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QQuickItem>
#include "UILayer/customizequickwidget.h"
#include "UILayer/Model/friendverifymodel.h"
#include "UILayer/Model/friendlistmodel.h"

class FriendManager : public QWidget
{
    Q_OBJECT
public:
    enum{
        BLANK=0,
        VERIFY=1,
    };

    explicit FriendManager(QWidget *parent = nullptr,FriendVerifyModel *model=nullptr,FriendListModel *listmodel=nullptr);

    void setInfo(QString text,captionType type,int duration);

public slots:
    void onIndexChanged(int position);
    void onReact(QString target,int type);
    void onNewDialog(QString account);
    void onPulseSet();

private:
    QSplitter *splitter_;
    QStackedWidget *pageControl_;
    QWidget *blankPage_;
    CustomizeQuickWidget *applyInterface_;

    CustomizeQuickWidget *friendList_;
    QQuickItem *friendListItem_;

    FriendVerifyModel *model_;
    FriendListModel *listmodel_;

    int msgnum_f_verify_=0;
signals:
    void updateRelationship(QString account,int type);
    void newUserInfoDialog(QString account);
};

#endif // FRIENDMANAGER_H
