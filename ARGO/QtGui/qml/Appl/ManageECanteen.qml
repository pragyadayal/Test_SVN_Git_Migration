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

    property bool printLastBillF   : false
    property bool itemSoldOutF     : false
    property bool itemAvailableF   : false

    OpacityRectangle
    {

    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_PRINT_LAST_BILL,                       //35
            EnumClass.ICON_ITEM_SOLD_OUT,                         //36
            EnumClass.ICON_ITEM_AVAILABLE                        //37
        ]

        iconEnableList:
        [
            printLastBillF,
            itemSoldOutF,
            itemAvailableF
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_PRINT_LAST_BILL :
                {
                    appData.data = EnumClass.GUI_BUTTON_PRINT_LAST_BILL_MENU +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ITEM_SOLD_OUT :
                {
                    appData.data = EnumClass.GUI_BUTTON_CAFE_SOLDOUT_ITEM +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_ITEM_AVAILABLE :
                {
                    appData.data = EnumClass.GUI_BUTTON_CAFE_AVAILABLE_ITEM +
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
