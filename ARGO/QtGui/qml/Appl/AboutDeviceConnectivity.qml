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


    property variant labelNameConnectivity : (!doorFirmwareType) ?
    [//Direct Door
        dispStr.readDispStr(DispStr.MSG_LAN_IP_ADDRESS_310)             /*"LAN IP Address"*/,
        dispStr.readDispStr(DispStr.MSG_SERVER_IP_ADDRESS_528)          /*"Server IP Address"*/,
        dispStr.readDispStr(DispStr.MSG_SERVER_CONNECTIVITY_526)        /*"Server Connectivity"*/,
        dispStr.readDispStr(DispStr.MSG_WI_FI_658)                /*Status "Wi-Fi"*/,
        dispStr.readDispStr(DispStr.MSG_MOBILE_BROADBAND_349)       /*"Mobile Broadband"*/
    ]:
    [//Panel Door
        dispStr.readDispStr(DispStr.MSG_LAN_IP_ADDRESS_310)             /*"LAN IP Address"*/,
        dispStr.readDispStr(DispStr.MSG_PANEL_IP_ADDRESS_423)           /*"Panel IP Address"*/,
        dispStr.readDispStr(DispStr.MSG_WI_FI_658)                /*status "Wi-Fi"*/,
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
        false
    ]

    property variant labelValueConnectivity : [""]

    OpacityRectangle
    {
        //opacity : 0.90
    }

    AboutDevicePage
    {
        id : connectivityPage
        labelName: labelNameConnectivity
        labelValue: labelValueConnectivity
        isExpandable: isExpandableList
        title: dispStr.readDispStr(DispStr.MSG_CONNECTIVITY_101)/*"Connectivity"*/
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        connectivityPage.fillList()
    }
}
