import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    property string color_: "#f9f7f7";
    property int maxPicWidth: 380;
    property bool multiChoiceState: false;

    color:color_;
    signal updateChoiceState(string id,int position,bool value);
    signal deleteSig();

    Component.onCompleted: {
        currentDialog.pullHistoryMsg();
    }


    function setMaxPicWidth(value)
    {
        maxPicWidth=value;
    }
    // type_custom
    // type_system
    // type_audio
    // type_video
    // type_file
    // type_image
    // type_text
    // type_timestamp

    //msg_sending
    //msg_sent
    //msg_fail

    Rectangle{
        id:caption_rec;
        width: parent.width;
        height: 30;
        color:color_;
        Text {
            id:caption_text;
            anchors.centerIn: parent;
            font.bold: true;
            font.pixelSize: 23;
            text:caption_content;
        }
    }

    Rectangle{
        id:notFriendTip;
        anchors.top: caption_rec.bottom;
        width: parent.width;
        color:"red";
        opacity: 0.2;
        height: !currentDialog.isNotFriend?0:notFriendTipText.height;
        Text {
            id: notFriendTipText;
            anchors.centerIn: parent;
            text: "对方与您还不是好友!";
            color:"red";
            visible: parent.height;
            font.pixelSize: 15;
        }
    }

    SplitView{
        anchors.bottom: parent.bottom;
        anchors.top: notFriendTip.bottom;
        width: parent.width;
        orientation: Qt.Vertical;

        spacing: 0;

        handle: Rectangle{
            implicitWidth: window.width;
            implicitHeight: 2;
            color:"#33000000"
        }

        Rectangle{
            id:listview_rec;
            width: parent.width;
            anchors.top: parent.top;
            SplitView.minimumHeight: parent.height/2;
            SplitView.preferredHeight: parent.height/1.2;
            color:color_;

            ListView{
                id:listview;
                anchors.fill:parent;
                model: chatDialogModel;
                clip:true;
                cacheBuffer: 80;
                spacing: 5;

                onCountChanged: {
                    positionViewAtEnd();
                    currentIndex=count-1;
                }


                onAtYBeginningChanged: {
                    if (listview.atYBeginning) {
                        currentDialog.pullHistoryMsg();
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    id: vScrollBar
                    active: false
                    policy: ScrollBar.AsNeeded
                }

                // roles[MessageIdRole] = "messageId";
                // roles[SenderIdRole] = "senderId";
                // roles[ContentRole] = "content";
                // roles[TimestampRole] = "timestamp";
                // roles[IsReadRole] = "isRead";
                // roles[TypeRole] = "type";
                // roles[StatusRole] = "msgstatus";
                // roles[ExtraDataRole] = "extraData";
                delegate: Rectangle{
                    width: listview.width;
                    height: {
                        if(type===type_text||type===type_image||type===type_file)
                            return msgRec.height+userName.height+20;
                        if(type==type_timestamp)
                            return time_rec.height;
                        return 0;
                    }
                    color:"transparent";
                    Rectangle{
                        color:"transparent";
                        anchors.fill: parent;
                        Image {
                            id: sourceImage;
                            source: type!==type_timestamp?"image://imageProvider/"+senderId+"_avatar":"";
                            visible: false;
                            cache: false;
                            sourceSize.width: avatar.width;
                            sourceSize.height: avatar.height;
                        }
                        Rectangle {
                            id: imageOutline;
                            width: avatar.width;
                            height: width;
                            radius: width/2;
                            smooth: true;
                            visible:false;
                        }
                        OpacityMask{
                            id:avatar;
                            maskSource: imageOutline;
                            source: sourceImage;
                            width: imageLength;
                            height: imageLength;
                            anchors.top: parent.top;
                            anchors.margins: 5;
                            visible: {
                                if(type===type_timestamp)
                                    return false;
                                return true;
                            }
                            Component.onCompleted: {
                                if(currentLoginAccount===senderId){
                                    anchors.right=parent.right;
                                } else {
                                    anchors.left=parent.left;
                                }
                            }
                        }//头像
                        Text {
                            id: userName;
                            text:(currentLoginAccount===senderId?loginName:caption_content);
                            anchors.top:avatar.top;
                            anchors.leftMargin: 5;
                            anchors.rightMargin: 5;
                            color:"black";
                            visible: {
                                if(type===type_timestamp)
                                    return false;
                                return true;
                            }
                            Component.onCompleted: {
                                if(currentLoginAccount===senderId){
                                    anchors.right=avatar.left;
                                } else {
                                    anchors.left=avatar.right;
                                }
                            }
                            font.pixelSize: 10;
                            opacity: 0.7;
                        }//用户名

                        Rectangle{
                            id:time_rec;
                            anchors.bottom: parent.bottom;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            color:"#30000000";
                            width: time_text.implicitWidth+25;
                            height: time_text.implicitHeight+15;
                            Text {
                                id:time_text;
                                text: content;
                                anchors.centerIn: parent;
                                color: "white";
                                font.pixelSize: 12;
                            }
                            visible:type===type_timestamp;
                        }//时间戳

                        Rectangle{
                            id:msgRec;
                            radius: 8;
                            color:{
                                if(type===type_timestamp||type==type_image||type==type_file)
                                    return "transparent";
                                else
                                    return currentLoginAccount===senderId?"#3eede7":"white";
                            }

                            property int maxWidth: listview.width-120;

                            width: {
                                if(type===type_text){
                                    return textInLine.implicitWidth+5 < maxWidth ? textInLine.implicitWidth+5 : maxWidth;
                                }else if(type===type_image){
                                    return imageWidth;
                                } else if(type===type_file){
                                    return fileMsg.width;
                                }
                            }
                            height: {
                                if(type===type_text)
                                    return textInLine.implicitWidth+5 < maxWidth ? textInLine.implicitHeight : textarea.implicitHeight;
                                else if(type===type_image){
                                    return imageHeight;
                                } else if(type===type_file){
                                    return fileMsg.height;
                                }

                            }
                            anchors.top:userName.bottom;
                            anchors.topMargin: 5;
                            visible:true;

                            Component.onCompleted: {
                                if(currentLoginAccount===senderId){
                                    anchors.right=userName.right;
                                } else {
                                    anchors.left=userName.left;
                                }
                            }

                            CustomFileMsg{
                                id:fileMsg;
                                visible: type===type_file;
                                fileName_: content;
                                fileSize_:fileSize;
                                progress_: fileProgress;
                                msg_fail_: msg_fail;
                                msg_sending_: msg_sending;
                                msg_sent_: msg_sent;
                                msg_ready_:msg_ready;
                                msgstatus_:msgstatus;
                                isMyself: currentLoginAccount===senderId;
                                onCancelSig: {
                                    currentDialog.handleFileCancel(messageId,index);
                                }
                                onDownloadSig: {
                                    currentDialog.handleFileDownLoad(messageId,index);
                                }
                            }

                            Image {
                                id: picMSg;
                                anchors.fill: parent;
                                source: type===type_image?"image://"+"imageProvider/"+content:"";
                                visible: false;
                            }
                            OpacityMask{
                                id:picmsgMask;
                                source: picMSg;
                                maskSource: picMsgOutline;
                                width: imageWidth;
                                height: imageHeight;
                                visible: type===type_image;
                                MouseArea{
                                    anchors.fill: parent;
                                    acceptedButtons: Qt.RightButton|Qt.LeftButton;
                                    onPressed: {
                                        if(!multiChoiceState)
                                            backrec.visible=true;
                                    }
                                    onReleased: {
                                        backrec.visible=false;
                                    }
                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            var contextMenu = imageMenuComponent.createObject(opacitymask, {
                                                "x": mouse.x,
                                                "y": mouse.y,
                                              "msgId":msgid,
                                              "position":index
                                            })
                                            contextMenu.open();
                                        } else if (mouse.button===Qt.LeftButton){
                                            if(multiChoiceState){
                                                multiChoiceBtn.checked=!multiChoiceBtn.checked;
                                            }
                                        }
                                    }
                                    onDoubleClicked:function(mouse) {
                                        if(mouse.button===Qt.LeftButton){
                                            if(picMSg.status===Image.Ready)
                                                currentDialog.handleCheckHdPic(content);
                                        }
                                    }
                                }
                            }

                            Rectangle{
                                id:picMsgOutline;
                                anchors.fill: parent;
                                visible: false;
                                radius: msgRec.radius;
                            }

                            Rectangle{
                                id:backrec;
                                color: "#33000000";
                                anchors.fill: parent;
                                visible: false;
                                radius: parent.radius;
                            }


                            TextArea{
                                id:textarea;
                                anchors.fill:parent;
                                visible:{
                                    if(type!==type_timestamp&&type!==type_image&&type!==type_file)
                                        return true;
                                    return false;
                                }
                                readOnly: true;
                                textFormat: "RichText";
                                Component.onCompleted: {
                                    if(textInLine.implicitWidth+5 <= msgRec.maxWidth){
                                        textarea.wrapMode="NoWrap";
                                    } else {
                                        textarea.wrapMode="WrapAnywhere";
                                    }
                                }
                                selectedTextColor: "white";
                                selectionColor: "blue";
                                selectByMouse: true;
                                color: "black";

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.RightButton // 只接受右键点击
                                    hoverEnabled: true;
                                    cursorShape: Qt.IBeamCursor;

                                    onEntered: {
                                        backrec.visible=true;
                                    }
                                    onExited: {
                                        backrec.visible=false;
                                    }

                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            var contextMenu = contextMenuComponent.createObject(textarea ,{
                                                "x": mouse.x,
                                                "y": mouse.y,
                                                "textAreaRef": textarea,
                                                "msgId":messageId,
                                                "position":index
                                            })
                                            contextMenu.open();
                                        }
                                    }
                                }

                                TextArea{
                                    id:textInLine;
                                    textFormat: "RichText";
                                    text: parent.text;
                                    font.pixelSize: parent.font.pixelSize;
                                    visible: false;
                                    wrapMode: Text.NoWrap;
                                }

                                text: content;
                                font.pixelSize: fontSize;
                            }
                        }//消息
                        Image {
                            id:stateImage;
                            source:{
                                if(msgstatus===msg_sending)
                                    return "/res/loading.png";
                                if(msgstatus===msg_fail)
                                    return "/res/warning.svg";
                                if(msgstatus===msg_sent)
                                    return "";
                            }

                            width: 20;
                            height: width;
                            anchors.bottom: msgRec.bottom;
                            anchors.right: msgRec.left;
                            anchors.leftMargin: 12;
                            visible:{
                                if(msgstatus!==msg_sent&&
                                        type!==type_timestamp&&
                                        type!==type_file&&
                                        currentLoginAccount===senderId)
                                    return true;
                                return false;
                            }
                            rotation:{
                                //直接为0会出现msgstatus变化后图像旋转角度不为0
                                if(msgstatus===msg_sent)
                                    return 0;
                                else
                                    return 0;
                            }

                            RotationAnimation on rotation {
                                from: 0;
                                to: 360;
                                duration: 1000;
                                loops: Animation.Infinite;
                                running: msgstatus===msg_sending;
                            }
                        }//消息状态

                    }//子rectangle
                }
            }

            Popup{
                id:emojipopup;
                background: emoji_choice_rec;
                width: 350;
                height: 250;
                x:5;
                y:parent.height-height-10;
                GridView{
                    id:emoji_choice_view;
                    anchors.fill: parent;
                    clip: true;
                    boundsBehavior: Flickable.StopAtBounds;

                    property var emojis: ["😃", "😙", "🤣", "😎","🥰","😅",
                    "😇","🥰","😍","😘","🤐","😋","😶","🙄","😔","🤔","😖","🙁",
                    "🥴","🥶","🥵","👀","🤬","🤑","🤕","😷","🤪","🤢","🤮","💀","😺",
                    "🐈","🐕","🦾","🤺","💑","🍵","🍻","🍭","🎂","🍦","🍴","🥳","🤠",
                    "🤓","😮","😦","😨","😰","😥","😤","😈","🤡","💩","🧨","✅","🧩",
                    "🔚","📌","📊","🟢","🟥"]

                    cellWidth: width/8;
                    cellHeight: cellWidth;
                    model: emojis

                    interactive: true;

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    delegate: Rectangle{
                        width: emoji_choice_view.cellWidth;
                        height: emoji_choice_view.cellHeight;
                        color: "transparent";
                        Rectangle{
                            anchors.fill: parent;
                            anchors.margins: 5;
                            radius: 5;
                            color:"white";
                            Text {
                                anchors.centerIn: parent;
                                text: modelData;
                                font.pixelSize: 20;
                            }
                            MouseArea{
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onEntered: {
                                    parent.color="grey";
                                }
                                onExited: {
                                    parent.color="white";
                                }
                                onClicked: {
                                    textedit.insert(textedit.cursorPosition,modelData);
                                    textedit.cursorPosition++;
                                    emojipopup.close();
                                }
                            }
                        }
                    }
                }
            }

            Rectangle{
                id:emoji_choice_rec;
                color:"white";
                radius: 8;
                width: 350;
                height: 250;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: emoji_choice_rec.radius;
                    samples: 16;
                    color: "#50000000";
                }

                Behavior on visible{
                    NumberAnimation{
                        duration: 80;
                    }
                }
            }


        }
        Rectangle{
            id:input_rec;
            width: parent.width;
            color: color_;
            SplitView.minimumHeight: 150;
            SplitView.preferredHeight: 200;

            Rectangle{
                id:choice_rec;
                width: parent.width;
                height: 25;
                color:"transparent"
                Image {
                    id: emoji;
                    source: "/res/FaceSmile.svg"
                    height: parent.height;
                    width: height;
                    anchors.left: parent.left;
                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            if(!emojipopup.opened)
                                emojipopup.open();
                            else
                                emojipopup.close();
                        }
                    }
                }
                Image {
                    id: picture;
                    source: "/res/Photo.svg";
                    height: parent.height;
                    width: height;
                    anchors.left: emoji.right;
                    anchors.leftMargin: 5;
                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            currentDialog.handleSendPic();
                        }
                    }
                }
                Image {
                    id:sendfile;
                    source: "/res/FolderOpen.svg";
                    height: parent.height;
                    width: height;
                    anchors.left: picture.right;
                    anchors.leftMargin: 5;
                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            currentDialog.handleSendFile();
                        }
                    }
                }

            }

            TextEdit{
                id:textedit;
                anchors.bottom: parent.bottom;
                anchors.top: choice_rec.bottom;
                width: parent.width;
                font.pixelSize: fontSize;
                textFormat: TextEdit.RichText;
                topPadding: 5;
                wrapMode:implicitWidth+5>width?"WrapAnywhere":"NoWrap";

                Keys.onPressed:(event)=> {
                    if (event.key === Qt.Key_Return||event.key === Qt.Key_Enter) {
                        if(event.modifiers & Qt.ShiftModifier)
                            textedit.append("");
                        else
                            sendMsg();
                        event.accepted = true;
                    }
                }

                MouseArea{
                    anchors.fill: parent;
                    acceptedButtons: Qt.RightButton;
                    propagateComposedEvents: true;
                    cursorShape: Qt.IBeamCursor;
                    onClicked: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            var contextMenu = inputMenuComponent.createObject(textedit, {
                                "x": mouse.x,
                                "y": mouse.y,
                                "tarEdit":textedit
                            })
                            contextMenu.open();
                        }
                    }
                }
            }

            Rectangle{
                id:sendBtn;
                anchors.bottom: parent.bottom;
                anchors.right: parent.right;
                anchors.bottomMargin: 10;
                anchors.rightMargin:15;
                width: 60;
                height: 25;
                radius: 8;
                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color:"#33000000";
                }

                Text {
                    text: "发送";
                    anchors.centerIn: parent;
                    font.bold: true;
                    font.pixelSize: 14;
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onPressed: {
                        parent.color="#F5F5F5";
                    }
                    onReleased: {
                        parent.color="white";
                    }
                    onClicked: {
                        sendMsg();
                    }
                }
            }

            Rectangle{
                anchors.fill: parent;
                color:"white";
                visible: false;
                MouseArea{
                    anchors.fill: parent;
                }

                Row{
                    anchors.centerIn: parent;
                    spacing: 20;
                    Rectangle{
                        id:deleteBtn;
                        width: 60;
                        height: width;
                        radius: width/2;
                        smooth: true;

                        layer.enabled: true;
                        layer.effect: DropShadow{
                            radius: 8;
                            samples: 16;
                            color: "gray";
                        }
                        Image {
                            id:deleteIcon;
                            source: "/res/Trash.svg"
                            width: 27;
                            height: width;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.top: parent.top;
                            anchors.topMargin: 10;
                        }
                        Text {
                            anchors.bottom: parent.bottom;
                            anchors.bottomMargin: 5;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            text:"删除";
                            font.pixelSize: 11;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                parent.color="red";
                            }
                            onExited: {
                                parent.color="white";
                            }
                            onClicked: {
                                deleteSig();
                                multiChoiceState=false;
                            }
                        }
                    }

                    Rectangle{
                        id:cancelBtn;
                        width: 60;
                        height: width;
                        radius: width/2;
                        smooth: true;

                        layer.enabled: true;
                        layer.effect: DropShadow{
                            radius:8;
                            samples: 16;
                            color: "gray";
                        }
                        Image {
                            id:cancelIcon;
                            source: "/res/XMark.svg"
                            width: 27;
                            height: width;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.top: parent.top;
                            anchors.topMargin: 10;
                        }
                        Text {
                            anchors.bottom: parent.bottom;
                            anchors.bottomMargin: 5;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            text:"取消";
                            font.pixelSize: 11;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                parent.color="#F5F5F5";
                            }
                            onExited: {
                                parent.color="white";
                            }
                            onClicked: {

                            }
                        }
                    }
                }
            }
        }   
    }

    property int fontSize: 15;

    function sendMsg()
    {
        if(textedit.length===0)
            return;
        currentDialog.handleSendMsg(textedit.text,type_text)
        textedit.clear();
    }

    Component{
        id:inputMenuComponent;
        CustomMenu{
            id:inputMenu;
            property var tarEdit: null;

            CustomMenuItem {
                mtext: "全选";
                itemRadius_top: inputMenu.backgroundRadius/2;
                onTriggered: {
                    tarEdit.selectAll();
                }
            }
            CustomMenuItem {
                mtext: "复制";
                onTriggered: {
                    tarEdit.copy();
                }
            }
            CustomMenuItem {
                mtext: "粘贴";
                itemRadius_bottom: inputMenu.backgroundRadius/2;
                onTriggered: {
                    tarEdit.paste();
                }
            }
        }
    }

    Component {
        id:imageMenuComponent;
        CustomMenu{
            id:imageMenu;
            property string msgId: "";
            property int position: -1;
            CustomMenuItem {
                mtext: "删除";
                itemRadius_top: imageMenu.backgroundRadius/2;
                onTriggered: {
                }
            }
            CustomMenuItem {
                mtext: "多选";
                itemRadius_bottom: imageMenu.backgroundRadius/2;
                onTriggered: {

                }
            }
        }
    }

    Component {
        id: contextMenuComponent

        CustomMenu {
            id:contextMenu;
            property var textAreaRef: null
            property string msgId: "";
            property int position: -1;
            CustomMenuItem {
                mtext: "复制";
                itemRadius_top: contextMenu.backgroundRadius/2;
                onTriggered: {
                    if(textAreaRef.selectedText==="")
                        textAreaRef.selectAll();
                    textAreaRef.copy();
                    textAreaRef.select(0,0);
                }
            }
            CustomMenuItem {
                mtext: "全选";
                onTriggered: {
                    textAreaRef.selectAll();
                }
            }
            CustomMenuItem {
                mtext: "删除";
                onTriggered: {
                    currentDialog.readyRemoveMsg(position,msgId);
                }
            }
            CustomMenuItem {
                mtext: "多选";
                itemRadius_bottom: contextMenu.backgroundRadius/2;
                onTriggered: {

                }
            }

        }
    }

    function setBlockDiaVisible(val)
    {
        blockDiaVisible=val;
    }
    property bool blockDiaVisible: false;
    BlockingDialog{
        id:blockdia;
        anchors.fill: parent;
        mtext: "正在执行...";
        visible: blockDiaVisible;
    }
    function showInfo(text,type,duration)
    {
        msgPop.showMessage(text,type,duration);
    }
    MessagePopup{
        id:msgPop;
        anchors.fill: parent;
    }
}
