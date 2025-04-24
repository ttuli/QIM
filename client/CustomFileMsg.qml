import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Effects
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Item {
    property string fileName_: "设计文档.pdf"          // 文件名称
    property var fileSize_;              // 文件大小(字节)

    property string fileIcon: "/res/Folder.svg"   // 文件图标
    property real progress_: 0                   // 下载进度(0-1)
    property int msg_sent_: 0;
    property int msg_sending_: 0;
    property int msg_fail_: 0;
    property int msgstatus_: 0;
    property int msg_ready_: 0;
    property bool isMyself: false;

    // 气泡样式属性
    property color accentColor: "#0084ff"            // 主题色(仅用于高亮和按钮)
    property color borderColor: "#e0e0e0"            // 边框颜色

    signal cancelSig();
    signal downloadSig();

    width: 280
    height: 100
    // 文件消息气泡组件
    Rectangle {
        id: fileBubble
        anchors.fill: parent;
        radius: 12
        color: "#ffffff"                                 // 白色背景
        border.width: 1
        border.color: borderColor
        // 气泡内容
        Rectangle {
            anchors.fill: parent
            anchors.margins: 10
            anchors.rightMargin: 50;

            // 文件图标
            Rectangle {
                id:iconRec;
                width: 60
                height: 60;
                radius: 4
                color: "#f5f5f5"
                border.width: 1
                border.color: borderColor
                anchors.verticalCenter: parent.verticalCenter;

                Image {
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                    source: fileIcon
                    fillMode: Image.PreserveAspectFit
                }
            }

            // 文件信息
            Text {
                id:text_filename;
                text: fileName_
                elide: Text.ElideMiddle
                font.pixelSize: 15
                font.weight: Font.Medium
                color: "#333333"
                anchors.left: iconRec.right;
                anchors.leftMargin: 5;
                anchors.top: iconRec.top;
                anchors.right: parent.right;
            }

            Text {
                id:text_filesize;
                text: bytesToHumanReadable(fileSize_)
                font.pixelSize: 12
                color: "#888888"
                anchors.left: text_filename.left;
                anchors.top: text_filename.bottom;
                anchors.topMargin: 5;
                anchors.leftMargin: 2;
            }

            // 状态和时间
            Rectangle {
                anchors.left: text_filesize.left;
                anchors.right: parent.right;
                anchors.top: text_filesize.bottom;
                anchors.topMargin: 5;
                color:"transparent";
                width: stateText.width+stateImage.width+10;
                height: stateText.implicitHeight;
                Image {
                    id:stateImage;
                    source:{
                        if(msgstatus_===msg_fail_)
                            return "/res/warning.svg";
                        else if(msgstatus_===msg_sent_)
                            return "/res/success.svg";
                        else return "";
                    }
                    width: height;
                    height: stateText.implicitHeight;
                    visible: {
                        if(msgstatus_===msg_fail_||msgstatus_===msg_sent_)
                            return true;
                        return false;
                    }
                }

                Text {
                    id:stateText;
                    anchors.left: stateImage.visible?stateImage.right:parent.left;
                    anchors.leftMargin: 5;
                    text: {
                        if(isMyself){
                            if(msgstatus_===msg_sending_)
                                return Math.round(progress_ * 100) + "%"
                            if(msgstatus_===msg_fail_)
                                return "发送失败"
                            if(msgstatus_===msg_sent_)
                                return "发送成功"
                        } else {
                            if(msgstatus_===msg_ready_)
                                return "待接收";
                            if(msgstatus_===msg_fail_)
                                return "接收失败";
                            if(msgstatus_===msg_sending_)
                                return Math.round(progress_ * 100) + "%";
                            if(msgstatus_===msg_sent_)
                                return "接收成功";
                        }
                        return "";
                    }
                    font.pixelSize: 10
                    color: {
                        if(msgstatus_===msg_fail_)return "red";
                        if(msgstatus_===msg_sent_||msgstatus_===msg_ready_)return accentColor;
                    }
                }
                Text {
                    id: downSpeedText;
                    text: "";
                    font.pixelSize: 11;
                    color:accentColor;
                    anchors.bottom: progressBar.top;
                    anchors.right: progressBar.right;
                    visible: progressBar.visible;
                }
                // 进度条
                Rectangle {
                    id:progressBar;
                    height: 4
                    radius: 2
                    color: "#f0f0f0"
                    visible: msgstatus_===msg_sending_;

                    Rectangle {
                        width: parent.width * progress_
                        height: parent.height
                        radius: 2
                        color: accentColor
                        visible: parent.visible;
                    }
                    anchors.left: stateText.right;
                    anchors.right: parent.right;
                    anchors.verticalCenter: stateText.verticalCenter;
                    anchors.leftMargin: 5;
                }
            }
        }
        // 下载/取消按钮
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;
            width: 32
            height: 32
            radius: 16
            color: progress_===1 ? "#f0f0f0" : accentColor
            visible: {
                if(progress_===1)return false;
                if(msgstatus_===msg_sent_||msgstatus_==msg_fail_)
                    return false;
                return true;
            }

            Text {
                id:btnText;
                anchors.centerIn: parent
                text: {
                    if (msgstatus_===msg_sent_) return "✓"
                    if (msgstatus_===msg_sending_) return "×"
                    if(msgstatus_===msg_ready_)return "↓"
                }

                color: progress_===1 ? accentColor : "#ffffff"
                font.pixelSize: 16
                font.bold: true
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if(btnText.text==="↓"){
                        downloadSig();
                    }else if(btnText.text==="×"){
                        cancelSig();
                    }
                }
            }
        }
    }
    function bytesToHumanReadable(bytes) {
        var KB = 1024;
        var MB = 1024 * KB;
        var GB = 1024 * MB;
        var TB = 1024 * GB;

        if (bytes < 0) {
            return "Invalid size";
        } else if (bytes < KB) {
            return bytes + " B";
        } else if (bytes < MB) {
            var size = bytes / KB;
            return size.toFixed(2) + " KB";
        } else if (bytes < GB) {
            var size = bytes / MB;
            return size.toFixed(2) + " MB";
        } else if (bytes < TB) {
            var size = bytes / GB;
            return size.toFixed(2) + " GB";
        } else {
            var size = bytes / TB;
            return size.toFixed(2) + " TB";
        }
    }
    property var lastBytes: 0
    property var lastTime: new Date().getTime()
    property real downloadSpeed: 0 // in bytes per second
    onProgress_Changed: {
        var currentTime = new Date().getTime()
        var currentBytes = progress_*fileSize_ // Assuming 'progress' contains the bytes downloaded
        var timeDifference = (currentTime - lastTime) / 1000 // Convert to seconds

        if (timeDifference > 0) {
            var bytesDownloaded = currentBytes - lastBytes
            downloadSpeed = bytesDownloaded / timeDifference

            // Update for next calculation
            lastBytes = currentBytes
            lastTime = currentTime

            downSpeedText.text=formatSpeed(downloadSpeed);
        }
    }
    function formatSpeed(bytesPerSecond) {
        if (bytesPerSecond < 1024) {
            return bytesPerSecond.toFixed(2) + " B/s"
        } else if (bytesPerSecond < 1024 * 1024) {
            return (bytesPerSecond / 1024).toFixed(2) + " KB/s"
        } else {
            return (bytesPerSecond / (1024 * 1024)).toFixed(2) + " MB/s"
        }
    }
}
