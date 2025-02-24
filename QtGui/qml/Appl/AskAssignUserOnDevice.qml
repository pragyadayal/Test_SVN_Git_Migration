import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   assignUser
    height  :   lcd_height
    width   :   lcd_width
    property int topMarginAssignUserOnDevice : 60

    OpacityRectangle
    {

    }

    UserInfoEntryRect
    {
        id : askAssignUsrInfo
        anchors.top : parent.top
        anchors.topMargin: topMarginAssignUserOnDevice
        userIdLen :15
    }

    Column
    {
        id                      : assignUsrText
        height                  : 100
        width                   : 300
        spacing                 : 3
        anchors.top             : askAssignUsrInfo.bottom
        anchors.topMargin       : 5
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_USER_AVAILABLE_ON_SERVER_885) /*"User available on Server."*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_ADD_USER_TO_DEVICE_886)	/*"Add User to Device?"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }

    Row
    {
        spacing : 5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top             : assignUsrText.bottom
        anchors.topMargin       : 5

        ButtonWithLabel
        {

            button_Text:dispStr.readDispStr(DispStr.MSG_NO_385)	/*"No"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass. GUI_MSG_ASSIGN_USER_ON_DEVICE_CANCEL+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
        ButtonWithLabel
        {
            button_Text: dispStr.readDispStr(DispStr.MSG_YES_672) /*"Yes"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass.GUI_MSG_ASSIGN_USER_ON_DEVICE_START +
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
