#include "customizequickwidget.h"
#include "constants.h"
#include <QQmlEngine>
#include <QQmlContext>

CustomizeQuickWidget::CustomizeQuickWidget(QWidget *parent)
    :QQuickWidget(parent)
{
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setClearColor(Qt::transparent);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    provider_=new ImageProvider;
    engine()->addImageProvider("imageProvider",provider_);
    rootContext()->setContextProperty("maxwaittime",MAX_WAIT_TIME);
    rootContext()->setContextProperty("imageLength",ImageLength);
    rootContext()->setContextProperty("imageMaxLength",ImageMaxLength);
    rootContext()->setContextProperty("imageMaxLength_Chat",ImageMaxLength_Chat);
}

CustomizeQuickWidget::~CustomizeQuickWidget()
{
    engine()->removeImageProvider("imageProvider");
}

