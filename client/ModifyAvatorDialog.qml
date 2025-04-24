import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

Rectangle {
    color:"#000000";

    Rectangle{
        width: parent.width;
        height: caption.implicitHeight;
        border.color:"grey";
        border.width: 1;
        color:"#80000000";
    }

    signal closeSig();
    Rectangle{
        id:closeBtn;
        anchors.right: parent.right;
        height: caption.implicitHeight;
        width: height;
        color:"transparent";
        Image {
            anchors.fill: parent;
            anchors.margins: 5;
            source: "/res/x_white.svg"
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
    Text {
        id:caption;
        anchors.horizontalCenter: parent.horizontalCenter;
        text: captionText;
        color:"white";
        font.bold: true;
        font.pixelSize: 19;
    }

    property bool hasChosedFile: false;
    FileDialog {
        id: fileDialog
        title: "选择文件";
        fileMode: FileDialog.OpenFile;
        nameFilters: ["image files (*.jpg *.png *.jpeg)"];

        onAccepted: {
            hasChosedFile=true;
            flickable_ima.source=fileDialog.selectedFile;
        }
    }

    Rectangle{
        id:openFileBtn;
        anchors.centerIn: parent;
        radius: 8;
        width: openFileBtn_image.width+openFileBtn_text.width+25;
        height: 50;
        color:"white";
        visible: !hasChosedFile;

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: 8;
            samples: 16;
            color: "white"
        }

        Image {
            id: openFileBtn_image;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            height: parent.height-10;
            width: height;
            source: "/res/Folder.svg";
        }

        Text {
            id:openFileBtn_text;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: openFileBtn_image.right;
            anchors.leftMargin: 5;
            text: qsTr("选择文件");
            font.bold: true;
            font.pixelSize: 25;
        }

        MouseArea{
            anchors.fill: parent;
            onPressed: {
                parent.color="#F5F5F5";
            }
            onReleased: {
                parent.color="white";
            }
            onClicked: {
                fileDialog.open();
            }
        }
    }

    Flickable
    {
        id:flickable;
        anchors.top: caption.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 50;
        width: rimageWidth;
        height: rimageHeight;
        objectName: "flickable";
        visible: hasChosedFile;

        clip: true;
        contentHeight: flickable_ima.height;
        contentWidth: flickable_ima.width;

        boundsBehavior: Flickable.StopAtBounds;

        Image {
            id: flickable_ima;
            property int initWidth: 0;
            property int initHeight: 0;
            source: "";

            onStatusChanged: {
                if (status === Image.Ready) {
                    if(implicitWidth<implicitHeight){
                        width=flickable.width;
                        var scalex=flickable.width/implicitWidth;
                        height=scalex*implicitHeight;
                    } else {
                        height=flickable.height;
                        var scaley=flickable.height/implicitHeight;
                        width=scaley*implicitWidth;
                    }
                    initHeight=height;
                    initWidth=width;
                }
            }
        }

        MouseArea
        {
            anchors.fill: parent;
            cursorShape: Qt.SizeAllCursor;
        }

    }

    Slider
    {
        id: control;
        property int iniwidth: 0;
        property int iniheight: 0;
        width: 200;
        height: 30;
        from: 0;
        to:10;
        stepSize:1;
        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottomMargin: 75;
        value: 0;
        visible: hasChosedFile;
        snapMode:Slider.SnapAlways;
        background:
        Rectangle
        {
            x: control.leftPadding;
            y: control.topPadding + control.availableHeight / 2 - height / 2
            implicitWidth: 100
            implicitHeight: 4
            width: control.availableWidth
            height: implicitHeight
            radius: 2
            color: "#bdbebf"

            Rectangle
            {
                width: control.visualPosition * parent.width
                height: parent.height
                color: "#21be2b"
                radius: 2
            }
        }

        handle: Rectangle
        {
            x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
            y: control.topPadding + control.availableHeight / 2 - height / 2
            implicitWidth: 26
            implicitHeight: 26
            radius: 13
            color: control.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }

        onValueChanged:
        {
            flickable_ima.width=flickable_ima.initWidth+control.value*30;
            flickable_ima.height=flickable_ima.initHeight+control.value*30*flickable_ima.initHeight/flickable_ima.initWidth;
        }
    }

    Row{
        spacing: 30;
        visible: hasChosedFile;
        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottomMargin: 35;
        Rectangle{
            width: 90;
            height: 30;
            color:"#00BFFF"
            radius: 5;
            Text {
                anchors.centerIn: parent;
                text: "确定"
                font.bold: true;
                color: "white";
                font.pixelSize: 15;
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
                    getImageData();
                }
            }
        }

        Rectangle{
            width: 90;
            height: 30;
            color:"#CD5C5C"
            radius: 5;
            Text {
                anchors.centerIn: parent;
                text: "取消"
                font.bold: true;
                color: "white";
                font.pixelSize: 15;
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
                    hasChosedFile=false;
                }
            }
        }
    }

    function getImageData()
    {

        flickable.grabToImage(function(result) {
            if (result) {
                modifydia.updateUserImage(result);
            }
        });
    }
}
