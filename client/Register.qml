import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle {
    id:window;

    gradient: Gradient{
        orientation: Qt.Horizontal
        GradientStop{
            position: 0;
            color:"#a8ff78";
        }
        GradientStop{
            position: 1;
            color:"#78ffd6";
        }
    }


    signal closeSig();

    MessagePopup{
        id:msgPopup;
        anchors.fill: parent;
    }

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
        id:caption;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 70;
        width: 100;
        height: 40;
        color:"transparent";

        Image {
            id:caption_image;
            source: "/res/IM_ICON.svg";
            anchors.left: parent.left;
            height: parent.height;
            width: height;
        }

        Text {
            text: qsTr("IM");
            font.bold: true;
            font.pixelSize: 30;
            anchors.left: caption_image.right;
            anchors.leftMargin: 10;
            verticalAlignment: Text.AlignVCenter;
        }
    }

    property int textWidth: 40;
    property int inputWidth: 100;
    property int fontSize: 20;
    property int recWidth: 350;
    property int recHeight: 40;
    property int textLeftMargin: 10;
    property int minterval: 5;

    Column{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 150;
        spacing: 10;


        Rectangle{
            id:username_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";
            border.color: "red";
            border.width: 0;

            Text {
                id: username_text;
                text: qsTr("用户名:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;

            }

            Rectangle{
                id:username_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: username_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                border.color: "red";
                border.width: 0;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                TextInput{
                    id:username_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;

                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "1-12位不包含-";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        pwd_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        pwd_field.focus=true;
                    }

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }
        }

        Rectangle{
            id:pwd_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";

            Text {
                id: pwd_text;
                text: qsTr("密码:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:pwd_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: pwd_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                border.color: "red";
                border.width: 0;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                TextInput{
                    id:pwd_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;
                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "6-15位数字,大小写字母或_";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        cpwd_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        cpwd_field.focus=true;
                    }

                    echoMode: "Password";

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }
        }

        Rectangle{
            id:cpwd_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";


            Text {
                id: cpwd_text;
                text: qsTr("确认密码:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:cpwd_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: cpwd_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                border.color: "red";
                border.width: 0;

                TextInput{
                    id:cpwd_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;

                    echoMode: "Password";

                    Keys.onReturnPressed: {
                        email_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        email_field.focus=true;
                    }

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }

        }

        Rectangle{
            id:email_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";

            Text {
                id: email_text;
                text: qsTr("邮箱:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:email_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: email_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                border.color: "red";
                border.width: 0;

                TextInput{
                    id:email_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "可选";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        doRegister();
                    }
                    Keys.onEnterPressed: {
                        doRegister();
                    }

                    verticalAlignment: TextInput.AlignVCenter;

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }

            }
        }
    }

    property bool registering: false;
    Rectangle{
        id:btn;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: 8;
            samples: 16;
            color: "grey";
        }

        color:"white";
        radius: 5;

        width: 150;
        height: 50;

        Text {
            id: btn_text;
            text: qsTr("立即注册");
            font.bold: true;
            font.pixelSize: fontSize;
            anchors.centerIn: parent;
        }
        Rectangle{
            id:btn_hover
            anchors.fill: parent;
            color:"#80000000";
            opacity: registering?1:0;
            radius:parent.radius;
            Behavior on opacity {
                NumberAnimation{
                    duration: 100;
                }
            }
        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: registering?Qt.ForbiddenCursor:Qt.PointingHandCursor
            onClicked: {
                if(registering)
                    return;
                doRegister();
            }
            onPressed: {
                parent.scale=0.9;
            }
            onReleased: {
                parent.scale=1;
            }
        }

        Behavior on scale {
            NumberAnimation{
                duration: 80;
            }
        }
    }

    function doRegister()
    {
        var res1=checkname();
        var res2=checkpwd();
        var res3=checkemail();

        if(res1&&res2&&res3){
            registering=true;
            showInfo("注册中...",0,10000)
            registerSig(username_field.text,pwd_field.text,email_field.text);
        } else {
            showInfo("输入错误",2,2000);
        }
    }


    Rectangle{
        id:finish_rec;
        anchors.fill: parent;
        color: "#50000000";
        visible: false;

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
        }
    }

    function init()
    {
        pwd_field.text="";
        username_field.text="";
        cpwd_field.text="";
        email_field.text="";
        pwd_field_rec.border.width=0;
        username_field_rec.border.width=0;
        cpwd_field_rec.border.width=0;
        email_field_rec.border.width=0;
        closebtn.color="transparent";
        registering=false;
    }

    signal registerSig(string uname,string pwd,string email);

    function checkname()
    {
        username_field_rec.border.width=0;
        if(username_field.length>12||username_field.text.indexOf('-')!==-1||username_field.length===0){
            username_field_rec.border.width=2;
            return false;
        }
        return true;
    }


    function checkpwd()
    {
        var regex = /^[a-zA-Z0-9_]{6,15}$/;
        pwd_field_rec.border.width=0;
        cpwd_field_rec.border.width=0;
        if(!regex.test(pwd_field.text)||pwd_field.text!==cpwd_field.text){
            pwd_field_rec.border.width=2;
            cpwd_field_rec.border.width=2;
            return false;
        }
        return true;
    }

    function checkemail()
    {
        if(email_field.length!==0){
            var emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
            email_field_rec.border.width=0;
            if(!emailRegex.test(email_field.text)){
                email_field_rec.border.width=2;
                return false;
            }
        }
        return true;
    }

    Timer{
        id:successClose;
        onTriggered: {
            stop();
            init();
            closeSig();
        }
    }

    function showInfo(text,type,duration)
    {
        if(type===2)//失败时才可以继续注册
            registering=false;
        if(type===1){
            successClose.interval=duration;
            successClose.start();
        }
        msgPopup.showMessage(text,type,duration);
    }
}
