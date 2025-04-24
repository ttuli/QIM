#include "UILayer/uimanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/res/icon.png"));
    a.setQuitOnLastWindowClosed(false);
    a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    UIManager *manager=new UIManager;
    manager->start();

    int result=a.exec();
    delete manager;
    return result;
}
