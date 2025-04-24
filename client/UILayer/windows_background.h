#ifndef WINDOWS_BACKGROUND_H
#define WINDOWS_BACKGROUND_H

#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>
#include <QHBoxLayout>
#include <QVBoxLayout>

class WindowsBackground : public QWidget
{
    Q_OBJECT
public:
    explicit WindowsBackground(QWidget *parent = nullptr);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    void initInterface();

    void setTitleWidget(QQuickWidget *widget);

    void showEvent(QShowEvent*);

    QHBoxLayout *mainHLayout_=nullptr;
private:
    int padding=5;

    QQuickWidget *title;
signals:
};

#endif // WINDOWS_BACKGROUND_H
