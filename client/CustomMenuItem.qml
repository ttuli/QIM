import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Controls

MenuItem{
    property string mtext:"";
    property int itemRadius_top:0;
    property int itemRadius_bottom: 0;
    contentItem:Text {
        text: mtext;
        font.pixelSize: 13
        anchors.centerIn: parent;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        color: "black"
    }
    background: Rectangle{
        color:"white";
        anchors.fill: parent;
        topRightRadius: itemRadius_top;
        topLeftRadius: itemRadius_top;
        bottomLeftRadius: itemRadius_bottom;
        bottomRightRadius: itemRadius_bottom;
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="grey";
            }
            onExited: {
                parent.color="white";
            }
        }
    }
}
