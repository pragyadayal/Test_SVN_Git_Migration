import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: backGround
    width  : lcd_width
    height : lcd_height

    property int dur    : 4

    property variant iconIdxList  : []

    property variant iconEnableList : []

    property bool regularOutEnable:true
    property bool officialOutEnable:true
    property bool preBreakOutEnable:true
    property bool overtimeOutEnable:true
    property bool shortLeaveOutEnable:true
    property bool regularInEnable   :true
    property bool officialInEnable  :true
    property bool postBreakInEnable :true
    property bool shortLeaveInEnable:true
    property bool overtimeInEnable  :true


    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_SELECT_MODE_858) /*"SELECT MODE"*/
    }

    MxMenuScreen
    {
        iconIdxList:allInOutSpecialFunction

        iconEnableList:
        [
            officialInEnable,
            officialOutEnable,
            shortLeaveInEnable,
            shortLeaveOutEnable,
            regularInEnable,
            regularOutEnable,
            postBreakInEnable,
            preBreakOutEnable,
            overtimeInEnable,
            overtimeOutEnable,
        ]
        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_GOING_IN_REGULAR_SPLFUNC:
                {
                    appData.data = EnumClass.GUI_BUTTON_REGULAR_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                   break
                }
                case EnumClass.ICON_GOING_IN_OFFICIAL_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_OFFICIAL_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_POST_BREAK_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_BREAK_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_OVER_TIME_SPLFUNC :
                {

                     appData.data = EnumClass.GUI_BUTTON_OVERTIME_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_IN_SHORT_LEAVE_SPLFUNC :
                {
                    console.log("hi")
                     appData.data = EnumClass.GUI_BUTTON_SHORT_LEAVE_IN +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_GOING_OUT_REGULAR_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_REGULAR_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_OFFICIAL_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_OFFICIAL_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_POST_BREAK_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_BREAK_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_OVER_TIME_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_OVERTIME_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_SHORT_LEAVE_SPLFUNC :
                {
                    appData.data = EnumClass.GUI_BUTTON_SHORT_LEAVE_OUT +
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
