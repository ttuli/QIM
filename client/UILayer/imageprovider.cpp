#include "imageprovider.h"
#include "DataLayer/datamanager.h"
#include <QThread>

QQuickImageResponse* ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    CusImageRespond *respond=new CusImageRespond(id,requestedSize);
    return respond;
}


CusImageRespond::CusImageRespond(const QString &id, const QSize &requestedSize)
    :id_(id)
{
    QImage ima=DataManager::getInstance()->getImageCache(id);
    if(!ima.isNull()){
        image_=ima;
        emit finished();
        return;
    }
    DataManager::getInstance()->getImageWithCallback(id,
         [this,id](QImage image){
             QMetaObject::invokeMethod(this,[this,id,image]{
                DataManager::getInstance()->addImageCache(id,image);
                 image_=image;
                 emit finished();
             });
         },
        RequestCallbcak({
            [this,id](PullRespond respond){
              DataManager::getInstance()->
                  addImageCache(id,QByteArray::fromStdString(respond.image_respond().data()));
                 QMetaObject::invokeMethod(this,[this,id]{
                     image_=DataManager::getInstance()->getImageCache(id);
                     emit finished();
                 });
         },[this,id]{
          DataManager::getInstance()->
              addImageCache(id,QByteArray());
              QMetaObject::invokeMethod(this,[this,id]{
                  image_=DataManager::getInstance()->getImageCache(id);
                  emit finished();
              });
        }
    }));
}

QQuickTextureFactory *CusImageRespond::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(QImage(image_));
}
