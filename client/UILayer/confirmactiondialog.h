#ifndef CONFIRMACTIONDIALOG_H
#define CONFIRMACTIONDIALOG_H

#include "framless_dialog.h"
#include "customizequickwidget.h"
#include <QQuickItem>

class ConfirmActionDialog : public FramlessDialog
{
    Q_OBJECT
public:
    ConfirmActionDialog(QWidget *parent=nullptr,QString content="",bool showAccept=true,bool showReject=false);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
};

#endif // CONFIRMACTIONDIALOG_H
