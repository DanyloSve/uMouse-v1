import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Item {
    id: rightpannel
    width: 300
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.topMargin: width * 0.2
    anchors.bottomMargin: width * 0.2
    property int defaultmargin: 20

    Rectangle {
        anchors.fill: parent
        color: "#00000000" // form: #AARRGGBB
        //color: "red"

        Rectangle {
            width: 200
            height: 300
            border.width: 2
            border.color: "#5D8AA8"
            color: "#506699CC"
            anchors.top: parent.top
            anchors.left: parent.left
            radius: 10
            y: 50



            Rectangle {
                id : speedLabel

                //border.width: 2
                //border.color: "black"
                anchors.topMargin: 20

                anchors.left: parent.left
                anchors.top : parent.top
                anchors.leftMargin: defaultmargin
                width: 100
                height: 60
                color: "#00000000"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Швидкість"
                    font.family: "Serif"
                    font.pointSize: 14
                }
            }

            CustomSpinBox {
                id: speedLabelInput
                value : 15
                anchors.left: speedLabel.right
                anchors.verticalCenter: speedLabel.verticalCenter
            }


            Rectangle {
                id : turningSpeedLabel
                anchors.top: speedLabel.bottom
                anchors.left: parent.left
                anchors.leftMargin: defaultmargin

                //border.width: 2
                //border.color: "black"

                width: 100
                height: 60
                color: "#00000000"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 20
                    text: "Швидкість\nповороту"
                    font.family: "Serif"
                    font.pointSize: 14
                }
            }

            CustomSpinBox {
                id: turningSpeedInput
                value : 7
                anchors.left: turningSpeedLabel.right
                anchors.verticalCenter: turningSpeedLabel.verticalCenter
            }

            Rectangle {
                id : zeroingAngle
                radius: 10
                border.width: mouseAreaZeroingAngle.containsMouse ? 4 : 2
                border.color: mouseAreaZeroingAngle.containsMouse ? "black" : "#5D8AA8"

                anchors.left: parent.left
                anchors.top : turningSpeedLabel.bottom
                anchors.leftMargin: defaultmargin
                width: parent.width - defaultmargin * 2
                height: 60
                color: "#00000000"


                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.leftMargin: defaultmargin
                    text: "Скинути кут"

                    font.family: "Serif"
                    font.pointSize: 14
                }


                MouseArea {
                    id: mouseAreaZeroingAngle
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true

                    onPressed: {
                        onPressed: {
                            AppCore.uiPressedZeroingAngle(1)
                        }
                    }
                }
            }

            Rectangle {
                id : zeroingLength

                border.width: mouseAreaZeroingLength.containsMouse ? 4 : 2
                border.color: mouseAreaZeroingLength.containsMouse ? "black" : "#5D8AA8"
                radius: 10
                anchors.topMargin: 10

                anchors.left: parent.left
                anchors.top : zeroingAngle.bottom
                anchors.leftMargin: defaultmargin
                width: parent.width - defaultmargin * 2
                height: 60
                color: "#00000000"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.leftMargin: defaultmargin
                    text: "Скинути відстань"
                    font.family: "Serif"
                    font.pointSize: 14
                }

                MouseArea {
                    id: mouseAreaZeroingLength
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true

                    onPressed: {
                        AppCore.uiPressedZeroingDistance(1)

                    }
                }
            }


        }

        ControlPanel {
            id: controlPanel
        }
    }
}
