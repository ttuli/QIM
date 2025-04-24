#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QMutex>

class CusImageRespond : public QQuickImageResponse
{
    Q_OBJECT
public:
    CusImageRespond(const QString& id, const QSize& requestedSize);
    ~CusImageRespond()=default;

    QQuickTextureFactory* textureFactory() const override;

private:
    QString id_;
    QImage image_;
};

class ImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    ImageProvider()=default;
    ~ImageProvider()=default;

    QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
};

#endif // IMAGEPROVIDER_H
