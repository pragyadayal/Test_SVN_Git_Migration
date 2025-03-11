import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   aboutItemId
    height  :   lcd_height
    width   :   lcd_width

    //spacingRelated
    property int topMarginAbtDevice     : 60


    property variant labelNameAlarms :
    (!doorFirmwareType) ?
    [//Direct Door
        dispStr.readDispStr(DispStr.MSG_RTC_FAULT_500)/*"RTC Fault"*/,
        dispStr.readDispStr(DispStr.MSG_TAMPER_600)/*"Tamper"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_ABNORMAL_155)/*"Door Abnormal"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_FORCE_OPEN_163)/*"Door Force Open"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_FAULT_161)/*"Door Fault"*/
    ]:
    [//Panel Door
       dispStr.readDispStr(DispStr.MSG_RTC_FAULT_500)/*"RTC Fault"*/,
       dispStr.readDispStr(DispStr.MSG_TAMPER_600)/*"Tamper"*/,
       dispStr.readDispStr(DispStr.MSG_DOOR_ABNORMAL_155)/*"Door Abnormal"*/,
       dispStr.readDispStr(DispStr.MSG_DOOR_FORCE_OPEN_163)/*"Door Force Open"*/,
       dispStr.readDispStr(DispStr.MSG_DOOR_FAULT_161)/*"Door Fault"*/,
       dispStr.readDispStr(DispStr.MSG_PANIC_424)/*"Panic"*/,
       dispStr.readDispStr(DispStr.MSG_DEAD_MAN_TIMER_EXPIRED_122)/*"Dead Man Timer Expired"*/
    ]

    property variant isExpandableList :
    (!doorFirmwareType) ?
    [//Direct Door
        false,
        false,
        false,
        false,
        false
    ]:
    [//Panel Door
        false,
        false,
        false,
        false,
        false,
        false,
        false
    ]

    property variant labelValueAlarms     : [""]

    OpacityRectangle
    {
        //opacity : 0.90
    }

    AboutDevicePage
    {
        id : alarmsPage
        labelName: labelNameAlarms
        labelValue: labelValueAlarms
        isExpandable: isExpandableList
        title:  dispStr.readDispStr(DispStr.MSG_ALARMS_41)/*"Alarms"*/
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        alarmsPage.fillList()
    }
}
