import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import com.test.AppCore 1.0

AnimatedImage {
    id: bluetoothImg
    property bool isConnection

    function connectionIndicator()
    {
        if(isConnection)
        {
            return ( "qrc:/resources/icon/bluetooth-connected.gif")}
        else
        {

            return ("qrc:/resources/icon/noBluetooth.png");
        }
    }

    width:  33
    height: 33
    source: connectionIndicator()
    fillMode:Image.PreserveAspectFit;

}
