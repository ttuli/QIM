#ifndef PICTUREVIEW_H
#define PICTUREVIEW_H

#include <QWidget>
#include <QQuickItem>
#include <QVBoxLayout>
#include "UILayer/customizequickwidget.h"
#include "UILayer/windows_background.h"

class PictureView : public WindowsBackground
{
    Q_OBJECT
public:
    explicit PictureView(QWidget *parent = nullptr,QString md5="");
    ~PictureView();

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

    CustomizeQuickWidget *titleBar_;
    QQuickItem *titleBarItem_;

    QVBoxLayout *qvb_;

    QString currentImageMd5_;
    int screenWidth_;
    int screenHeight_;
private slots:
    void onClose();
    void onMax();
    void adjustWindow(int newWidth,int newHeight);
    void centerWindow();
    void onSave();
signals:
    void doDestory(QString md5);
    void doSave(QString md5,QString savepath);
};

#endif // PICTUREVIEW_H
