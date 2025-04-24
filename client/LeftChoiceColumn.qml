import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"#3f72af";

    function setImage(acc)
    {
        user_ima.source="";
        user_ima.source="image://imageProvider/"+acc+"_avatar"
    }

    function setTip(value)
    {
        tip.visible=value;
    }

    function setTotalMsgNum(value)
    {
        totalMsgNum=value;
    }

    Rectangle{
        id:caption;
        color:"transparent";
        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width;
        height: caption_text.implicitHeight;

        Image {
            id:caption_image;
            source: "/res/IM_ICON.svg";
            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.top: parent.top;
            anchors.topMargin: 5;
            height: parent.height;
            width: height;
        }

        Text {
            id:caption_text;
            anchors.left: caption_image.right;
            anchors.leftMargin: 3;
            anchors.top: parent.top;
            anchors.topMargin: 5;
            text: qsTr("IM");
            verticalAlignment: Text.AlignVCenter;
            font.bold: true;
            font.pixelSize: 17;
        }
    }

    property int cubeSizdeWidth:width-15;
    property int imaMargin: 7;

    property int hoverdIndex:-1;

    property int totalMsgNum: globalData.msgnum_chat;
    property int totalMsgNum_F: globalData.msgnum_f_verify;

    Column{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: caption.height+20;
        spacing: 20;


        Rectangle{
            property int index: 0;
            color:{
                if(!mainWindow)
                    return "transparent";
                if(mainWindow.currentIndex===index)
                    return "#90ffffff";
                else if(hoverdIndex===index)
                    return "#50ffffff";
                else
                    return "transparent";
            }

            radius: 8;
            width: cubeSizdeWidth;
            height: cubeSizdeWidth;

            Image {
                source: "/res/message.svg";
                anchors.fill: parent;
                anchors.margins: imaMargin;
            }

            Rectangle {
                id: totalMsg;
                color:"red";
                radius: width/2;
                width: 20;
                height: 20;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.margins: -5;
                visible: totalMsgNum;
                Text {
                    anchors.centerIn: parent;
                    color:"white";
                    text: totalMsgNum>99?"99+":totalMsgNum;
                    font.pixelSize: 14;
                    font.bold: true;
                }
            }

            MouseArea{
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                hoverEnabled: true;
                onEntered: {
                    hoverdIndex=parent.index;
                }
                onExited: {
                    hoverdIndex=-1;
                }
                onClicked: {
                    mainWindow.setCurrentIndex(parent.index);
                }
            }
        }

        Rectangle{
            property int index: 1;
            color:{
                if(!mainWindow)
                    return "transparent";
                if(mainWindow.currentIndex===index)
                    return "#90ffffff";
                else if(hoverdIndex===index)
                    return "#50ffffff";
                else
                    return "transparent";
            }
            radius: 8;
            width: cubeSizdeWidth;
            height: cubeSizdeWidth;


            Image {
                source: "/res/friend.svg";
                anchors.fill: parent;
                anchors.margins: imaMargin;
            }

            Rectangle {
                id: tip;
                color:"red";
                radius: width/2;
                width: 20;
                height: 20;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.margins: -5;
                visible: totalMsgNum_F;
                Text {
                    anchors.centerIn: parent;
                    color:"white";
                    text: totalMsgNum_F>99?"99+":totalMsgNum_F;
                    font.pixelSize: 14;
                    font.bold: true;
                }
            }

            MouseArea{
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                hoverEnabled: true;
                onEntered: {
                    hoverdIndex=parent.index;
                }
                onExited: {
                    hoverdIndex=-1;
                }
                onClicked: {
                    mainWindow.setCurrentIndex(parent.index);
                }
            }
        }
    }

    signal checkInfoSig();
    Rectangle{
        id:user_ima_rec;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        color:"transparent";

        width: imageLength;
        height: imageLength;
        radius: 30;

        Image{
            id:circle_ima;
            visible: false;
            source: "/res/circle.png";
        }

        Image {
            id:user_ima;
            source: "";
            cache: false;
            visible: false;
            sourceSize.width: imageLength;
            sourceSize.height:imageLength;
        }

        Image {
            id: status_ima;
            source: "/res/online.png";
            visible: false;
        }

        OpacityMask{
            source: user_ima;
            maskSource: circle_ima;
            anchors.fill: parent;
        }

        OpacityMask{
            property int margin: -1;
            source: status_ima;
            maskSource: circle_ima;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            anchors.rightMargin: margin;
            anchors.bottomMargin: margin;
            width: 16;
            height: width;
        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: Qt.PointingHandCursor;
            onClicked: {
                checkInfoSig();
            }
        }
    }

    signal addFriendSig();
    Rectangle{
        id:add_friend_rec;
        color:"transparent";
        anchors.bottom: user_ima_rec.top;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        radius: 8;
        width: 45;
        height: 45;

        Image {
            source: "/res/Plus.svg";
            anchors.fill: parent;
            anchors.margins: imaMargin;
        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: Qt.PointingHandCursor;
            hoverEnabled: true;
            onEntered: {
                parent.color="#50ffffff";
            }
            onExited: {
                parent.color="transparent";
            }
            onClicked: {
                addFriendSig();
            }
        }
    }
}
