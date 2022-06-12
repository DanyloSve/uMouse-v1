import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Rectangle {
    property int value : 15
    property int bottomLimit: 0
    property int topLimit: 99

    property int bttnId

    implicitWidth: 40
    implicitHeight: 25
    anchors.leftMargin: 6
    color: "#00000000"


    function pressAndHold()
    {
        if (bttnId == 1)
        {
            if (value < topLimit) {
                value++;
            }
        }else if (bttnId == 2)
        {
            if (value > bottomLimit) {
                value--;
            }
        }

    }

    Timer {
        id: longPressTimer

        interval: 100 //your press-and-hold interval here
        repeat: true
        running: false

        onTriggered: {
            pressAndHold()
        }
    }


    TextInput {
        cursorVisible: false

        validator: IntValidator {bottom: bottomLimit; top: topLimit}

        inputMethodHints: Qt.ImhFormattedNumbersOnly

        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: Number(value)
        font.family: "Serif"
        font.pointSize: 14

        onTextChanged: {
           value = text
        }


    }

    Rectangle{
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 5
        color: "#5D8AA8"
        border.color: "#5D8AA8"
        border.width: 4
        height: 2
    }

    Image {
        id: incrementBttn
        width: 12
        height: 8
        anchors.topMargin: 3
        anchors.left: parent.right
        anchors.top: parent.top
        anchors.bottomMargin: 20
        source: "qrc:/resources/icon/angle-up.svg"

        MouseArea {
            id: incrementBttnMouseArea
            anchors.fill: incrementBttn
            hoverEnabled: true
            preventStealing: true

            onClicked: {
                if (value < topLimit) {
                    value++;
                }
            }

            onPressed: {
                bttnId = 1
                longPressTimer.start()
            }

            onReleased: {
                bttnId = 0
                longPressTimer.stop();
            }
        }
    }

    Image {
        id: decrementBttn
        width: 12
        height: 8
        anchors.bottomMargin: 3
        anchors.left: parent.right
        anchors.bottom: parent.bottom
        visible: true

        source: "qrc:/resources/icon/angle-down.svg"

        MouseArea {
            id: decrementBttnMouseArea
            anchors.fill: decrementBttn
            hoverEnabled: true
            preventStealing: true

            onClicked: {
                if (value > bottomLimit) {
                    value--;
                }
            }

            onPressed: {
                bttnId = 2
                longPressTimer.start()
            }

            onReleased: {
                bttnId = 0
                longPressTimer.stop();
            }

        }
    }




}
