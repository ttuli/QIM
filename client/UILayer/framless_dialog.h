#ifndef FRAMLESS_DIALOG_H
#define FRAMLESS_DIALOG_H

#include <QDialog>

class FramlessDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FramlessDialog(QWidget *parent=nullptr);

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

#endif // FRAMLESS_DIALOG_H
