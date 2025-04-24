import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Rectangle{
    color:"#000000";
    signal centerSig();
    signal resizeSig(int width,int height);
    signal saveSig();

    function setImageSource(value)
    {
        image.source="image://imageProvider/"+value;
        msgPop.showMessage("加载中",0,100000);
    }

    property real maxWidthRatio: 0.8
    property real maxHeightRatio: 0.8

    // 计算可用显示区域
    property int maxDisplayWidth: Screen.width * maxWidthRatio
    property int maxDisplayHeight: Screen.height * maxHeightRatio

    // 缩放属性
    property real currentScale: 1.0
    property real minScale: 0.1
    property real maxScale: 3.0

    // 居中显示
    Component.onCompleted: {
        centerSig();
    }

    function adjustWindowSize() {
        if (image.status !== Image.Ready) return;

        var imgWidth = image.implicitWidth;
        var imgHeight = image.implicitHeight;
        var imgRatio = imgWidth / imgHeight;
        var containerRatio = maxDisplayWidth / maxDisplayHeight;

        var newWidth, newHeight;

        // 根据宽高比决定如何调整大小
        if (imgRatio > containerRatio) {
            // 图片较宽，以宽度为基准
            newWidth = Math.min(maxDisplayWidth, imgWidth);
            newHeight = newWidth / imgRatio;
        } else {
            // 图片较高，以高度为基准
            newHeight = Math.min(maxDisplayHeight, imgHeight);
            newWidth = newHeight * imgRatio;
        }

        // 计算初始缩放比例
        currentScale = newWidth / imgWidth;

        resizeSig(newWidth + 40,newHeight + 40);
        centerSig();
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onDoubleClicked: {
            currentScale = 1.0;
            flickable.contentX = 0;
            flickable.contentY = 0;
            adjustWindowSize();
        }
        onWheel:function(wheel) {
            var scaleFactor = wheel.angleDelta.y > 0 ? 1.1 : 0.9;
            var newScale = currentScale * scaleFactor;

            // 限制缩放范围
            if (newScale >= minScale && newScale <= maxScale) {
                currentScale = newScale;

                flickable.contentX = image.width/2 - flickable.width / 2;
                flickable.contentY = image.height/2 - flickable.height / 2;
            }
        }
    }

    Flickable {
        id: flickable
        anchors.centerIn: parent
        width: image.width>parent.width?parent.width:image.width;
        height: image.height>parent.height?parent.height:image.height;
        contentWidth: image.width;
        contentHeight: image.height;
        clip: true;

        boundsBehavior: Flickable.StopAtBounds

        Image {
            id: image
            width: implicitWidth * currentScale
            height: implicitHeight * currentScale
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
            source: "";

            onStatusChanged: {
                if (status === Image.Ready) {
                    adjustWindowSize();
                    msgPop.showMessage("加载成功",1,1200);
                }
            }
        }

        MouseArea{
            anchors.fill: parent;
            onWheel:function(wheel) {
                var scaleFactor = wheel.angleDelta.y > 0 ? 1.1 : 0.9;
                var newScale = currentScale * scaleFactor;

                // 限制缩放范围
                if (newScale >= minScale && newScale <= maxScale) {
                    currentScale = newScale;

                    flickable.contentX = image.width/2 - flickable.width / 2;
                    flickable.contentY = image.height/2 - flickable.height / 2;
                }
            }
        }
    }

    // 底部控制栏
    Rectangle {
        id: controlBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#80000000"

        Row {
            anchors.centerIn: parent
            spacing: 10

            Rectangle{
                height: parent.height;
                width: height;
                color:"transparent";
                Image {
                    anchors.fill: parent;
                    source: "/res/big.svg"
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if (currentScale < maxScale) {
                            currentScale *= 1.2;
                        }
                    }
                }
            }

            Text {
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
                text: "缩放: " + (currentScale * 100).toFixed(0) + "%"
            }

            Rectangle{
                height: parent.height;
                width: height;
                color:"transparent";
                Image {
                    anchors.fill: parent;
                    source: "/res/small.svg"
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if (currentScale > minScale) {
                            currentScale /= 1.2;
                        }
                    }
                }
            }

            Rectangle{
                height: parent.height;
                width: height;
                color:"transparent";
                Image {
                    anchors.fill: parent;
                    source: "/res/reset.svg"
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        currentScale = 1.0;
                        flickable.contentX = 0;
                        flickable.contentY = 0;
                        //adjustWindowSize();
                    }
                }
            }

            Rectangle{
                height: parent.height;
                width: height;
                color:"transparent";
                Image {
                    anchors.fill: parent;
                    source: "/res/save.svg"
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        saveSig();
                    }
                }
            }
        }
    }

    MessagePopup{
        id:msgPop;
        anchors.fill:parent;
    }
}
