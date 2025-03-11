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

    property int wifiModulePresentF: 0
    property bool wifiF: ( wifiModulePresentF > 0 ) ? true : false
  
    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_SETTINGS_548) /*"Settings"*/
    }

    MxMenuScreen
    {
        iconIdxList:(!doorFirmwareType) ?
        [//direct door
            EnumClass.ICON_DATE_AND_TIME,                         //14
            EnumClass.ICON_DISPLAY,                               //15
            EnumClass.ICON_SERVER,                                //16
            EnumClass.ICON_LAN,                                   //17
            EnumClass.ICON_WI_FI,                                 //18
            EnumClass.ICON_MODEM,                                 //19
            EnumClass.ICON_PASSWORD,                              //20
            EnumClass.ICON_DOOR_MODE_CHANGE                       //86
        ]:(currentDoorStatus !== EnumClass.DOOR_ONLINE) ?
        [// panel door
            EnumClass.ICON_DISPLAY,                               //15
            EnumClass.ICON_LAN,                                   //17
            EnumClass.ICON_WI_FI,                                 //18
            EnumClass.ICON_DOOR_MODE_CHANGE                       //86
        ]:
        [
            EnumClass.ICON_DISPLAY,                               //15
            EnumClass.ICON_LAN,                                   //17
            EnumClass.ICON_WI_FI,                                 //18
            EnumClass.ICON_ZONE_SETTINGS,                         //66
            EnumClass.ICON_ALARM_OPTIONS,                         //67
            EnumClass.ICON_DOOR_MODE_CHANGE                       //86
        ]

        iconEnableList:(!doorFirmwareType) ?
        [
            true,
            true,
            true,
            true,
            wifiF,                                                 //related to wifi present or not
            true,
            true,
            true
        ]:(currentDoorStatus !== EnumClass.DOOR_ONLINE) ?
        [
           true,
           true,
           wifiF,
           true
        ]:
        [
           true,
           true,
           wifiF,
           true,
           true,
           true
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_DATE_AND_TIME:
                {
                    appData.data = EnumClass.GUI_BUTTON_SET_DATE_TIME +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DISPLAY :
                {
                    appData.data = EnumClass.GUI_BUTTON_DISPALY_CFG +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_SERVER :
                {
                    appData.data = EnumClass.GUI_BUTTON_SET_SERVER_CFG +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_LAN :
                {
                    appData.data = EnumClass.GUI_BUTTON_SET_ETHERNET_CFG +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                      break
                }
                case EnumClass.ICON_WI_FI :
                {
                    appData.data = EnumClass.GUI_BUTTON_SET_WIFI_CFG +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_MODEM :
                {
                    appData.data = EnumClass.GUI_BUTTON_BROADBAND_CFG +
                                         String.fromCharCode(uiMsgFsp).toString() +
                                        String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_PASSWORD :
                {
                    appData.data = EnumClass.GUI_BUTTON_PASSWORD_MENU +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_SETTINGS :
                {
                    appData.data = EnumClass.GUI_BUTTON_ZONE_SETTINGS +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ALARM_OPTIONS :
                {
                    appData.data = EnumClass.GUI_BUTTON_ALARM_OPTIONS +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DOOR_MODE_CHANGE:
                {

                        popUpLoader.loadConfirmDefault(true, 0)
//                      appData.data = EnumClass.GUI_BUTTON_DOOR_MODE_CHANGE +
//                                    String.fromCharCode(uiMsgFsp).toString() +
//                                    String.fromCharCode(eotFsp).toString()
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
