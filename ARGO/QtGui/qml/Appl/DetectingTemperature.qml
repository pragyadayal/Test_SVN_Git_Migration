import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item {
    height : lcd_height
    width  : lcd_width

    OpacityRectangle
    {

    }

    MxText
    {
        id: detectingTemperatureText
        text: dispStr.readDispStr(DispStr.DETECTING_TEMPERATURE)
        font.pointSize: fontSizePopUpLabel
        color: "white"
        anchors.centerIn: parent
    }
}
