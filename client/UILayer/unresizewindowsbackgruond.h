#ifndef UNRESIZEWINDOWSBACKGRUOND_H
#define UNRESIZEWINDOWSBACKGRUOND_H

#include <QWidget>

class UnresizeWindowsBackgruond : public QWidget
{
    Q_OBJECT
public:
    explicit UnresizeWindowsBackgruond(QWidget *parent = nullptr);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    void initInterface();

    void showEvent(QShowEvent*);

    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint windowp_;
    QPoint cursorp_;
    bool leftButton_=false;

signals:
};

#endif // UNRESIZEWINDOWSBACKGRUOND_H
