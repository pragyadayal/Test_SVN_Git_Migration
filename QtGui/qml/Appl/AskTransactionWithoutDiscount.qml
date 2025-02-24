import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   cafeTransactionWithoutDiscount
    height  :   lcd_height
    width   :   lcd_width
    property int topMarginCafeTransactionWithoutDiscount : 60

    OpacityRectangle
    {

    }

    UserInfoEntryRect
    {
        id : askcafeUsrInfo
        anchors.top : parent.top
        anchors.topMargin: topMarginCafeTransactionWithoutDiscount
        userIdLen :15
    }

    Column
    {
        id                      : cafeUsrText
        height                  : 90
        width                   : 300
        spacing                 : 3
        anchors.top             : askcafeUsrInfo.bottom
        anchors.topMargin       : 15
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_DISCOUNT_WILL_NOT_BE_APPLICABLE_904) /*Discount will Not Be Applicable*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : 15
            smooth                  : true
            color                   : "white"
        }
        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_DO_YOU_WANT_TO_PROCEED_905)	/*Do You Want to Proceed?*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : 15
            smooth                  : true
            color                   : "white"
        }
    }

    Row
    {
        spacing : 5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top             : cafeUsrText.bottom
        anchors.topMargin       : 5

        ButtonWithLabel
        {

            button_Text:dispStr.readDispStr(DispStr.MSG_NO_385)	/*"No"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass. GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT_CANCEL+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
        ButtonWithLabel
        {
            button_Text: dispStr.readDispStr(DispStr.MSG_YES_672) /*"Yes"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass.GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT_START+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
