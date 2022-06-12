import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Item{
    id: controlPanel
    anchors.fill: parent
    property int buttonSize: 70
    property bool isLockedCentralBttn : false
    property int bttnRstState: -1

    Rectangle {
        width: 200
        height: 200
        border.width: 2
        border.color: "#5D8AA8"
        color: "#506699CC"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        radius: 10

        ControlCentralButton {
            id: centralCtrlBttn
            width:  50
            height:  50
            bttnId : 0
            anchors.centerIn: parent
        }

        ControlButton {
            id: upCtrlBttn
            bttnId: 0
            imgPathInactv: "qrc:/resources/icon/arrow-alt-up-inact.svg"
            imgPathActv: "qrc:/resources/icon/arrow-alt-up.svg"
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            width:  buttonSize * 1.5
            height: buttonSize
        }

        ControlButton {
            id: rightCtrlButton
            bttnId: 1
            imgPathInactv: "qrc:/resources/icon/arrow-alt-right-inact.svg"
            imgPathActv: "qrc:/resources/icon/arrow-alt-right.svg"

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            width:  buttonSize * 0.95
            height: buttonSize * 1.5

            anchors.rightMargin: 5
        }

        ControlButton {
            bttnId: 2
            id: downCtrlBttn
            imgPathInactv: "qrc:/resources/icon/arrow-alt-down-inact.svg"
            imgPathActv: "qrc:/resources/icon/arrow-alt-down.svg"

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width:  buttonSize * 1.5
            height: buttonSize
        }

        ControlButton {
            id: leftCtrlButton
            bttnId: 3
            imgPathInactv: "qrc:/resources/icon/arrow-alt-left-inact.svg"
            imgPathActv: "qrc:/resources/icon/arrow-alt-left.svg"

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            width:  buttonSize * 0.95
            height: buttonSize * 1.5

            anchors.leftMargin: 5
        }


    }
}
