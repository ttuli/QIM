import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"#F0F8FF";

    property int verify_dia_index: 1;
    signal newDialog(string account);

    Image {
        id:icon;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        width: icon_text.implicitHeight;
        height: width;
        x:parent.width/2-(width+icon_text.anchors.leftMargin+icon_text.implicitWidth)/2;

        source: "/res/Users.svg";
    }
    Text {
        id:icon_text;
        anchors.left: icon.right;
        anchors.top: icon.top;
        anchors.leftMargin: 5;
        text: "关系列表";
        font.bold: true;
        font.pixelSize: 18;
    }

    Rectangle{
        id:verify_dialog_btn;
        anchors.top: icon_text.bottom;
        anchors.topMargin: 5;
        width: parent.width;
        height: 30;
        color:"transparent";

        property bool chosed: false;

        Text {
            anchors.centerIn: parent;
            text: "验证消息";
        }
        Rectangle{
            id:white_rec;
            anchors.fill: parent;
            visible: false;
            color:"#20000000";
        }
        Rectangle{
            id:black_rec;
            anchors.fill: parent;
            color:"#30000000"
            visible: false;
        }
        Rectangle {
            id:tip;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            width: 20;
            height: width;
            radius: width/2;
            color:"red";
            visible: globalData.msgnum_f_verify;
            Text {
                anchors.centerIn: parent;
                color:"white";
                text: globalData.msgnum_f_verify>99?"99+":globalData.msgnum_f_verify;
                font.pixelSize: 16;
                font.bold: true;
            }
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                if(!parent.chosed){
                    white_rec.visible=true;
                }
            }
            onExited: {
                if(!parent.chosed){
                    white_rec.visible=false;
                }
            }
            onClicked: {
                parent.chosed=true;
                white_rec.visible=false;
                black_rec.visible=true;
                manager.onIndexChanged(verify_dia_index);
            }
        }
    }

    ScrollView{
        id:view_area;
        anchors.top: verify_dialog_btn.bottom;
        width: parent.width;
        anchors.bottom: parent.bottom;
        contentHeight: friend_list.height+friend_btn.height;


        Rectangle{
            id:friend_btn;
            anchors.top: parent.top;
            width: parent.width;
            height: verify_dialog_btn.height;
            color:"transparent";
            Text {
                anchors.centerIn: parent;
                text:"我的好友";
            }
            Image {
                id: friend_arrow;
                source: "/res/ChevronDown.svg";
                anchors.right: parent.right;
                anchors.rightMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;
                height: parent.height-10;
                width:height;

                Behavior on rotation {
                    NumberAnimation{
                        duration: 80;
                    }
                }
            }
            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    friend_arrow.rotation+=180;
                    friend_list.toShow=!friend_list.toShow;
                }
            }
        }


        ListView{
            id:friend_list;
            anchors.top: friend_btn.bottom;
            width: parent.width;
            height: {
                if(toShow&&contentHeight)
                    return contentHeight;
                return 0;
            }

            clip: true;
            flickableDirection: Flickable.HorizontalFlick
            property bool toShow: false;

            model:friendListModel;

            delegate:Rectangle{
                width: view_area.width;
                height: 60;
                color:"transparent";

                Image{
                    id:avatar_source;
                    source:"image://imageProvider/"+account+"_avatar";
                    visible: false;
                    cache: false;
                    sourceSize.width: imageLength;
                    sourceSize.height: imageLength;
                }
                Rectangle{
                    id:avatar_outline;
                    width:imageLength;
                    height: imageLength;
                    radius: width/2;
                    visible: false;
                }
                OpacityMask{
                    id:avatar;
                    source: avatar_source;
                    maskSource: avatar_outline;
                    width: avatar_outline.width;
                    height: avatar_outline.height;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 5;
                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 5;
                        samples: 8;
                        color:"grey";
                    }
                }
                Text {
                    id: username;
                    text: name;
                    color:"black";
                    width: implicitWidth>parent.width-60?parent.width-60:implicitWidth;
                    elide: Text.ElideRight;
                    anchors.top: avatar.top;
                    anchors.left: avatar.right;
                    anchors.leftMargin: 5;
                    font.pixelSize: 16;
                }
                Image {
                    id: sexImage;
                    source: sex?"/res/userinfo_female.svg":"/res/userinfo_male.svg";
                    width: height;
                    height: username.implicitHeight;
                    anchors.top: username.bottom;
                    anchors.left: username.left;
                }
                Rectangle{
                    id:black_hover;
                    color:"#30000000";
                    anchors.fill: parent;
                    visible: false;
                }

                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onExited: {
                        black_hover.visible=false;
                    }
                    onEntered: {
                        black_hover.visible=true;
                    }
                    onClicked: {
                        manager.onNewDialog(account);
                    }
                }
            }
            Behavior on height {
                NumberAnimation {
                    duration: 100;
                }
            }
        }//friend_list
    }

    function setBlockDiaVisible(val)
    {
        blockDia.visible=val;
    }

    BlockingDialog{
        id:blockDia;
        anchors.fill: parent;
        visible: false;
        mtext: "...";
    }
}
