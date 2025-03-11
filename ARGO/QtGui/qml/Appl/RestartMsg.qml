import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    height : lcd_height
    width  : lcd_width
    anchors.centerIn: parent
    anchors.topMargin: 70

    OpacityRectangle
    {

    }

    Column
    {
        height                  : 200
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : 50
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                  : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_DOOR_MODE_CHANGE_168) /*"Door Mode Change"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                : "   "
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline3
            anchors.horizontalCenter: parent.horizontalCenter
            text                : "    "
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline4
            anchors.horizontalCenter: parent.horizontalCenter
            text                :dispStr.readDispStr(DispStr.MSG_DEVICE_RESTARTING_135)/*"Device Restarting...."*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
    }
}

