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

    property bool enrollUserF   : false
    property bool enrollSplFuncF: false
    property bool deleteCredF   : false


    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_ENROLLMENT_196) /*"Enrollment"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_ENROLL_USER,                           //27
            EnumClass.ICON_ENROLL_SPECIAL_CARDS,                  //28
            EnumClass.ICON_DELETE_CREDENTIALS                    //29
        ]

        iconEnableList:
        [
            enrollUserF,
            enrollSplFuncF,
            deleteCredF
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_ENROLL_USER:
                {
                    appData.data = EnumClass.GUI_BUTTON_ENROLL_USER +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ENROLL_SPECIAL_CARDS :
                {
                    appData.data = EnumClass.GUI_BUTTON_ENROLL_SPL_FN +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_DELETE_CREDENTIALS :
                {
                    appData.data = EnumClass.GUI_BUTTON_DEL_CRED +
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
