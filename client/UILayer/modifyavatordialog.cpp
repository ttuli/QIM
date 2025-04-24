#include "modifyavatordialog.h"
#include <QVBoxLayout>
#include <QQmlContext>

ModifyAvatorDialog::ModifyAvatorDialog(QWidget *parent,const QSize requestSize,QString caption)
    :FramlessDialog(parent)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("rimageWidth",requestSize.width());
    interface_->rootContext()->setContextProperty("rimageHeight",requestSize.height());
    interface_->rootContext()->setContextProperty("captionText",caption);
    interface_->rootContext()->setContextProperty("modifydia",this);
    interface_->setSource(QUrl("qrc:/ModifyAvatorDialog.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(reject()));

    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);

    setMinimumSize(requestSize.width(),requestSize.height()+200);
    setMaximumSize(requestSize.width(),requestSize.height()+200);
}

void ModifyAvatorDialog::updateUserImage(QQuickItemGrabResult *result)
{
    if(result==nullptr){
        qDebug()<<"null";
        return;
    }
    resultImage_=result->image();
    accept();
}

