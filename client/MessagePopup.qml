import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: messagePopup
    // 默认宽度和高度，你可以根据需要调整
    width: parent ? parent.width : 400
    height: 50
    visible: popupAnimation.running;

    // 调用此函数显示消息
    function showMessage(msg,type,duration) {
        errorPopup_text.text = msg
        errorPopup_ima.infotype = type;
        pauseanimation.duration=duration;

        if(popupAnimation.running)
            popupAnimation.stop();
        popupAnimation.start();
    }

    // 使用一个圆角背景的矩形实现简洁风格
    Rectangle{
        id:errorPopup;
        color:"white";
        width: errorPopup_ima.width+errorPopup_text.implicitWidth+20;
        height: 40;
        radius: 10;
        y:-height;
        opacity: 0;

        anchors.horizontalCenter: parent.horizontalCenter;

        Image {
            id:errorPopup_ima;
            property int infotype: 0;
            source: {
                if(infotype===0){
                    //loading
                    return "/res/loading.png";
                } else if (infotype===1) {
                    //success
                    return "/res/success.svg";
                } else if(infotype===2) {
                    //fail
                    return "/res/wrong.svg";
                }
            }

            width: 25;
            height: width;

            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
            rotation:(infotype!==0?0:0);

            RotationAnimation on rotation {
                from: 0;
                to: 360;
                duration: 1000;
                loops: Animation.Infinite;
                running: errorPopup_ima.infotype===0;
            }
        }

        Text {
            id:errorPopup_text;
            text: "";
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: errorPopup_ima.right;
            anchors.leftMargin: 5;
        }

        SequentialAnimation {
            id: popupAnimation
            running: false  // 默认不运行

            // 从小变大
            ParallelAnimation {
                PropertyAnimation {
                    target: errorPopup
                    property: "y"
                    to: 10  // 弹出后的位置
                    duration: 500
                    easing.type: Easing.OutBack  // 缓动效果
                }
                PropertyAnimation {
                    target: errorPopup
                    property: "opacity"
                    to: 1  // 完全显示
                    duration: 500
                }
                ScaleAnimator {
                    target: errorPopup
                    from: 0.5  // 初始缩放
                    to: 1  // 最终缩放
                    duration: 500
                    easing.type: Easing.OutBack  // 缓动效果
                }
            }

            // 停留 2 秒
            PauseAnimation {
                id:pauseanimation;
                duration: 3000
            }

            // 从大变小
            ParallelAnimation {
                PropertyAnimation {
                    target: errorPopup
                    property: "y"
                    to: -errorPopup.height  // 回到界面外
                    duration: 500
                    easing.type: Easing.InBack  // 缓动效果
                }
                PropertyAnimation {
                    target: errorPopup
                    property: "opacity"
                    to: 0  // 完全消失
                    duration: 500
                }
                ScaleAnimator {
                    target: errorPopup
                    from: 1  // 初始缩放
                    to: 0.5  // 最终缩放
                    duration: 500
                    easing.type: Easing.InBack  // 缓动效果
                }
            }
        }
    }
}
