#ifndef PICTUREVIEWMANAGER_H
#define PICTUREVIEWMANAGER_H

#include <QObject>
#include <QMap>
#include "pictureview.h"

class PictureViewManager : public QObject
{
    Q_OBJECT
public:
    PictureViewManager(QObject *parent = nullptr);
    ~PictureViewManager();

    void createView(QString md5);

private:
    QMap<QString,PictureView*> viewManager_;
signals:
    void saveImageSig(QString id,QString savepath);
};

#endif // PICTUREVIEWMANAGER_H
