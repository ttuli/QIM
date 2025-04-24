#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "UILayer/windows_background.h"
#include "UILayer/customizequickwidget.h"
#include "UILayer/Model/searchmodel.h"
#include <QQuickItem>
#include "constants.h"

class SearchDialog : public WindowsBackground
{
    Q_OBJECT

public:
    enum {
        BY_ACCOUNT=0,
        BY_NAME=1
    };
    SearchDialog(QWidget *parent=nullptr,SearchModel_F *model=nullptr);
    void showInformation(QString msg,captionType type,int duration);
    void setIsGetAll(bool val);

    static constexpr const int searchSingleTime=30;

protected:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void onSearch(QString type,int mode,QString content,int offs);
    void onAdd(QString target,QString name,int index);
    void onUserInfoSig(QString account,int index);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

    SearchModel_F *model_;
    int offset_=0;
    bool isGetAll=false;

    QPoint windowp_;
    QPoint cursorp_;
    bool leftButton_=false;
signals:
    void doSearch(QString type,int mode,QString content,int offs);
    void doAdd(QString target,QString content,int index);
    void newUserInfoDia(QString account,int index);
};

#endif // SEARCHDIALOG_H
