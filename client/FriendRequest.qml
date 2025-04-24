import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    color: "#F5F5F5";

    property string username: "value";
    property string account: "1000000001";
    property int btnWidth: 30;

    function setInfo(n,acc)
    {
        username=n;
        account=acc;
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter;
        text: "验证信息";
        font.bold: true;
        font.pixelSize:22;
    }

    signal closeSig();
    Rectangle{
        id:closebtn;
        width: btnWidth;
        height: width;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";

        Image {
            anchors.fill: parent;
            anchors.margins: 5;
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
                closeSig();
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: 80
            }
        }
    }//关闭

    Rectangle{
        id:contour;
        width: 40;
        height: width;
        radius: width/2;
        visible: false;
    }
    Image {
        id: sourceImage;
        source: "image://imageProvider/"+account+"_avatar";
        smooth: true;
        visible: false;
        sourceSize.width: avator.width;
        sourceSize.height: avator.height;
    }
    OpacityMask{
        id:avator;
        source: sourceImage;
        maskSource: contour;
        width: imageLength;
        height: imageLength;
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.topMargin: 65;
        anchors.leftMargin: 30;
    }
    Text {
        id: info_name;
        text: username;
        anchors.top: avator.top;
        anchors.left: avator.right;
        anchors.leftMargin: 5;
        font.bold: true;
        font.pixelSize:18;
    }
    Text {
        id: info_account;
        text: account;
        anchors.top: info_name.bottom;
        anchors.left: avator.right;
        anchors.margins: 5;
    }

    Rectangle{
        id:input_rec_all;
        radius: 8;
        anchors.top: avator.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 80;
        anchors.horizontalCenter: parent.horizontalCenter;
        color:"white";
        width: parent.width-50;

        layer.effect: DropShadow{
            radius: 8;
            samples: 16;
            color: "grey";
        }
        layer.enabled: true;

        Text {
            id: inputCaption;
            text: "留言:";
            font.bold: true;
            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.margins: 5;
            font.pixelSize:18;
        }
        Rectangle{
            id:sperator;
            color:"black";
            anchors.top: inputCaption.bottom;
            anchors.topMargin: 5;
            width: parent.width;
            height: 1;
        }

        Rectangle{
            id:input_rec;
            anchors.top: sperator.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: wordnum.top;
            anchors.margins: 3;
            radius: parent.radius;
            TextArea{
                id:input;
                anchors.fill:parent;
                anchors.margins: 1;
                textFormat: "RichText";
                color: "black";
                font.pixelSize: 18;
                wrapMode: TextEdit.WordWrap
                placeholderText: "单击以输入...";

                property int maxWidth: 50;
                property string laststring:"";

                onTextChanged: {
                    if(length>maxWidth){
                        text=laststring;
                    } else {
                        laststring=text;
                    }
                }
            }
        }
        Text {
            id: wordnum;
            text: input.length+"/"+input.maxWidth;
            anchors.bottom: parent.bottom;
            anchors.right: parent.right;
            anchors.margins: 5;
        }
    }
    Rectangle{
        id:btn;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: input_rec_all.bottom;
        anchors.topMargin: 20;
        width: 140;
        height: 45;
        color:"#00BFFF"
        radius: 5;
        Text {
            anchors.centerIn: parent;
            text: "发送";

            font.bold: true;
            color: "white";
            font.pixelSize: 20;
        }
        Rectangle{
            id:blackRec;
            anchors.fill: parent;
            radius: parent.radius;
            color: "#20000000";
            visible: false;
        }
        Rectangle{
            id:whiteRec;
            anchors.fill: parent;
            radius: parent.radius;
            color:"#20ffffff";
            visible: false;
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                whiteRec.visible=true;
                blackRec.visible=false;
            }
            onExited: {
                whiteRec.visible=false;
                blackRec.visible=false;
            }
            onPressed: {
                whiteRec.visible=false;
                blackRec.visible=true;
            }
            onReleased: {
                whiteRec.visible=true;
                blackRec.visible=false;
            }
            onClicked: {
                sendSig(input.text);
            }
        }
    }

    signal sendSig(string msg);
}
