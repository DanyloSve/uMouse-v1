import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.test.AppCore 1.0

Image {
    id: leftSide
    property int size: 50
    width:  size
    height: size
    fillMode:Image.PreserveAspectFit;
}
