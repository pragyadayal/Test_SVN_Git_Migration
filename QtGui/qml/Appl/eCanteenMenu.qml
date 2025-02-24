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
        title : dispStr.readDispStr(DispStr.MSG_CAFETERIA_24) /*"Cafeteria*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_MANAGE_E_CANTEEN,                      //33
            EnumClass.ICON_VIEW_BALANCE                           //34
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case  EnumClass.ICON_MANAGE_E_CANTEEN :
                {
                    appData.data = EnumClass.GUI_BUTTON_MANAGE_CANTEEN_MENU +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                   break
                }
                case  EnumClass.ICON_VIEW_BALANCE :
                {
                    appData.data = EnumClass.GUI_BUTTON_VIEW_BALANCE_MENU +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
