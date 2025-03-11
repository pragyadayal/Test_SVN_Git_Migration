import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : goingInId
    height : lcd_height
    width  : lcd_width

    property bool regularInEnable   :false
    property bool officialInEnable  :false
    property bool postBreakInEnable :false
    property bool shortLeaveInEnable:false
    property bool overtimeInEnable  :false

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_GOING_IN_OPTIONS_254) /*"Going In Options"*/
    }

    MxTnAScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_GOING_IN_REGULAR,
            EnumClass.ICON_GOING_IN_OVER_TIME,
            EnumClass.ICON_GOING_IN_POST_BREAK,
            EnumClass.ICON_GOING_IN_OFFICIAL,
            EnumClass.ICON_GOING_IN_SHORT_LEAVE
        ]

        iconEnableList:
        [
            regularInEnable,
            overtimeInEnable,
            postBreakInEnable,
            officialInEnable,
            shortLeaveInEnable
        ]
        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_GOING_IN_REGULAR:
                {
                    appData.data = EnumClass.GUI_BUTTON_REGULAR_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                   break
                }
                case EnumClass.ICON_GOING_IN_OFFICIAL :
                {
                    appData.data = EnumClass.GUI_BUTTON_OFFICIAL_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_POST_BREAK :
                {
                    appData.data = EnumClass.GUI_BUTTON_BREAK_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_OVER_TIME :
                {

                     appData.data = EnumClass.GUI_BUTTON_OVERTIME_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_SHORT_LEAVE :
                {
                     appData.data = EnumClass.GUI_BUTTON_SHORT_LEAVE_IN +
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
