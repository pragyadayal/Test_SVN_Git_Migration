import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id : userNameEntry
    width   : lcd_width
    height  : lcd_height

    property alias title  : userNameLable.text

    OpacityRectangle
    {

    }

    ButtonElement
    {
        id : buttons
        buttonRowVisibleF: true
        anchors.bottomMargin: 165       // Should not be affected on Keypad Hide
        isInImageFormat:true
        leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath: imageLocation + "/ButtonIcons/ok and set.png"
        //buttonLeftText: "Cancel"
        onButtonLeftPress:
        {
            appData.data = EnumClass.GUI_MSG_ASSIGN_USER_ON_DEVICE_CANCEL +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }

        //buttonRightText: "Ok"
        onButtonRightPress:
        {
            appData.data= EnumClass.MSG_ENTER_USER_NAME_VALIDATION_EVENT +
                    String.fromCharCode(uiMsgFsp).toString() +
                    userName.value +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }

    MxRectangleFrame
    {
        id: userNameEntryect
        anchors.top : parent.top
        anchors.topMargin: topMarginPswPin
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        height: 80
    }
    MxText
    {
        id: userNameLable
        anchors.top: userNameEntryect.top
        anchors.topMargin: 10
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: userNameEntryect.horizontalCenter
        color: "white"
        text:dispStr.readDispStr(DispStr.MSG_ENTER_USER_NAME_891) /*"Enter User Name"*/
        font.pointSize: fontSizeTextBoxHeading
        width : 300
    }

    TextEditBox
    {
        id:userName
        anchors.bottom: userNameEntryect.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: userNameEntryect.horizontalCenter
        isCenterAligned: true
        maxChar: 15
    }

    function processDeviceResponse (msgId,rcvData)
    {
        userName.value = ""
        userName.forceActiveFocus()
        loadAlphaNumKeypad()
    }
}
