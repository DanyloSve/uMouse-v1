/*
 8.0 < Vbat         - alarm!
 7.0 < Vbat <= 8.0  - 100%
6.75 < Vbat <= 7.0  - 75%
6.5  < Vbat <= 6.75 - 50%
6.25 < Vbat <= 6.5  - 25%
       Vbat <= 6.25 - alarm!


*/
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import com.test.AppCore 1.0

Item {
    id: batteryIcon
    property color indicatorColor : "#ff0000"

    function drawLevel()
    {

        if (AppCore.battery > 8.0) // 8.0 < Vbat         - alarm!
        {
            indicatorColor = "#E73439"
            return "qrc:/resources/icon/battery-bolt.svg"
        }
        else if (AppCore.battery <= 8.0 && AppCore.battery > 7.1) // 7.0 < Vbat <= 8.0
        {
            indicatorColor = "#019939"
            return "qrc:/resources/icon/battery-full.svg"
        }
        else if (AppCore.battery <=  7.0 && AppCore.battery > 6.75) // 6.75 < Vbat <= 7.0 - 75%
        {
            indicatorColor = "#76B82E"
            return "qrc:/resources/icon/battery-three-quarters.svg"
        }
        else if (AppCore.battery <= 6.75 && AppCore.battery > 6.5) // 6.5  < Vbat <= 6.75 - 50%
        {
            indicatorColor = "#B9C308"
            return "qrc:/resources/icon/battery-half.svg"
        }
        else if (AppCore.battery <= 6.5 && AppCore.battery > 6.0) // 6.25 < Vbat <= 6.5  - 25%
        {
            indicatorColor = "#E0D400"
            return "qrc:/resources/icon/battery-quarter.svg"
        }
        else if (AppCore.battery < 6.0) //  Vbat <= 6.25 - alarm!
        {
            indicatorColor = "#E73439"
            return "qrc:/resources/icon/battery-slash.svg"
        }
        else
        {
            console.log("err")
        }

    }

    Image {
        id: batteryImg
        source: drawLevel()
        width: 30
        height: 30

    }


    Text {
        id: batteryVoltage
        anchors.left: batteryImg.right
        anchors.verticalCenter: batteryImg.verticalCenter
        anchors.margins: 10
        color: indicatorColor
        text: parseFloat((AppCore.battery).toFixed(1))  + "В"
        font.family: "Helvetica"
        font.pointSize: 14

    }


}
