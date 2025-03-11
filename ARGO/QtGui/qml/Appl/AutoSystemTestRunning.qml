import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   autoTest
    height  :   lcd_height
    width   :   lcd_width

    OpacityRectangle
    {

    }

    Column
    {
        id                      : seftEnrollText
        height                  : parent.height
        width                   : parent.width
        spacing                 : 3
        anchors.top             : parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter  : parent.verticalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_SYSTEM_TEST_IS_RUNNING_1012) /*"Do you want to enroll"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }
}
