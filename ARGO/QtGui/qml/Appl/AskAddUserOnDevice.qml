import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   addUser
    height  :   lcd_height
    width   :   lcd_width
    property int topMarginAddUserOnDevice : 60
    property string askAddUsrInfoUserId : ""

    OpacityRectangle
    {

    }

    Column
    {
        id                      : addUsrText
        height                  : 100
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : topMarginAddUserOnDevice
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_USER_ID_626)+" :"+askAddUsrInfoUserId    /*"User ID"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : 15
            smooth                  : true
            color                   : "white"
        }

        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_NOT_FOUND_888) /*"not found."*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }

        MxText
        {
            id                      : confirmDefltline3
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_DO_YOU_WANT_TO_ADD_NEW_889) /*"Do you want to add new"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }

        MxText
        {
            id                      : confirmDefltline4
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_USER_WITH_THIS_ID_890)	/*"User with this ID?"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }

    Row
    {
        spacing : 5
        anchors.top        : addUsrText.bottom
        anchors.topMargin       : topMarginAddUserOnDevice
        anchors.horizontalCenter: parent.horizontalCenter


        ButtonWithLabel
        {
            button_Text:dispStr.readDispStr(DispStr.MSG_NO_385)	/*"No"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass. GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_CANCEL+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
        ButtonWithLabel
        {
            button_Text: dispStr.readDispStr(DispStr.MSG_YES_672) /*"Yes"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass.GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_START +
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
