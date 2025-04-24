import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    property string color_: "#F0F8FF";

    color:color_;
    ListView{
        id:listview;
        anchors.fill: parent;
        model:chatListModel;
        currentIndex:-1;

        delegate: Rectangle{
            id:outline;
            width: listview.width;
            height: 70;
            color: "transparent";

            Rectangle{
                anchors.fill:parent;
                color: "#33000000";
                visible: manager?manager.currentA===account:false;
            }
            Rectangle{
                id:hoverColorRec;
                anchors.fill:parent;
                color: "#20000000";
                visible: false;
            }

            Rectangle{
                width: parent.width;
                height: 1;
                anchors.bottom: parent.bottom;
                color: "#20000000"
            }

            Image {
                id: outline_ima;
                source: "/res/circle.png";
                visible: false;
            }
            OpacityMask{
                id:avator;
                maskSource: outline_ima;
                source: avator_ima;
                height: imageLength;
                width: imageLength;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color:"grey";
                }
            }
            Image {
                id: avator_ima;
                source: "image://imageProvider/"+account+"_avatar";
                visible: false;
                cache: false;
                sourceSize.width: avator.width;
                sourceSize.height: avator.height;
            }
            Rectangle{
                width: 20;
                height: 20;
                radius: width/2;
                color: "red";
                anchors.top: m_timestemp.bottom;
                anchors.bottomMargin: 5;
                anchors.right: parent.right;
                anchors.margins: 5;
                visible: unreadmsg;

                Text {
                    anchors.centerIn: parent;
                    text: unreadmsg>99?"99+":unreadmsg;
                    color:"white";
                    font.pixelSize: 13;
                    font.bold: true;
                }
            }

            Text {
                id: user_name;
                text:name;
                anchors.top: avator.top;
                anchors.topMargin: -5;
                anchors.left: avator.right;
                anchors.leftMargin: 5;
                anchors.right: parent.right;
                anchors.rightMargin: m_timestemp.implicitWidth+5;
                elide: Text.ElideRight;
                font.bold: true;
                font.pixelSize: 18;
            }

            Text {
                id: last_msg;
                anchors.bottom: avator.bottom;
                anchors.left: avator.right;
                anchors.right: parent.right;
                anchors.rightMargin: 80;
                anchors.leftMargin: 5;
                font.pixelSize: 18;
                textFormat: Text.RichText;
                elide: Text.ElideRight
                color:"#696969";
                clip: true;
                text: lastMsg
            }

            Text {
                id: elideText;
                text: qsTr("...");
                anchors.left: last_msg.right;
                anchors.leftMargin: 3;
                anchors.bottom: avator.bottom;
                visible: last_msg.implicitWidth>last_msg.width;
                font:last_msg.font;
            }

            Text {
                id: m_timestemp;
                text: timestemp;
                anchors.right: parent.right;
                anchors.rightMargin: 5;
                anchors.top:avator.top;
                color:"black";
                opacity: 0.5;
                font.pixelSize: 13;
            }

            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton|Qt.RightButton;
                onClicked: function(mouse){
                    if(mouse.button === Qt.LeftButton){
                        manager.setCurrentA(account);
                        manager.onTalk(account,name);
                        chatListModel.clearUnReadMsg(index);
                    } else if (mouse.button === Qt.RightButton) {
                        var contextMenu = contextMenuComponent.createObject(outline, {
                            "x": mouse.x,
                            "y": mouse.y,
                            "position":index
                        });
                        contextMenu.open();
                    }
                }
                onEntered: {
                    if(manager.currentA!==account)
                        hoverColorRec.visible=true;
                }
                onExited: {
                    hoverColorRec.visible=false;
                }
            }
        }
    }

    Rectangle{
        height: parent.height;
        width: 1;
        color:"#33000000";
        anchors.right: parent.right;
    }

    Component{
        id:contextMenuComponent;
        CustomMenu{
            id:contextMenu;
            property int position: -1;
            CustomMenuItem{
                mtext: "设置已读";
                itemRadius_top: contextMenu.backgroundRadius/2;
                onTriggered: {
                    chatListModel.clearUnReadMsg(position);
                }
            }
            CustomMenuItem{
                mtext: "移除选中项";
                itemRadius_bottom: contextMenu.backgroundRadius/2;
                onTriggered: {
                    chatListModel.removeInfo(position);
                }
            }
        }
    }
}
