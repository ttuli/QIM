import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"#F5F5F5";
    Rectangle{
        id:closebtn;
        width: 25;
        height: 25;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";

        Image {
            anchors.fill: parent;
            source: "/res/XMark.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="red";
            }
            onExited: {
                parent.color="transparent";
            }
            onPressed: {
                parent.color="#B22222";
            }
            onReleased: {
                parent.color="red";
            }
            onClicked: {
                dialog.reject();
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: 80
            }
        }
    }

    function init(content,showAccept,showReject)
    {
        contentText.text=content;
        confirmBtn.visible=showAccept;
        rejectBtn.visible=showReject;
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter;
        color:"#111827";
        text: "提示";
        font.bold: true;
        font.pixelSize: 17;
    }

    Text {
        id:contentText;
        text: "";
        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 70;
        font.pixelSize: 16;
        color:"black";
    }

    property int btnWidth: 80;
    property int btnHeight: 35;
    Row{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        spacing: 65;
        Rectangle{
            id:confirmBtn;
            width: btnWidth;
            height: btnHeight;
            color:"#3B82F6";
            radius: 5;
            Text {
                anchors.centerIn: parent;
                text:"确认";
                font.pixelSize: 13;
                color:"white";
            }
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 5;
                samples: 10;
                color:"grey";
            }
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: {
                    confirmBtn.color="#2563EB";
                }
                onExited: {
                    confirmBtn.color="#3B82F6";
                }
                onClicked: {
                    dialog.accept();
                }
            }
        }
        Rectangle{
            id:rejectBtn;
            width: btnWidth;
            height: btnHeight;
            radius: 5;
            color:"#9CA3AF";
            Text {
                anchors.centerIn: parent;
                text: "取消";
                font.pixelSize: 13;
                color:"#111827";
            }
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 5;
                samples: 10;
                color:"grey";
            }
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: {
                    rejectBtn.color="#6B7280";
                }
                onExited: {
                    rejectBtn.color="#9CA3AF";
                }
                onClicked: {
                    dialog.reject();
                }
            }
        }
    }
}
