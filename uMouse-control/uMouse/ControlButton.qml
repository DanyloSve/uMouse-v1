import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Rectangle {
    property string imgPathInactv
    property string imgPathActv
    property int bttnId
    color: "#00000000"


    Image {
        anchors.fill: parent
        source: mouseAreaCntrlBttn.containsMouse ? imgPathActv : imgPathInactv

        MouseArea {
            id: mouseAreaCntrlBttn
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true

            onPressed: {

                AppCore.uiPressedControllButtons(bttnId)

                AppCore.uiChangedSpeed(speedLabelInput.value)
                AppCore.uiChangedTurningSpeed(turningSpeedInput.value)

            }

            onReleased: {
                    AppCore.uiPressedControllButtons(bttnRstState)

            }


        }
    }

}
