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

    property variant labelNameIdle :
    [
        dispStr.readDispStr(DispStr.MSG_VERSION_REVISION_640)  /*"Version"*/,
        dispStr.readDispStr(DispStr.MSG_MAC_ADDRESS_330)       /*"MAC Address"*/,
        dispStr.readDispStr(DispStr.MSG_CONNECTIVITY_101)           /*"Connectivity"*/,
        dispStr.readDispStr(DispStr.MSG_READERS_INTERFACES_486)  /*"Readers & Interfaces"*/,
        dispStr.readDispStr(DispStr.MSG_ALARMS_41)              /*"Alarms"*/,
    ]

    property variant isExpandableList :
    [
        false,
        false,
        true,
        true,
        true
    ]

    property variant labelValueIdle     : [""]
    //spacingRelated
    property int topMarginAbtDevice     : 60

    OpacityRectangle
    {
        //opacity : 0.90
    }

    AboutDevicePage
    {
        id : idlePage
        labelName: labelNameIdle
        labelValue: labelValueIdle
        isExpandable: isExpandableList
        title: dispStr.readDispStr(DispStr.MSG_ABOUT_DEVICE_20) /*"About Device*/

        onElementClicked:
        {
            if(label == labelNameIdle[2])
            {

                appData.data = EnumClass.GUI_BUTTON_ABOUT_DEVICE_CONNECTIVITY +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()

            }
            else if (label == labelNameIdle[3])
            {

                appData.data = EnumClass.GUI_BUTTON_ABOUT_DEVICE_READERS_INTERFACES +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()

            }
            else if (label == labelNameIdle[4])
            {

                 appData.data = EnumClass.GUI_BUTTON_ABOUT_DEVICE_ALARMS +
                                     String.fromCharCode(uiMsgFsp).toString() +
                                     String.fromCharCode(eotFsp).toString()

            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        idlePage.fillList()
    }
}
