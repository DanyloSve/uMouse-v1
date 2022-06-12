import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Window {
    width: 1280
    height: 640
    visible: true
    title: qsTr("uMouse Control")

    LeftPannel {
        id: leftpannel
        z: 0
        anchors.right: centralpannel.left
        anchors.top: parent.top
        anchors.leftMargin: width * 0.2
    }

    RightPannel {
        id: rightpannel
        z: 0
        anchors.left: centralpannel.right
    }

    CentralPanel {
        id: centralpannel
        z: -1
    }

}
