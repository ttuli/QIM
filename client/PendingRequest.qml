import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"white";
    objectName: "root";

    function showInfo(msg,type,duration)
    {
        msgPop.showMessage(msg,type,duration);
        blockdia.visible=false;
    }

    ListView{
        id:verify_list;
        anchors.centerIn: parent;
        width: parent.width-50;
        height: parent.height-50;
        spacing:20;

        model:friendVerifyModel;

        delegate: Rectangle{
            width: parent.width;
            height: 70;
            radius: 8;

            layer.enabled: true;
            layer.effect: DropShadow{
                radius:4;
                samples: 8;
                color:"grey";
            }

            Rectangle{
                id:pulseRemind;
                anchors.fill: parent;
                color:"transparent";
                radius: parent.radius;
                SequentialAnimation{
                    id: pulseAnimation
                    running: false;

                    ColorAnimation {
                        target: pulseRemind
                        property: "color"
                        from: "transparent"
                        to: "#F9A825"
                        duration: 700
                        easing.type: Easing.InOutQuad
                    }

                    ColorAnimation {
                        target: pulseRemind
                        property: "color"
                        from: "#F9A825"
                        to: "transparent"
                        duration: 700
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            Image {
                id: avator_source;
                source: "image://imageProvider/"+account+"_avatar";
                visible: false;
                sourceSize.width: avator.width;
                sourceSize.height: avator.height;
            }
            Rectangle{
                id:avator_outline;
                width: avator.width;
                height: avator.height;
                radius: width/2;
                visible: false;
            }
            OpacityMask{
                id:avator;
                height: imageLength;
                width: imageLength;
                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.leftMargin: 5;
                source: avator_source;
                maskSource: avator_outline;
            }

            Text {
                id: user_name;
                text: name;
                anchors.top: avator.top;
                anchors.left: avator.right;
                anchors.right: accept_btn.left;
                elide: Text.ElideRight;
                anchors.leftMargin: 5;
                font.bold: true;
                font.pixelSize: 17;
                color:"black";

                ToolTip{
                    id:name_tip;
                }

                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: {
                        if(parent.width<parent.implicitWidth)
                            name_tip.show(parent.text,3000);
                    }
                    onExited: {
                        name_tip.close();
                    }
                }
            }
            Text {
                id: user_account;
                text:account;
                anchors.top: user_name.bottom;
                anchors.left: user_name.left;
                anchors.leftMargin: 5;
                font.pixelSize: 12;
                opacity: 0.6;
            }
            TextArea{
                id:pureText;
                text:umessage;
                textFormat: "RichText";
                visible: false;
            }

            Text {
                id: message;
                anchors.bottom: parent.bottom;
                anchors.left: user_name.left;
                anchors.right: accept_btn.left;
                anchors.bottomMargin: 5;
                elide: Text.ElideRight;
                text: "留言:"+pureText.getText(0,pureText.length);

                ToolTip{
                    id:message_tip;
                }
                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: {
                        if(message.width<message.implicitWidth)
                        message_tip.show(message.text,5000);
                    }
                    onExited: {
                        message_tip.close();
                    }
                }
            }
            Text {
                id: state_sent;
                text: "已发送申请";
                font.bold: true;
                font.pixelSize: 17;
                color:"black";
                opacity: 0.5;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                visible: st===4;
            }
            Text {
                id: state_reject;
                text: "已拒绝";
                font.bold: true;
                font.pixelSize: 17;
                color:"black";
                opacity: 0.5;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                visible: st===3;
            }
            Text {
                id: state_delete;
                text: "已删除";
                font.bold: true;
                font.pixelSize: 17;
                color:"red";
                opacity: 0.5;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                visible: st===6;
            }
            Text {
                id: state_accept;
                text: "已同意";
                font.bold: true;
                font.pixelSize: 17;
                color:"lightgreen";
                opacity: 0.5;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                visible: st===2;
            }
            Text {
                id: state_ignore;
                text: "已忽略";
                font.bold: true;
                font.pixelSize: 17;
                color:"orange";
                opacity: 0.5;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                visible: st===5;
            }
            Button{
                id:accept_btn;
                visible: st===1;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: reject_btn.left;
                anchors.rightMargin: 10;
                text:"同意";
                contentItem:Text{
                    text: parent.text
                    font.pixelSize: 14;
                    color:"black";
                }
                background:Rectangle{
                    color:"white";
                    radius: 5;
                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 5;
                        samples: 8;
                        color:"grey";
                    }
                    Rectangle{
                        anchors.fill: parent;
                        color:"#30000000";
                        radius: parent.radius;
                        visible: accept_btn.pressed;
                    }
                }
                onClicked: {
                    doReact(account,2);
                }
            }
            Button{
                id:reject_btn;
                visible: st===1;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: ignore_btn.left;
                anchors.rightMargin: 10;
                text:"拒绝";
                contentItem:Text{
                    text: parent.text
                    font.pixelSize: 14;
                    color:"black";
                }
                background:Rectangle{
                    color:"white";
                    radius: 5;
                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 5;
                        samples: 8;
                        color:"grey";
                    }
                    Rectangle{
                        anchors.fill: parent;
                        color:"#30000000";
                        radius: parent.radius;
                        visible: reject_btn.pressed;
                    }
                }
                onClicked: {
                    doReact(account,3);
                }
            }
            Button{
                id:ignore_btn;
                visible: st===1;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                text:"忽略";
                contentItem:Text{
                    text: parent.text
                    font.pixelSize: 14;
                    color:"black";
                }
                background:Rectangle{
                    color:"white";
                    radius: 5;
                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 5;
                        samples: 8;
                        color:"grey";
                    }
                    Rectangle{
                        anchors.fill: parent;
                        color:"#30000000";
                        radius: parent.radius;
                        visible: ignore_btn.pressed;
                    }
                }
                onClicked: {
                    doReact(account,5);
                }
            }

            Connections {
                target: friendVerifyModel
                function onDataChanged(topLeft, bottomRight, roles) {
                    if (index >= topLeft.row && index <= bottomRight.row) {
                        if (roles.indexOf(4) !== -1) {
                            pulseAnimation.start();
                        }
                    }
                }
            }
        }
    }

    function doReact(tar,type){
        blockdia.visible=true;
        fmanager.onReact(tar,type);
    }

    BlockingDialog{
        id:blockdia;
        anchors.fill: parent;
        mtext:"正在操作...";
        visible:false;
    }
    MessagePopup{
        id:msgPop;
        anchors.fill: parent;
    }

}
