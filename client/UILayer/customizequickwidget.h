#ifndef CUSTOMIZEQUICKWIDGET_H
#define CUSTOMIZEQUICKWIDGET_H

#include <QQuickWidget>
#include "imageprovider.h"

class CustomizeQuickWidget : public QQuickWidget
{
    Q_OBJECT
public:
    CustomizeQuickWidget(QWidget *parent=nullptr);
    ~CustomizeQuickWidget();
private:
    ImageProvider *provider_;
};

#endif // CUSTOMIZEQUICKWIDGET_H
