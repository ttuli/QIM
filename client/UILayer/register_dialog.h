#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include "unresizewindowsbackgruond.h"
#include "customizequickwidget.h"
#include <QVBoxLayout>
#include "constants.h"

class RegisterDialog : public UnresizeWindowsBackgruond
{
    Q_OBJECT
public:
    RegisterDialog(QWidget *parent=nullptr);

    ~RegisterDialog();

    void showInfo(QString content,captionType type,int duration);

protected:

private:
    CustomizeQuickWidget *registerInterface_;
    QQuickItem *registerInterfaceItem_;

private slots:
    void doRegister(QString username,QString pwd,QString email);
    void onClose();

signals:
    void finishSig(QString account);
    void registerSig(QString name,QString password,QString email);
};

#endif // REGISTER_DIALOG_H
