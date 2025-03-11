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

    property bool zoneLockF
    property bool zoneUnlockF
    property bool zoneNormalF
    property bool zoneArmF
    property bool zoneDisarmF


    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_ZONE_CONTROL_677) /*"Zone Control"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_ZONE_LOCK,                             //73
            EnumClass.ICON_ZONE_UNLOCK,                           //74
            EnumClass.ICON_ZONE_NORMAL,                           //75
            EnumClass.ICON_ZONE_ARM,                              //87
            EnumClass.ICON_ZONE_DISARM                            //88
        ]

        iconEnableList:
        [
            zoneLockF,
            zoneUnlockF,
            zoneNormalF,
            zoneArmF,
            zoneDisarmF
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_ZONE_LOCK :
                {
                    appData.data = EnumClass.GUI_BUTTON_LOCK_ZONE +
                                         String.fromCharCode(uiMsgFsp).toString() +
                                         String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_UNLOCK :
                {
                    appData.data = EnumClass.GUI_BUTTON_UNLOCK_ZONE +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_NORMAL :
                {
                    appData.data = EnumClass.GUI_BUTTON_NORMALIZE_ZONE +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_ARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_ARM_ZONE +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ZONE_DISARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_DISARM_ZONE +
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
