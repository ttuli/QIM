#include "unresizewindowsbackgruond.h"
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#include <QScreen>
#include <QMouseEvent>

UnresizeWindowsBackgruond::UnresizeWindowsBackgruond(QWidget *parent)
    : QWidget{parent}
{
}

bool UnresizeWindowsBackgruond::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
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
                    NCCALCSIZE_PARAMS* pnc = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                    //SM_CXSIZEFRAME	 水平方向的系统拉伸边框厚度（逻辑像素）
                    //SM_CXPADDEDBORDER	 DWM 添加的额外边框宽度（逻辑像素）
                    const int border = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                    pnc->rgrc[0].left   += border;
                    pnc->rgrc[0].top    += border;
                    pnc->rgrc[0].right  -= border;
                    pnc->rgrc[0].bottom -= border;
                }
                *result = 0;
                return true;
            }
        } else if (msg->message == WM_SYSKEYDOWN){
            //屏蔽alt键按下
        } else if (msg->message == WM_SYSKEYUP){
            //屏蔽alt键松开
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

void UnresizeWindowsBackgruond::initInterface()
{
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    ::SetWindowLong(hwnd, GWL_STYLE, style|WS_MAXIMIZEBOX | WS_THICKFRAME|WS_CAPTION);
    MARGINS margins = {-1,-1,-1,-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void UnresizeWindowsBackgruond::showEvent(QShowEvent *)
{
    initInterface();
}

void UnresizeWindowsBackgruond::mouseMoveEvent(QMouseEvent *event)
{
    if(leftButton_){
        move(windowp_+event->globalPosition().toPoint()-cursorp_);
    }
}

void UnresizeWindowsBackgruond::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        leftButton_=true;
        cursorp_=event->globalPosition().toPoint();
        windowp_=pos();
    }
}

void UnresizeWindowsBackgruond::mouseReleaseEvent(QMouseEvent *event)
{
    leftButton_=false;
}
