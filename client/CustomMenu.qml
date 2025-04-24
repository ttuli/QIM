import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Menu {
    id: customMenu
    // 修改为适合深色主题的颜色
    property int borderWidth: 1
    property color shadowColor: "#88000000"
    property int backgroundRadius: 8;
    property color highlightColor: "#3d3d3d"       // 悬浮高亮背景色
    spacing: 2
    closePolicy:Menu.CloseOnEscape | Menu.CloseOnPressOutside | Menu.CloseOnReleaseOutside;
    z: 1000
    background: Rectangle {
        id: menuBackground
        implicitWidth: 80
        implicitHeight: 40
        color: "white"
        radius: backgroundRadius;
        layer.enabled: true;
        layer.effect:DropShadow{
            radius: 5;
            samples: 10;
            color:"grey";
        }
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 100 }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
    }
}

