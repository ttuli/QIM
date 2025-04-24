#ifndef FRIENDREQUESTDIALOG_H
#define FRIENDREQUESTDIALOG_H

#include "UILayer/framless_dialog.h"
#include "UILayer/customizequickwidget.h"
#include <QQuickItem>

class FriendRequestDialog : public FramlessDialog
{
    Q_OBJECT
public:
    FriendRequestDialog(QWidget *parent=nullptr,QString account="",QString name="");

    QString content_;

private:
    QString account_;
    QString name_;

    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

private slots:
    void onApply(QString content);
signals:

};

#endif // FRIENDREQUESTDIALOG_H
