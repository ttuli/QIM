#include "pictureviewmanager.h"

PictureViewManager::PictureViewManager(QObject *parent)
    : QObject{parent}
{
}

PictureViewManager::~PictureViewManager()
{
    for(auto i:viewManager_){
        delete i;
    }
}

void PictureViewManager::createView(QString md5)
{
    if(viewManager_.contains(md5)){
        if(viewManager_[md5]->isVisible())
            viewManager_[md5]->activateWindow();
        return;
    }
    PictureView *view=new PictureView(nullptr,md5);
    connect(view,&PictureView::doDestory,[this](QString md5){
        if(viewManager_.contains(md5)){
            viewManager_[md5]->close();
            viewManager_[md5]->deleteLater();
            viewManager_.remove(md5);
        }
    });
    connect(view,&PictureView::doSave,this,[this](QString id,QString path){
        emit saveImageSig(id,path);
    });
    viewManager_[md5]=view;
    view->show();
}
