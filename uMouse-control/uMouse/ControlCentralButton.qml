import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Rectangle {
    id: centralCtrlBttn
    property int bttnId
    width:  buttonSize
    height: buttonSize
    border.width: 5
    radius: 15
    color: isLockedCentralBttn ? "#5D8AA8" : "#00000000"
    border.color: mouseAreaCentrlCtrlBttn.containsMouse ? "black" : "#5D8AA8"

    MouseArea {
        id: mouseAreaCentrlCtrlBttn
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            if (isLockedCentralBttn)
            {
                AppCore.uiPressedControllButtons(bttnRstState)
            }

            isLockedCentralBttn = !isLockedCentralBttn
            AppCore.uiChangedTurningMode(isLockedCentralBttn)
        }
    }

}
