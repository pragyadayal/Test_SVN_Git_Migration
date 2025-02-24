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

    property bool dndActiveDeactive
    property bool dmzActiveDeactive
    property bool dndF
    property bool dmzF

    OpacityRectangle
    {

    }
    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_ZONE_SETTINGS_682) /*"Zone Settings"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            dndActiveDeactive ? EnumClass.ICON_DEACT_DND : EnumClass.ICON_ACT_DND,    //69:68
            dmzActiveDeactive ? EnumClass.ICON_DEACT_DMZ : EnumClass.ICON_ACT_DMZ,    //71:70
            EnumClass.ICON_ZONE_CONTROL,                                            //72
            EnumClass.ICON_DOOR_CONTROL                                             //6
        ]
        iconEnableList:
        [
           dndF,
           dmzF,
           true,
           true
        ]


        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_ACT_DND :
                {
                    appData.data = EnumClass.GUI_BUTTON_ACT_DND +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DEACT_DND :
                {
                    appData.data = EnumClass.GUI_BUTTON_DEACT_DND +
                            String.fromCharCode(uiMsgFsp).toString() +
                             String.fromCharCode(eotFsp).toString()
                }

                case EnumClass.ICON_ACT_DMZ :
                {
                    appData.data = EnumClass.GUI_BUTTON_ACT_DMZ +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_DEACT_DMZ :
                {
                    appData.data = EnumClass.GUI_BUTTON_DEACT_DMZ +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_CONTROL :
                {
                    appData.data = EnumClass.GUI_BUTTON_ZONE_CONTROL +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_DOOR_CONTROL :
                {
                    appData.data = EnumClass.GUI_BUTTON_ADMIN_MENU_DOOR_CONTROL +
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
