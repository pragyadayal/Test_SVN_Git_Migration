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


    property variant labelNameReaders : internalFpSupport ?
    [//YES

        dispStr.readDispStr(DispStr.MSG_CARD_READER_78)                /*"Card Reader"*/,
        dispStr.readDispStr(DispStr.MSG_FINGER_READER_241)                  /*"Finger Reader"*/,
        dispStr.readDispStr(DispStr.MSG_EXTERNAL_CARD_READER_231)           /*"External Card Reader"*/,
        dispStr.readDispStr(DispStr.MSG_EXTERNAL_FINGER_READER_232)         /*"External Finger Reader"*/,
        dispStr.readDispStr(DispStr.MSG_MEMORY_CARD_342)                    /*"Memory Card"*/,
        dispStr.readDispStr(DispStr.MSG_EXIT_SWITCH_227)      /*"Exit Switch"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_STATUS_174)                /*"Door Status"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_SENSE_171)       /*"Door Sense"*/
    ]:
    [//NO

        dispStr.readDispStr(DispStr.MSG_CARD_READER_78)                /*"Card Reader"*/,
        dispStr.readDispStr(DispStr.MSG_EXTERNAL_CARD_READER_231)           /*"External Card Reader"*/,
        dispStr.readDispStr(DispStr.MSG_EXTERNAL_FINGER_READER_232)         /*"External Finger Reader"*/,
        dispStr.readDispStr(DispStr.MSG_MEMORY_CARD_342)                    /*"Memory Card"*/,
        dispStr.readDispStr(DispStr.MSG_EXIT_SWITCH_227)      /*"Exit Switch"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_STATUS_174)                /*"Door Status"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_SENSE_171)       /*"Door Sense"*/
    ]

    property variant isExpandableList : internalFpSupport ?
    [//YES
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false
    ]:
    [//NO
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false
    ]

    property variant labelValueReaders      : [""]

    OpacityRectangle
    {
        //opacity : 0.90
    }

    AboutDevicePage
    {
        id : readersPage
        labelName: labelNameReaders
        labelValue: labelValueReaders
        isExpandable: isExpandableList
        title: dispStr.readDispStr(DispStr.MSG_READERS_INTERFACES_486)  /*"Readers & Interfaces"*/
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        readersPage.fillList()
    }
}
