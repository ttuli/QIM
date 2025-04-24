import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    signal closeSig();
    signal minSig();
    signal modifyAvatorSig();
    signal modifyBackImaSig(int rwidth,int rheight);
    signal talkSig(string account);
    signal addSig(string account,string name);
    signal checkImageSig();
    signal deleteSig(string target);

    property bool sex_: false;
    property bool sex_copy: false;

    function setInfo(account,name,email,createTime,sex)
    {
        userName.text=name;
        accountInfo.text=account;
        emailInfo.text=email;
        timeInfo.text=createTime;
        sex_=sex;
        imageSource.source="image://imageProvider/"+account+"_avatar";
    }

    function refreshImage()
    {
        imageSource.source="";
        imageSource.source="image://imageProvider/"+accountInfo.text+"_avatar";
        background.source="";
        background.source="image://imageProvider/"+accountInfo.text+"_background";
    }

    Image {
        id: background
        source: "image://imageProvider/"+accountInfo.text+"_background";
        width: parent.width;
        height: 300;
        cache: type_===_MYSELF?false:true;
    }



    Rectangle{
        id:closeBtn;
        anchors.right: parent.right;
        height: 25;
        width: height;
        color:"transparent";
        Image {
            anchors.fill: parent;
            source: "/res/XMark.svg"
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
            onClicked: {
                closeSig();
            }
        }
    }
    Rectangle{
        id:minBtn;
        anchors.right: closeBtn.left;
        width: closeBtn.width;
        height: width;
        color:"transparent";
        Image {
            anchors.fill: parent;
            source: "/res/Minus.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="#F5F5F5";
            }
            onExited: {
                parent.color="transparent";
            }
            onClicked: {
                minSig();
            }
        }
    }

    Rectangle{
        id:mainArea;
        anchors.top: background.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        border.color: "#33000000";
        border.width: 1;

        Image {
            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.margins: 5;
            width: 20;
            height: 20;
            source: "/res/modify.svg"
            visible: type_==_MYSELF;
            ToolTip{id:modifyBackImaTip}
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor
                onEntered: {
                   modifyBackImaTip.show("修改背景",5000);
                }
                onExited: {
                    modifyBackImaTip.close();
                }
                onClicked: {
                    if(!loading&&background.status===Image.Ready)
                        modifyBackImaSig(background.width,background.height);
                }
            }
        }

        Rectangle{
            id:contour//图像轮廓
            width: userImage.width;
            height: userImage.height;
            radius: width/2;
            visible: false;
        }
        Image {
            id: imageSource;
            source: "";
            cache: type_===_MYSELF?false:true;
            visible: false;
            onStatusChanged: {
                if(status===Image.Ready)
                    initBlock.visible=false;
            }
            sourceSize.width: userImage.width;
            sourceSize.height: userImage.height;
        }
        OpacityMask{
            id:userImage;
            maskSource: contour;
            source: imageSource;
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.leftMargin: 30;
            anchors.topMargin: -height/2.5;
            width: imageMaxLength;
            height: width;
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8
                samples: 16
                color: "grey"
            }
            MouseArea{
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                onClicked: {
                    checkImageSig();
                }
            }
        }
        Image {
            anchors.bottom: userImage.bottom;
            anchors.left: userImage.right;
            width: 20;
            height: 20;
            source: "/res/modify.svg"
            visible: type_==_MYSELF;
            ToolTip{id:modifyAvatorTip}
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor
                onEntered: {
                   modifyAvatorTip.show("修改头像",5000);
                }
                onExited: {
                    modifyAvatorTip.close();
                }
                onClicked: {
                    if(!loading&&imageSource.status===Image.Ready)
                        modifyAvatorSig();
                }
            }
        }

        Text {
            id: userName;
            text: "";
            width: implicitWidth>150?150:implicitWidth;
            anchors.bottom: userImage.bottom;
            anchors.left: userImage.right;
            anchors.leftMargin: 10;
            anchors.bottomMargin: 25;
            elide: Text.ElideRight;
            font.bold: true;
            font.pixelSize: 20;
            Rectangle{
                height: 28;
                width: 100;
                visible: ediable;
                radius: 5;
                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color:"grey";
                }
                TextInput{
                    id:nameEdit;
                    anchors.fill: parent;
                    anchors.margins: 2;
                    clip: true;
                    font.pixelSize: mainInfoArea.fontSize-2;
                }
            }
        }


        Rectangle{
            id:mainInfoArea;
            color:"transparent";
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: userImage.bottom;
            anchors.bottomMargin: interactArea.height;
            property int fontSize: 19;

            Column{
                anchors.fill: parent;
                anchors.margins: 35;
                spacing: 15;
                Row{
                    id:accountInfoRec;
                    spacing: 5;
                    Image {
                        height: accountPrefix.implicitHeight;
                        width: height;
                        source: "/res/userinfo_account.svg";
                    }
                    Text {
                        id:accountPrefix;
                        text: "账号: ";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }
                    Rectangle{
                        height: accountInfo.height;
                        width: accountInfo.width+accountInfo.height;
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                accoutCopy.visible=true;
                            }
                            onExited: {
                                accoutCopy.visible=false;
                            }
                        }
                        Text{
                            id: accountInfo
                            text: "1000000001";
                            font.pixelSize: mainInfoArea.fontSize;
                            font.bold: true;
                        }
                        TextArea{
                            id:accoutText;
                            text: accountInfo.text;
                            visible: false;
                        }
                        Image {
                            id:accoutCopy;
                            height: accountPrefix.implicitHeight;
                            width: height;
                            anchors.left: accountInfo.right;
                            source: "/res/userinfo_copy.svg"
                            visible: false;
                            MouseArea{
                                anchors.fill: parent;
                                cursorShape: Qt.PointingHandCursor;
                                onClicked: {
                                    accoutText.selectAll();
                                    accoutText.copy();
                                    accoutText.select(0,0);
                                    messagePop.showMessage("复制成功",1,1200);
                                }
                            }
                        }
                    }
                }
                Row{
                    id:emailtInfoRec;
                    spacing: 5;
                    Image {
                        height: emailPrefix.implicitHeight;
                        width:height;
                        source: "/res/userinfo_email.svg"
                    }
                    Text {
                        id:emailPrefix;
                        text: "邮箱: ";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }

                    Rectangle{
                        height: emailInfo.height;
                        width: emailInfo.width+emailInfo.height;
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                if(!ediable)
                                emailCopy.visible=true;
                            }
                            onExited: {
                                emailCopy.visible=false;
                            }
                        }
                        Text {
                            id: emailInfo
                            text: "koung123@126.com";
                            font.pixelSize: mainInfoArea.fontSize;
                            font.bold: true;
                            visible: !ediable
                        }
                        Rectangle{
                            height: emailPrefix.implicitHeight;
                            width: 150;
                            visible: ediable;
                            radius: 5;
                            layer.enabled: true;
                            layer.effect: DropShadow{
                                radius: 8;
                                samples: 16;
                                color:"grey";
                            }
                            TextInput{
                                id:emailEdit;
                                anchors.fill: parent;
                                anchors.margins: 2;
                                clip: true;
                                font.pixelSize: mainInfoArea.fontSize-2;
                            }
                        }

                        TextArea{
                            id:emailText;
                            text: emailInfo.text;
                            visible: false;
                        }
                        Image {
                            id:emailCopy;
                            height: emailPrefix.implicitHeight;
                            width: height;
                            anchors.left: emailInfo.right;
                            source: "/res/userinfo_copy.svg"
                            visible: false;
                            MouseArea{
                                anchors.fill: parent;
                                cursorShape: Qt.PointingHandCursor;
                                onClicked: {
                                    emailText.selectAll();
                                    emailText.copy();
                                    emailText.select(0,0);
                                    messagePop.showMessage("复制成功",1,1200);
                                }
                            }
                        }
                    }
                }
                Row{
                    id:sexInfoRec;
                    spacing: 5;
                    Image {
                        height: sexPrefix.height;
                        width: height;
                        source: "/res/userinfo_sex.svg";
                    }
                    Text {
                        id:sexPrefix;
                        text: "性别: ";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }
                    Text {
                        id: sexInfo;
                        text: sex_?"女":"男";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }
                    Image {
                        height: sexPrefix.height;
                        width: height;
                        source: sex_?"/res/userinfo_female.svg":"/res/userinfo_male.svg";
                    }
                    Image {
                        height: sexPrefix.height;
                        width: height;
                        source: "/res/flushs.svg"
                        visible: ediable;
                        MouseArea{
                            anchors.fill: parent;
                            onClicked: {
                                sex_=!sex_;
                            }
                        }
                    }
                }
                Row{
                    id:timetInfoRec;
                    spacing: 5;
                    Image {
                        height: timePrefix.height;
                        width: height;
                        source: "/res/useringo_time.svg";
                    }
                    Text {
                        id:timePrefix;
                        text: "创建时间: ";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }
                    Text {
                        id: timeInfo
                        text: "2025-2-2";
                        font.pixelSize: mainInfoArea.fontSize;
                        font.bold: true;
                    }
                }
            }
        }

        Rectangle{
            id:interactArea;
            anchors.bottom: parent.bottom;
            width: parent.width;
            color: "white";
            border.width: 1;
            border.color: "#33000000";
            height: 60;
            Rectangle{
                width: parent.border.width;
                height: parent.height;
                color: parent.color;
            }
            Rectangle{
                width: parent.border.width;
                height: parent.height;
                anchors.right: parent.right;
                color: parent.color;
            }

            Rectangle{
                id:btnAtRight;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                width: 140;
                height: parent.height-10;
                color:"#00BFFF"
                radius: 5;
                Text {
                    anchors.centerIn: parent;
                    text: {
                        if(type_===_MYSELF)
                            return "修改信息";
                        else if(type_===_STRANGER)
                            return "+添加";
                        else if(type_===_FRIEND)
                            return "发送消息";
                        else if(type_==_SENT)
                            return "已发送申请"
                    }

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
                        if(loading||userName.text==="")
                            return;
                        if(type_===_MYSELF){
                            setEdiable(true);
                        } else if (type_===_STRANGER) {
                            addSig(accountInfo.text,userName.text);
                        } else if(type_===_FRIEND){
                            talkSig(accountInfo.text);
                        } else if (type_==_SENT){

                        }
                    }
                } 
            }

            Rectangle{
                id:deleteBtn;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: btnAtRight.left;
                anchors.rightMargin: 10;
                width: 140;
                height: parent.height-10;
                color:"red"
                radius: 5;
                visible: type_===_FRIEND;
                Text {
                    anchors.centerIn: parent;
                    text: "删除好友";
                    font.bold: true;
                    color: "white";
                    font.pixelSize: 20;
                }
                Rectangle{
                    id:blackRec_;
                    anchors.fill: parent;
                    radius: parent.radius;
                    color: "#20000000";
                    visible: false;
                }
                Rectangle{
                    id:whiteRec_;
                    anchors.fill: parent;
                    radius: parent.radius;
                    color:"#20ffffff";
                    visible: false;
                }

                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: {
                        whiteRec_.visible=true;
                        blackRec_.visible=false;
                    }
                    onExited: {
                        whiteRec_.visible=false;
                        blackRec_.visible=false;
                    }
                    onPressed: {
                        whiteRec_.visible=false;
                        blackRec_.visible=true;
                    }
                    onReleased: {
                        whiteRec_.visible=true;
                        blackRec_.visible=false;
                    }
                    onClicked: {
                        if(loading)
                            return;
                        deleteSig(accountInfo.text);
                    }
                }

            }
            Row{
                spacing: 30;
                visible: ediable;
                anchors.centerIn: parent;
                Rectangle{
                    width: 110;
                    height: interactArea.height-20;
                    color:"#00BFFF"
                    radius: 5;
                    Text {
                        anchors.centerIn: parent;
                        text: "提交修改"
                        font.bold: true;
                        color: "white";
                        font.pixelSize: 18;
                    }
                    Rectangle{
                        id:blackRec_confirm;
                        anchors.fill: parent;
                        radius: parent.radius;
                        color: "#20000000";
                        visible: false;
                    }
                    Rectangle{
                        id:whiteRec_confirm;
                        anchors.fill: parent;
                        radius: parent.radius;
                        color:"#20ffffff";
                        visible: false;
                    }

                    MouseArea{
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: loading?Qt.ForbiddenCursor:Qt.ArrowCursor;
                        onEntered: {
                            whiteRec_confirm.visible=true;
                            blackRec_confirm.visible=false;
                        }
                        onExited: {
                            whiteRec_confirm.visible=false;
                            blackRec_confirm.visible=false;
                        }
                        onPressed: {
                            whiteRec_confirm.visible=false;
                            blackRec_confirm.visible=true;
                        }
                        onReleased: {
                            whiteRec_confirm.visible=true;
                            blackRec_confirm.visible=false;
                        }
                        onClicked: {
                            if(loading)
                                return;
                            loading=true;
                            checkInfo();
                        }
                    }
                }

                Rectangle{
                    width: 110;
                    height: interactArea.height-20;
                    color:"#CD5C5C"
                    radius: 5;
                    Text {
                        anchors.centerIn: parent;
                        text: "取消"
                        font.bold: true;
                        color: "white";
                        font.pixelSize: 18;
                    }
                    Rectangle{
                        id:blackRec_cancel;
                        anchors.fill: parent;
                        radius: parent.radius;
                        color: "#20000000";
                        visible: false;
                    }
                    Rectangle{
                        id:whiteRec_cancel;
                        anchors.fill: parent;
                        radius: parent.radius;
                        color:"#20ffffff";
                        visible: false;
                    }

                    MouseArea{
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: loading?Qt.ForbiddenCursor:Qt.ArrowCursor;
                        onEntered: {
                            whiteRec_cancel.visible=true;
                            blackRec_cancel.visible=false;
                        }
                        onExited: {
                            whiteRec_cancel.visible=false;
                            blackRec_cancel.visible=false;
                        }
                        onPressed: {
                            whiteRec_cancel.visible=false;
                            blackRec_cancel.visible=true;
                        }
                        onReleased: {
                            whiteRec_cancel.visible=true;
                            blackRec_cancel.visible=false;
                        }
                        onClicked: {
                            if(loading)
                                return;
                            sex_=sex_copy;
                            setEdiable(false);
                        }
                    }
                }
            }
        }
    }

    property bool ediable: false;
    function setEdiable(val)
    {
        if(val)
            sex_copy=sex_;
        ediable=val;
        btnAtRight.visible=!ediable;
        nameEdit.text="";
        emailEdit.text="";
    }

    function init(type)
    {
        type_=type;
        loading=false;
    }
    property int type_: 0;
    MessagePopup{
        id:messagePop;
        anchors.fill: parent;
    }

    signal changeInfoSig(string name,string email,int sex);
    property bool loading: false;
    function showInfo(msg,type,duration)
    {
        loading=false;
        messagePop.showMessage(msg,type,duration);
    }
    function changeInfoSuccess()
    {
        if(nameEdit.length)
            userName.text=nameEdit.text;
        if(emailEdit.length)
            emailInfo.text=emailEdit.text;
        setEdiable(false);
    }
    function waitForImaChange()
    {
        loading=true;
        messagePop.showMessage("正在修改",0,maxwaittime);
    }

    function checkInfo()
    {
        if(!nameEdit.length&&!emailEdit.length){
            setEdiable(false);
            loading=false;
            return;
        }
        messagePop.showMessage("正在提交修改",0,maxwaittime);
        var emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
        if(nameEdit.length>12||nameEdit.text.indexOf('-')!==-1){
            showInfo("用户名不合法",2,1200);
            return;
        }
        if(emailEdit.length&&!emailRegex.test(emailEdit.text)){
            showInfo("邮箱不合法",2,1200);
            return;
        }
        var nametext=accountInfo.text;
        var emailtext=emailInfo.text;
        if(nameEdit.length){
            nametext=nameEdit.text;
        }
        if(emailEdit.length){
            emailtext=emailEdit.text;
        }

        changeInfoSig(nametext,emailtext,sex_);
    }
    BlockingDialog{
        id:initBlock;
        mtext: "正在加载";
        anchors.fill: parent;
        visible: true;
    }
}
