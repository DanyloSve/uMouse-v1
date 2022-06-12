import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Item {
    id: centralpannel
    width:  540
    height: parent.height
    anchors.horizontalCenter:parent.horizontalCenter
    anchors.top: parent.top


    Rectangle {
        anchors.fill: parent
        //color: "green"

        Rectangle {
            id : imageContainer
            rotation: (AppCore.angle * -1)
            width:540
            height:540
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            ImgSensors {
                id: leftSide
                source: AppCore.sensorLS ? "qrc:/resources/img/on-left-side.png" : "qrc:/resources/img/off-left-side.png"
                x: 87
                y: 130
            }

            ImgSensors {
                id: leftFront
                source: AppCore.sensorLF ? "qrc:/resources/img/on-left-front.png" : "qrc:/resources/img/off-left-front.png"
                x: 160
                y: 20
            }

            ImgSensors {
                id: rightFront
                source:  AppCore.sensorRF ? "qrc:/resources/img/on-right-front.png" : "qrc:/resources/img/off-right-front.png"
                x: 327
                y: 20
            }

            ImgSensors {
                id: rightSide
                source: AppCore.sensorRS ? "qrc:/resources/img/on-right-side.png" : "qrc:/resources/img/off-right-side.png"
                x: 403
                y: 130
            }


            Image {
                id: imgBoard
                source: "qrc:/resources/img/board.png"
                width:540
                height:540
                fillMode:Image.PreserveAspectFit;
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
