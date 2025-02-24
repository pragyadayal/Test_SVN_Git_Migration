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
        title : dispStr.readDispStr(DispStr.MSG_PASSWORD_426) /*Admin "Password"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_CHANGE_PASSWORD,                         //84
            EnumClass.ICON_DEFAULT_HRD_PASSWORD,                    //85
        ]

        iconEnableList:
        [
            true,
            true,
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_CHANGE_PASSWORD:
                {
                    appData.data = EnumClass.GUI_BUTTON_CHANGE_PW +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DEFAULT_HRD_PASSWORD :
                {
                    appData.data = EnumClass.GUI_BUTTON_DEFAULT_HRD_PASSWORD +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }

    function processDeviceResponse(msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
