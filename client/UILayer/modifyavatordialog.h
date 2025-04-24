#ifndef MODIFYAVATORDIALOG_H
#define MODIFYAVATORDIALOG_H

#include "framless_dialog.h"
#include "customizequickwidget.h"
#include <QQuickItem>
#include <QQuickItemGrabResult>

class ModifyAvatorDialog : public FramlessDialog
{
    Q_OBJECT
public:
    ModifyAvatorDialog(QWidget *parent=nullptr,const QSize requestSize=QSize(390,390),QString caption="");

    QImage resultImage_;
public slots:
    void updateUserImage(QQuickItemGrabResult *result);
private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
};

#endif // MODIFYAVATORDIALOG_H
