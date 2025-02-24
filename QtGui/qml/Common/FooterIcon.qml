import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id           : outerRect
    height       : 45
    width        : 60
    color        : "transparent"
    opacity      : footerArea.pressed ? 0.5 : 1
    property alias footerSource : footerImage.source
    property alias btnEnable 	: footerArea.enabled

    signal footerPress

    Rectangle
    {
        id                     : innerRect
        height                 : footerImage.height + 2
        width                  : footerImage.width
        anchors.centerIn       : parent
        color                  : "transparent"
        Image
        {
            id : footerImage
            fillMode: Image.PreserveAspectFit
            smooth: true
            height:  footerArea.pressed ? 16 : 22
            width :  footerArea.pressed ? 16 : 22
        }
    }
    MouseArea
    {
        id: footerArea
        anchors.fill: parent
        onPressed:
        {
            footerPress()
        }
    }
}
