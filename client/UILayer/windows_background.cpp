#include "windows_background.h"
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#include <QScreen>
#include <QMouseEvent>
#include <QGuiApplication>

WindowsBackground::WindowsBackground(QWidget *parent)
    : QWidget{parent}
{
    title=nullptr;
    mainHLayout_=nullptr;
}

void WindowsBackground::initInterface()
{
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    ::SetWindowLong(hwnd, GWL_STYLE, style|WS_MAXIMIZEBOX | WS_THICKFRAME|WS_CAPTION);
    MARGINS margins = {-1,-1,-1,-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void WindowsBackground::setTitleWidget(QQuickWidget *widget)
{
    title=widget;
}

void WindowsBackground::showEvent(QShowEvent *)
{
    initInterface();
}


bool WindowsBackground::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG *>(message);

    if (eventType == "windows_generic_MSG"){
        if (msg->message == WM_NCCALCSIZE)
        {
            //表示窗口正在被系统重新计算位置和大小（比如最大化）
            if (msg->wParam == TRUE) {
                WINDOWPLACEMENT wp;//用于获取窗口当前的状态
                wp.length = sizeof(WINDOWPLACEMENT);//必须先设置 length 字段，否则 GetWindowPlacement 不会正确填充结构体
                GetWindowPlacement(msg->hwnd, &wp);

                //检查窗口当前是否处于最大化状态
                if (wp.showCmd == SW_MAXIMIZE) {
                    // NCCALCSIZE_PARAMS* pnc = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                    // //SM_CXSIZEFRAME	 水平方向的系统拉伸边框厚度（逻辑像素）
                    // //SM_CXPADDEDBORDER	 DWM 添加的额外边框宽度（逻辑像素）
                    // int border=GetSystemMetrics(SM_CXSIZEFRAME);
                    // pnc->rgrc[0].left  += border;
                    // pnc->rgrc[0].top  += border;
                    // pnc->rgrc[0].right  -= border;
                    // pnc->rgrc[0].bottom -= border;
                    if(mainHLayout_)
                        mainHLayout_->setContentsMargins(6,6,6,6);
                } else{
                    if(mainHLayout_)
                        mainHLayout_->setContentsMargins(0,0,0,0);
                }
                *result = WVR_ALIGNTOP|WVR_ALIGNLEFT;
                return true;
            }
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
            //系统给定窗口拉伸的距离，不同缩放下有不同的值
            int padding = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            padding/=2;

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

            if(title&&title->geometry().contains(pos)){
                QPoint quickWidgetPos = title->mapFromParent(pos);
                if (!title->rootObject()->childAt(quickWidgetPos.x(), quickWidgetPos.y())) {
                    *result = HTCAPTION;
                    return true;
                }
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
