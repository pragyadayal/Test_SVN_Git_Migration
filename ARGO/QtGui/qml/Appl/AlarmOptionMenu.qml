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

    property bool panicAlarmF
    property bool muteDoorBuzzerF
    property bool clearDoorAuxOPF
    property bool muteMasterBuzzerF
    property bool clearMasterOPF
    property bool clearDcAllAlarmF

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_ALARM_OPTIONS_39)         /*"Alarm Options"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_SET_PANIC_ALARM,
            EnumClass.ICON_MUTE_DOOR_BUZZER,
            EnumClass.ICON_CLEAR_DOOR_AUX_OP,
            EnumClass.ICON_MUTE_MASTER_BUZZEER,
            EnumClass.ICON_CLEAR_MASTER_OP,
            EnumClass.ICON_CLEAR_ALARM,
        ]

        iconEnableList:
        [
            panicAlarmF,
            muteDoorBuzzerF,
            clearDoorAuxOPF,
            muteMasterBuzzerF,
            clearMasterOPF,
            clearDcAllAlarmF,
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_SET_PANIC_ALARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_SET_PANIC_ALARM +
                                         String.fromCharCode(uiMsgFsp).toString() +
                                         String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_MUTE_DOOR_BUZZER :
                {
                    appData.data = EnumClass.GUI_BUTTON_MUTE_BUZZER +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_CLEAR_DOOR_AUX_OP :
                {
                    appData.data = EnumClass.GUI_BUTTON_CLEAR_DOOR_AUX_OP +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_MUTE_MASTER_BUZZEER :
                {
                    appData.data = EnumClass.GUI_BUTTON_MUTE_MASTER_BUZZER +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_CLEAR_MASTER_OP :
                {
                    appData.data = EnumClass.GUI_BUTTON_CLEAR_MASTER_OP +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_CLEAR_ALARM :
                {
                    appData.data = EnumClass.GUI_BUTTON_CLEAR_ALARM +
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
