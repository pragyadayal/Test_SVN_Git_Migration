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
        title :  dispStr.readDispStr(DispStr.MSG_DISPLAY_SETTINGS_144) /*"Display Settings"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_SET_WALL_PAPER,                        //21
            EnumClass.ICON_SELECT_CLOCK,                          //22
            EnumClass.ICON_SELECT_KEYPAD,                         //23
            EnumClass.ICON_BRIGHTNESS_SETTINGS                    //103
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_SET_WALL_PAPER :
                {
                    appData.data = EnumClass.GUI_BUTTON_BACKGROUND_CFG +
                                        String.fromCharCode(uiMsgFsp).toString() +
                                        String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_SELECT_CLOCK :
                {
                    appData.data = EnumClass.GUI_BUTTON_CLOCK_CFG +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_SELECT_KEYPAD :
                {
                    appData.data = EnumClass.GUI_BUTTON_KEYPAD_CFG +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_BRIGHTNESS_SETTINGS :
                {
                    appData.data = EnumClass.GUI_BUTTON_BRIGHTNESS_CFG +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId,rcvData)
    }
}
