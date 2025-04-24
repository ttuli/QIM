#include "pictureview.h"
#include <QScreen>
#include <QVBoxLayout>
#include <QQmlEngine>
#include <QImage>
#include <QFileDialog>
#include <QDateTime>

PictureView::PictureView(QWidget *parent,QString md5)
    : WindowsBackground{parent},currentImageMd5_(md5)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->setSource(QUrl("qrc:/PictureViewer/PictureView.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(centerSig()),this,SLOT(centerWindow()));
    connect(interfaceItem_,SIGNAL(resizeSig(int,int)),this,SLOT(adjustWindow(int,int)));
    connect(interfaceItem_,SIGNAL(saveSig()),this,SLOT(onSave()));

    titleBar_=new CustomizeQuickWidget(this);
    titleBar_->setSource(QUrl("qrc:/PictureViewer/ViewTitle.qml"));
    titleBarItem_=titleBar_->rootObject();
    titleBar_->setFixedHeight(35);
    connect(titleBarItem_,SIGNAL(closeSig()),this,SLOT(onClose()));
    connect(titleBarItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(titleBarItem_,SIGNAL(maxSig()),this,SLOT(onMax()));

    qvb_=new QVBoxLayout;
    qvb_->setContentsMargins(0,0,0,0);
    qvb_->addWidget(titleBar_);
    qvb_->addWidget(interface_);
    qvb_->setSpacing(0);

    QMetaObject::invokeMethod(interface_->rootObject(),"setImageSource",Q_ARG(QVariant,md5));

    resize(800,600);
    setTitleWidget(titleBar_);
    mainHLayout_=new QHBoxLayout(this);
    mainHLayout_->setContentsMargins(0,0,0,0);
    mainHLayout_->addLayout(qvb_);
}

PictureView::~PictureView()
{
}

void PictureView::onClose()
{
    emit doDestory(currentImageMd5_);
}

void PictureView::onMax()
{
    isMaximized()?showNormal():showMaximized();
}

void PictureView::adjustWindow(int newWidth, int newHeight)
{
    resize(newWidth,newHeight);
}

void PictureView::centerWindow()
{
    screenWidth_=QGuiApplication::primaryScreen()->size().width();
    screenHeight_=QGuiApplication::primaryScreen()->size().height();
    move(screenWidth_/2-width()/2,screenHeight_/2-height()/2);
}

void PictureView::onSave()
{
    QString timestemp=QDateTime::currentDateTime().toString("yyMMddHHms");
    QString position=QFileDialog::getSaveFileName(this,"选择存储位置",QDir::currentPath() + "/"+timestemp+".png");
    if(!position.isEmpty())
        emit doSave(currentImageMd5_,position);
}

