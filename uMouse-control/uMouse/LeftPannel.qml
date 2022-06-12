import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import com.test.AppCore 1.0

Item {
    id: leftpannel
    width: 300
    height: parent.height


    Rectangle {
        anchors.fill: parent
        color: "#00000000" // form: #AARRGGBB
        //color: "blue"



        Rectangle {
            id: leftPanelDisplay
            width: 225
            height: 200
            border.width: 2
            border.color: "#5D8AA8"
            color: "#506699CC"
            anchors.verticalCenter: parent.verticalCenter
            radius: 10
            y: 50

            ColumnLayout{
                spacing: 20
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.margins: 25

                Text {
                    text: "Кут\t" + "   " + Number(AppCore.angle) + "°"
                    font.family: "Serif"
                    font.pointSize: 14
                }

                Text {
                    text: "Швидкість " + ((AppCore.motorSpeed).toFixed(3)) + " м/с"
                    font.family: "Serif"
                    font.pointSize: 14
                }

                Text {
                    text: "Відстань\t"+"   " + ((AppCore.distance).toFixed(2)) + " м"
                    font.pointSize: 14
                }
            }


        }

        BluetoothIcon {
            id: bluetoothImg
            isConnection : true
            anchors.bottom: leftPanelDisplay.top
            anchors.right: leftPanelDisplay.right
        }


        BatteryIcon {
            id: batteryIcon
            anchors.bottom: leftPanelDisplay.top
            anchors.right: leftPanelDisplay.left
            anchors.bottomMargin: 30
        }

    }
}
