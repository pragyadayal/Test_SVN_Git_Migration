import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    height : lcd_height
    width  : lcd_width

    property  bool lateInF         : ( lateInEnable && (doorApplicationType != 2 ) )
    property  bool earlyOutF       : ( earlyOutEnable && (doorApplicationType != 2 ) )
    property bool userProfileF

    OpacityRectangle
    {

    }

    MxMenuScreen
    {
        iconIdxList:(!doorFirmwareType) ?
        [
            EnumClass.ICON_ENROLLMENT,                                                      //7
            lateInStartStop ? EnumClass.ICON_LATE_IN_START : EnumClass.ICON_LATE_IN_STOP,   //8:10
            earlyOutStartStop ? EnumClass.ICON_EARLY_OUT_START : EnumClass.ICON_EARLY_OUT_STOP,  //9:11
            EnumClass.ICON_CHANGE_PASSWORD                                                            //20
        ]:
        [
            EnumClass.ICON_ENROLLMENT,                            //7
            lateInStartStop ? EnumClass.ICON_LATE_IN_START : EnumClass.ICON_LATE_IN_STOP,   //8:10
            earlyOutStartStop ? EnumClass.ICON_EARLY_OUT_START : EnumClass.ICON_EARLY_OUT_STOP,  //9:11
            EnumClass.ICON_USER_PROFILES                         //65
        ]

        iconEnableList:(!doorFirmwareType) ?
        [
            true,
            lateInF,
            earlyOutF,
            true
        ]:
        [
           true,
           lateInF,
           earlyOutF,
           userProfileF
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_ENROLLMENT:
                {
                    appData.data = EnumClass.GUI_BUTTON_ENROLL_MENU +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_LATE_IN_START :
                {
                    appData.data = EnumClass.GUI_BUTTON_LATE_IN_START +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                      break
                }
                case EnumClass.ICON_LATE_IN_STOP :
                {
                    appData.data = EnumClass.GUI_BUTTON_LATE_IN_STOP +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_EARLY_OUT_START :
                {
                    appData.data = EnumClass.GUI_BUTTON_EARLY_OUT_START +
                               String.fromCharCode(uiMsgFsp).toString() +
                               String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_EARLY_OUT_STOP :
                {
                    appData.data = EnumClass.GUI_BUTTON_EARLY_OUT_STOP +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_CHANGE_PASSWORD :
                {
                    appData.data = EnumClass.GUI_BUTTON_CHANGE_PW +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_USER_PROFILES :
                {
                    appData.data = EnumClass.GUI_BUTTON_VIEW_USER_PROFILE +
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
