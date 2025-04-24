import QtQuick
import Qt5Compat.GraphicalEffects
import FluentUI

Rectangle{
    color:"#F0F8FF";

    signal closeSig();
    signal flushSig();

    property int btnImaMargin: 5;

    function isInteractiveControl(x1, y1)
    {
        return childAt(x1,y1)!==null
    }

    signal minSig();
    signal maxSig();
    property bool isMax: false;
    property int btnHeight: height-8;

    Rectangle{
        id:btnarea;
        anchors.right: parent.right;
        height: parent.height;
        width: parent.height*3;
        color:"transparent";

        Rectangle{
            id:closebtn;
            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.rightMargin: 5;
            height: btnHeight
            width: height
            color:"transparent";
            radius: 5;


            Image {
                id:closeImage;
                anchors.fill: parent;
                source: "/res/XMark.svg";
            }

            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    closeSig();
                }
                onEntered: {
                    parent.color="red";
                }
                onExited: {
                    parent.color="transparent";
                }
            }
        }//关闭

        Rectangle{
            id:minbtn;
            height: btnHeight;
            width: height;
            anchors.right: closebtn.left;
            anchors.verticalCenter: parent.verticalCenter;
            color:"transparent";
            radius: 5;

            Image {
                id:minImage;
                source: "/res/Minus.svg";
                anchors.fill: parent;
            }
            Rectangle{
                id:hoverRec_min;
                color: "#F5F5F5";
                anchors.fill: parent;
                visible: false;
            }
            OpacityMask{
                id:minHover;
                anchors.fill: parent;
                maskSource: minImage;
                source: hoverRec_min;
                visible: false;
            }

            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    minSig();
                }
                onEntered: {
                    minHover.visible=true;
                    parent.color="grey";
                }
                onExited: {
                    minHover.visible=false;
                    parent.color="transparent";
                }
            }

        }//最小化

        Rectangle{
            id:maxbtn;
            height: btnHeight;
            width: height;
            anchors.right: minbtn.left;
            anchors.verticalCenter: parent.verticalCenter;
            color:"transparent";
            radius: 5;

            Image {
                id:maxImage;
                source: isMax?"/res/nscreen.svg":"/res/fullscreen.svg";
                anchors.fill: parent;
            }
            Rectangle{
                id:hoverRec_max;
                color: "#F5F5F5";
                anchors.fill: parent;
                visible: false;
            }
            OpacityMask{
                id:maxHover;
                anchors.fill: parent;
                maskSource: maxImage;
                source: hoverRec_max;
                visible: false;
            }

            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor
                onClicked:{
                    maxSig();
                }
                onEntered:{
                    maxHover.visible=true;
                    parent.color="grey";
                }
                onExited:{
                    maxHover.visible=false;
                    parent.color="transparent";
                }
            }
        }
    }


}
