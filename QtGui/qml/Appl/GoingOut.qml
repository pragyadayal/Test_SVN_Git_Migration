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
    width : lcd_width

    property bool regularOutEnable:false
    property bool officialOutEnable:false
    property bool preBreakOutEnable:false
    property bool overtimeOutEnable:false
    property bool shortLeaveOutEnable:false

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_GOING_OUT_OPTIONS_256)  /*"Going Out Options"*/
    }

    MxTnAScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_GOING_OUT_REGULAR,                      //56
            EnumClass.ICON_GOING_OUT_OVER_TIME,                    //57
            EnumClass.ICON_GOING_OUT_POST_BREAK,                   //58
            EnumClass.ICON_GOING_OUT_OFFICIAL,                     //59
            EnumClass.ICON_GOING_OUT_SHORT_LEAVE                   //60
        ]
        iconEnableList:
        [
            regularOutEnable,
            overtimeOutEnable,
            preBreakOutEnable,
            officialOutEnable,
            shortLeaveOutEnable
        ]
        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_GOING_OUT_REGULAR :
                {
                    appData.data = EnumClass.GUI_BUTTON_REGULAR_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_OFFICIAL :
                {
                    appData.data = EnumClass.GUI_BUTTON_OFFICIAL_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_POST_BREAK :
                {
                    appData.data = EnumClass.GUI_BUTTON_BREAK_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_OVER_TIME :
                {
                    appData.data = EnumClass.GUI_BUTTON_OVERTIME_OUT +
                                           String.fromCharCode(uiMsgFsp).toString() +
                                           String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_GOING_OUT_SHORT_LEAVE :
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
