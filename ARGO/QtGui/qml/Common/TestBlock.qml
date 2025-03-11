import QtQuick 1.1
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Rectangle
{
    id : testBlockRect
    width: 30
    height: 30
    color : touchArea.pressed ? "white" : mxColorRed
    signal blockPress
    MouseArea
    {
        id : touchArea
        anchors.fill: parent
        onClicked:
        {
            sendOnlyKeepAlive()
            blockPress()
        }
    }
}
