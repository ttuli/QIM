import QtQuick

Item {
    anchors.fill: parent;
    property string mtext:"";
    Rectangle{
        id:finish_rec;
        anchors.fill: parent;
        color: "#50000000";

        MouseArea{
            anchors.fill: parent;
            acceptedButtons: Qt.NoButton;
        }

        onVisibleChanged: {
            if(visible===false)
                finish_caption_rec.scale=0.7;
            else
                finish_caption_rec.scale=1;
        }

        Rectangle{
            id:finish_caption_rec;
            anchors.centerIn: parent;
            width: 210;
            height: 50;
            color: "#F5F5F5";
            radius: 20;
            scale:0.7;

            Behavior on scale {
                NumberAnimation{
                    duration: 80;
                }
            }

            Image {
                id: finish_image;
                source: "/res/loading.png";
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;
                width: 35;
                height: 35;

                RotationAnimation on rotation {
                    from: 0;
                    to: 360;
                    duration: 1000;
                    loops: Animation.Infinite;
                    running: true;
                }
            }

            Text {
                id:finish_caption;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left:finish_image.right;
                anchors.leftMargin: 5;
                font.bold: true;
                font.pixelSize: 30;
                text: mtext;
            }

        }
    }
}
