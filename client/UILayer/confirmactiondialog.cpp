#include "confirmactiondialog.h"
#include <QHBoxLayout>
#include <QQmlContext>

ConfirmActionDialog::ConfirmActionDialog(QWidget *parent,QString content,bool showAccept,bool showReject)
    :FramlessDialog(parent)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("dialog",this);
    interface_->setSource(QUrl("qrc:/ConfirmActionDialog.qml"));
    interfaceItem_=interface_->rootObject();

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(interface_);

    QMetaObject::invokeMethod(interfaceItem_,"init",Q_ARG(QVariant,content)
                              ,Q_ARG(QVariant,showAccept)
                              ,Q_ARG(QVariant,showReject));
    setMaximumSize(250,170);
    setMinimumSize(250,170);
}
