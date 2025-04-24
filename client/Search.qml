import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    color: "#fafafa"

    property int btnWidth: 30;

    signal closeSig();
    signal userInfoSig(string account,int pos);
    Rectangle{
        id:closebtn;
        width: btnWidth;
        height: width;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";
        z:1;

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

    signal minSig();
    Rectangle{
        id:minbtn;
        height: width;
        width: btnWidth;
        anchors.right: closebtn.left;
        color:"transparent";
        z:1;

        Image {
            source: "/res/Minus.svg";
            anchors.fill: parent;
            anchors.margins: 5;
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;

            onClicked: {
                minSig();
            }

            onEntered: {
                parent.color="#c9d6df";
            }
            onExited: {
                parent.color="transparent";
            }
        }

    }//最小化

    property string searchMode: "person";
    property string preSearchMode: "person";
    property int offset: 0;
    property string currentContent: "";
    property int currentPattern: 0;

    signal clearSig();

    Rectangle{
        id:btnarea;
        width: window.width;
        height: 180;

        gradient:Gradient{
            GradientStop{position: 0;color: "#56ccf2"}
            GradientStop{position: 1;color:"#2f80ed"}
        }


        Row{
            Layout.alignment: Qt.AlignHCenter;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 50;
            spacing: 80;

            Rectangle{
                id:findpersonbtn;
                color:"transparent";
                width: findpersonbtn_text.implicitWidth;
                height: findpersonbtn_text.implicitHeight;

                Text {
                    id: findpersonbtn_text;
                    anchors.centerIn: parent;
                    text: qsTr("找人");
                    color: "white";
                    font.bold: true;
                    font.pixelSize: 35;
                }

                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if(searchMode=="person")
                            return;
                        clearSig();
                        parent.scale=1;
                        findgroupbtn.scale=0.8;
                        searchMode="person";
                    }
                }
            }

            Rectangle{
                id:findgroupbtn;
                color:"transparent";
                width: findgroupbtn_text.implicitWidth;
                height: findgroupbtn_text.implicitHeight;
                scale: 0.8;

                Text {
                    id: findgroupbtn_text;
                    anchors.centerIn: parent;
                    text: qsTr("找群");
                    color: "white";
                    font.bold: true;
                    font.pixelSize: 35;
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if(searchMode=="group")
                            return;
                        clearSig();
                        parent.scale=1;
                        findpersonbtn.scale=0.8;
                        searchMode="group";
                    }
                }
            }
        }

        Rectangle{
            id:input_rec;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: 10;
            color: "#00BFFF";
            radius: 20;
            width: 400;
            height: 50;
            clip: true;

            TextField{
                id:input;
                anchors.fill: parent;
                anchors.rightMargin: 50;
                background: Rectangle{
                    color:"white";
                    topLeftRadius: 20;
                    bottomLeftRadius: 20;
                }
                font.pixelSize: 20;
                font.bold: true;
                color: "black";
                Keys.onReturnPressed: {
                    searchbtn.handleClick();
                }
                Keys.onEnterPressed: {
                    searchbtn.handleClick();
                }
            }

            Image {
                id:searchbtn;
                source: "/res/search.svg";
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.rightMargin: 10;
                width: 35;
                height: 35;
                property bool canClick: true

                Timer {
                    id: clickTimer
                    interval: 1000
                    onTriggered: parent.canClick=true;
                }
                function handleClick()
                {
                    if(canClick){
                        checkContent();
                        canClick=false;
                        clickTimer.start();
                    }
                }

                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        parent.handleClick();
                    }
                }
            }

        }
    }

    signal searchSig(string mode,int pattern,string content,int offset);

    function checkContent()
    {
        if(preSearchMode==searchMode&&currentContent===input.text)
            return;
        var rule1=/^[0-9]+$/;
        setFinishVisible(true);

        //0代表按账号，1代表按名字
        currentContent=input.text;
        offset=0;
        preSearchMode=searchMode;
        if(rule1.test(currentContent)){
            searchSig(searchMode,0,currentContent,offset);
            currentPattern=0;
        }else{
            searchSig(searchMode,1,currentContent,offset);
            currentPattern=1;
        }
    }

    property int cellwidth: width/3;
    property int cellheight: 90;

    signal addSig(string target,string name,int id);
    Rectangle {
        id: resultsContainer
        anchors.top: btnarea.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        color: "#F5F5F5"
        border.color: "#eeeeee"
        radius: 4

        GridView{
            id:gridview;
            anchors.fill: parent;

            clip: true;
            cellWidth:cellwidth;
            cellHeight:cellheight;

            model:searchModel;

            ScrollBar.vertical: ScrollBar { }

            onAtYEndChanged: {
                if (atYEnd&&count) {
                    offset++;
                    searchSig(searchMode,currentPattern,currentContent,offset);
                }
            }

            delegate: Rectangle{
                width: cellwidth;
                height: cellheight;

                color: "transparent";
                Rectangle{
                    anchors.fill: parent;
                    anchors.margins: 8;
                    radius: 8;
                    color:"white";

                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 8;
                        samples: 16;
                        color:"grey";
                    }

                    Image {
                        id: circle_ima;
                        source: "/res/circle.png";
                        visible: false;
                    }
                    Image {
                        id: userimage;
                        source: "image://imageProvider/"+user_account+"_avatar";
                        visible: false;
                        sourceSize.width: headimage.width;
                        sourceSize.height: headimage.height;
                    }

                    OpacityMask{
                        id:headimage;
                        source:userimage;
                        maskSource: circle_ima;
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 6;
                        width: imageLength;
                        height: imageLength;
                        MouseArea{
                            anchors.fill: parent;
                            cursorShape: Qt.PointingHandCursor;
                            onClicked: {
                                userInfoSig(user_account,index);
                            }
                        }
                    }

                    Text {
                        id: username;
                        text: user_name;
                        font.bold: true;
                        font.pixelSize: 20;
                        anchors.left: headimage.right;
                        anchors.right: parent.right;
                        anchors.top:parent.top;
                        anchors.topMargin: 8;
                        elide: Text.ElideRight;
                        anchors.leftMargin: 10;
                    }
                    Text {
                        id: usersex
                        text: user_sex===0?"男":"女";
                        font.pixelSize: 16;
                        anchors.left: username.left;
                        anchors.top: username.bottom;
                        anchors.margins: 5;
                    }
                    Image {
                        anchors.left: usersex.right;
                        anchors.top: usersex.top;
                        height: usersex.height;
                        width: height;
                        source: user_sex!==0?"/res/userinfo_female.svg":"/res/userinfo_male.svg";
                    }

                    Rectangle{
                        id:addbtn;
                        color: user_applystate?"transparent":"#00CD66";
                        width: btnText.implicitWidth+20;
                        height: 30;
                        radius: 5;
                        anchors.right: parent.right;
                        anchors.bottom: parent.bottom;
                        anchors.rightMargin: 5;
                        anchors.bottomMargin: 3;

                        Text {
                            id:btnText;
                            anchors.centerIn: parent;
                            text: user_applystate?"√ 已申请":"+添加";
                            color: user_applystate?"#00CD66":"white";
                            font.bold: true;
                            font.pixelSize: 15;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            cursorShape: user_applystate?Qt.ArrowCursor:Qt.PointingHandCursor;
                            onClicked: {
                                if(user_applystate)
                                    return;
                                addSig(user_account,user_name,index);
                            }
                        }
                    }
                }
            }
        }
    }

    function setFinishVisible(value)
    {
        finish_rec.visible=value;
    }

    function setMsg(msg,type,duration)
    {
        setFinishVisible(false);
        messagePopup.showMessage(msg,type,duration);
    }

    BlockingDialog{
        id:finish_rec;
        anchors.fill: parent;
        mtext: "正在执行...";
        visible: false;
    }


    MessagePopup {
        id: messagePopup
        anchors.fill: parent;
    }
}

