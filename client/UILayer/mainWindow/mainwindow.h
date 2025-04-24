#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "UILayer/windows_background.h"
#include <QQuickItem>
#include "UILayer/customizequickwidget.h"
#include <QHBoxLayout>
#include "UILayer/userinfodialog.h"
#include "friendmanager.h"
#include <QSystemTrayIcon>
#include <QStackedWidget>
#include "chatmanager.h"

class MainWindow : public WindowsBackground
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL)
public:
    MainWindow(QWidget *parent=nullptr,FriendManager* friendManager=nullptr,ChatManager *chatManager=nullptr);
    ~MainWindow();
    void updateUserImage();

protected:
    void showEvent(QShowEvent *) override;

public slots:
    void init(QString account);
    void setCurrentIndex(int index);
    int currentIndex(){return currentIndex_;}

private slots:
    void onCheckMyInfo();
    void onMax();
    void onSearch();

private:
    void setSystemTray();

private:
    CustomizeQuickWidget *titleBar_;
    QQuickItem *titleBarItem_;
    CustomizeQuickWidget *leftColumn_;
    QQuickItem *leftColumnItem_;

    QStackedWidget *pageControl_;
    FriendManager* friendManager_;
    ChatManager *chatManager_;

    QString currentAccount_;
    QSystemTrayIcon *systemIcon_;

    int currentIndex_=0;
signals:
    void newUserInfoDialog(QString account,DialogType type);
    void closeSig();
    void searchSig();
    void currentIndexChanged();
    void showSig();
};

#endif // MAINWINDOW_H
