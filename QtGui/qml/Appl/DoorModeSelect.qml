import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    height : lcd_height
    width  : lcd_width

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_SELECT_MODE_858) /*Select Mode*/
    }

    MxMenuScreen
    {
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter

        maxIconPerRow : 2

        iconIdxList:
        [
            EnumClass.ICON_DOOR_MODE_IN,
            EnumClass.ICON_DOOR_MODE_OUT
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_DOOR_MODE_IN:
                {

                    appData.data = EnumClass.GUI_BUTTON_DOOR_MODE_IN +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_DOOR_MODE_OUT:
                {
                    appData.data = EnumClass.GUI_BUTTON_DOOR_MODE_OUT +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }
}
