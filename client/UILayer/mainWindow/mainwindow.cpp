#include "mainwindow.h"
#include "UILayer/qmlsharedata.h"
#include <QQmlContext>
#include <QVBoxLayout>
#include <QMenu>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent,FriendManager* friendManager,ChatManager *chatManager)
    :WindowsBackground(parent),friendManager_(friendManager),chatManager_(chatManager)
{
    titleBar_=new CustomizeQuickWidget(this);
    titleBar_->setSource(QUrl("qrc:/mainWindow/TitleBar.qml"));
    titleBar_->setFixedHeight(30);
    titleBarItem_=titleBar_->rootObject();
    connect(titleBarItem_,SIGNAL(closeSig()),this,SLOT(hide()));
    connect(titleBarItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(titleBarItem_,SIGNAL(maxSig()),this,SLOT(onMax()));

    leftColumn_=new CustomizeQuickWidget(this);
    leftColumn_->rootContext()->setContextProperty("globalData",QmlShareData::getInstance());
    leftColumn_->rootContext()->setContextProperty("mainWindow",this);
    leftColumn_->setSource(QUrl("qrc:/mainWindow/LeftChoiceColumn.qml"));
    leftColumn_->setFixedWidth(60);
    leftColumnItem_=leftColumn_->rootObject();
    connect(leftColumnItem_,SIGNAL(checkInfoSig()),this,SLOT(onCheckMyInfo()));
    connect(leftColumnItem_,SIGNAL(addFriendSig()),this,SLOT(onSearch()));

    mainHLayout_=new QHBoxLayout(this);
    mainHLayout_->setContentsMargins(0,0,0,0);
    mainHLayout_->setSpacing(0);
    mainHLayout_->addWidget(leftColumn_);

    pageControl_=new QStackedWidget(this);
    pageControl_->setStyleSheet("background-color:#f9f7f7;");
    pageControl_->addWidget(chatManager_);
    pageControl_->addWidget(friendManager_);
    pageControl_->setCurrentWidget(chatManager_);

    QVBoxLayout *qvb=new QVBoxLayout;
    qvb->setContentsMargins(0,0,0,0);
    qvb->setSpacing(0);
    qvb->addWidget(titleBar_);
    qvb->addWidget(pageControl_);
    mainHLayout_->addLayout(qvb);

    setMinimumSize(minMainWindowWidth,minMainWindowHeight);
    setTitleWidget(titleBar_);
    resize(width(),height()+100);
#if DOTEST
    setSystemTray();
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::updateUserImage()
{
    QMetaObject::invokeMethod(leftColumnItem_,"setImage",Q_ARG(QVariant,currentAccount_));
}

void MainWindow::showEvent(QShowEvent *)
{
    initInterface();
    emit showSig();
}

void MainWindow::init(QString account)
{
    currentAccount_=account;
    QMetaObject::invokeMethod(leftColumnItem_,"setImage",Q_ARG(QVariant,account));
    QTimer::singleShot(2000,[this,account]{
        show();
        setSystemTray();
    });
}

void MainWindow::setCurrentIndex(int index)
{
    pageControl_->setCurrentIndex(index);
    IM_Current_Window = static_cast<currentWindow>(index);
    currentIndex_=index;
    emit currentIndexChanged();
}

void MainWindow::onCheckMyInfo()
{
    emit newUserInfoDialog(currentAccount_,DialogType::MYSELF);
}

void MainWindow::onMax()
{
    isMaximized()?showNormal():showMaximized();
}

void MainWindow::onSearch()
{
    emit searchSig();
}

void MainWindow::setSystemTray()
{
    systemIcon_=new QSystemTrayIcon(QIcon(":/res/icon.png"),this);
    systemIcon_->setToolTip("IM");
    systemIcon_->setVisible(true);
    connect(systemIcon_,&QSystemTrayIcon::activated,[=](QSystemTrayIcon::ActivationReason reason){
        if(reason==QSystemTrayIcon::Trigger){
            activateWindow();
            if(isMinimized()){
                showNormal();
                QRect screenRect=QGuiApplication::primaryScreen()->geometry();
                move(screenRect.width()/2-width()/2,screenRect.height()/2-height()/2);
            } else if(!isVisible()){
                showNormal();
                QRect screenRect=QGuiApplication::primaryScreen()->geometry();
                move(screenRect.width()/2-width()/2,screenRect.height()/2-height()/2);
            }
        }
    });

    QMenu *menu=new QMenu(this);

    QAction *loginoutBtn=new QAction("注销",this);
    //connect(loginoutBtn,&QAction::triggered,this,&ChatState::restartProgram);
    menu->addAction(loginoutBtn);

    QAction *quitBtn=new QAction("退出",this);
    connect(quitBtn,&QAction::triggered,[this]{
        emit closeSig();
    });
    menu->addAction(quitBtn);


    systemIcon_->setContextMenu(menu);
}
