#include "framless_dialog.h"
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#include <QScreen>
#include <QMouseEvent>
#include <QGuiApplication>

FramlessDialog::FramlessDialog(QWidget *parent)
    :QDialog(parent)
{}

bool FramlessDialog::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG"){

        MSG *msg = static_cast<MSG *>(message);

        if (msg->message == WM_NCCALCSIZE)
        {
            *result = 0;
            return true;
        } else if (msg->message == WM_SYSKEYDOWN){
            //屏蔽alt键按下
        } else if (msg->message == WM_SYSKEYUP){
            //屏蔽alt键松开
        } else if (msg->message == WM_NCHITTEST){
            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);
            QPoint cursorpos(x,y);

            //注意屏幕缩放的问题
            qreal scales=QGuiApplication::primaryScreen()->devicePixelRatio();
            cursorpos/=scales;

            QPoint pos=mapFromGlobal(cursorpos);
            int padding=5;

            bool left = pos.x() < padding;
            bool right = pos.x() > width() - padding;
            bool top = pos.y() < padding;
            bool bottom = pos.y() > height() - padding;

            *result = 0;

            if (left && top) {
                *result = HTTOPLEFT;
            } else if (left && bottom) {
                *result = HTBOTTOMLEFT;
            } else if (right && top) {
                *result = HTTOPRIGHT;
            } else if (right && bottom) {
                *result = HTBOTTOMRIGHT;
            } else if (left) {
                *result = HTLEFT;
            } else if (right) {
                *result = HTRIGHT;
            } else if (top) {
                *result = HTTOP;
            } else if (bottom) {
                *result = HTBOTTOM;
            }

            //先处理掉拉伸
            if (0 != *result) {
                return true;
            }

        } else if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST){
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC){
            //休眠唤醒后自动打开
            this->showNormal();
        }
    }
    return false;
}

void FramlessDialog::initInterface()
{
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    ::SetWindowLong(hwnd, GWL_STYLE, style|WS_MAXIMIZEBOX | WS_THICKFRAME|WS_CAPTION);
    MARGINS margins = {-1,-1,-1,-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void FramlessDialog::showEvent(QShowEvent *)
{
    initInterface();
}

void FramlessDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(leftButton_){
        move(windowp_+event->globalPosition().toPoint()-cursorp_);
    }
}

void FramlessDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        leftButton_=true;
        cursorp_=event->globalPosition().toPoint();
        windowp_=pos();
    }
}

void FramlessDialog::mouseReleaseEvent(QMouseEvent *event)
{
    leftButton_=false;
}
