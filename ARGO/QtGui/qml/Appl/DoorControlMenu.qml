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

    property bool doorLockF    :false
    property bool doorUnlockF  :false
    property bool doorNormalF  :false
    property bool doorArmF     :false
    property bool doorDisarmF  :false

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :  dispStr.readDispStr(DispStr.MSG_DOOR_CONTROL_159) /*"Door Control"*/
    }

    MxMenuScreen
    {
        iconIdxList:(!doorFirmwareType)?
        [
            EnumClass.ICON_DOOR_LOCK,                             //48
            EnumClass.ICON_DOOR_UNLOCK,                           //49
            EnumClass.ICON_DOOR_NORMAL,                           //50
        ]:
        [
            EnumClass.ICON_DOOR_LOCK,                             //48
            EnumClass.ICON_DOOR_UNLOCK,                           //49
            EnumClass.ICON_DOOR_NORMAL,                           //50
            EnumClass.ICON_DOOR_ARM,                              //89
            EnumClass.ICON_DOOR_DISARM,                            //90
        ]

        iconEnableList:
        [
            doorLockF,
            doorUnlockF,
            doorNormalF,
            doorArmF,
            doorDisarmF,
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_DOOR_LOCK :
                {
                    appData.data = EnumClass.GUI_BUTTON_DOOR_LOCK +
                                         String.fromCharCode(uiMsgFsp).toString() +
                                         String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DOOR_UNLOCK :
                {
                    appData.data = EnumClass.GUI_BUTTON_DOOR_UNLOCK +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DOOR_NORMAL :
                {
                    appData.data = EnumClass.GUI_BUTTON_DOOR_NORMAL +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DOOR_ARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_ARM_DOOR +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DOOR_DISARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_DISARM_DOOR +
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
