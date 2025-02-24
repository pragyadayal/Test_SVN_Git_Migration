import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id : passwordEntry
    width   : lcd_width
    height  : lcd_height

    property alias title  : passwordLable.text
	
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
            appData.data = EnumClass.GUI_BUTTON_HOME +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
            if( currentFeatureAccessed != -1 )
            {
                currentFeatureAccessed = -1
            }
        }

        //buttonRightText: "Ok"
        onButtonRightPress:
        {
            if( currentFeatureAccessed != -1 )
            {
                appData.data= EnumClass.MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        currentFeatureAccessed +
                        String.fromCharCode(uiMsgFsp).toString() +
                        passWord.value +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
            else
            {
                appData.data= EnumClass.MSG_LOGIN_MENU_VALIDATION_EVENT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        passWord.value +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
    }

    MxRectangleFrame
    {
        id: passwordEntryect
        anchors.top : parent.top
        anchors.topMargin: topMarginPswPin
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        height: 80
    }
    MxText
    {
        id: passwordLable
        anchors.top: passwordEntryect.top
        anchors.topMargin: 10
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: passwordEntryect.horizontalCenter
        color: "white"
        text: isAdminHrdF ? dispStr.readDispStr(DispStr.MSG_ENTER_ADMIN_PASSWORD_202)/*Enter Admin Password*/ : dispStr.readDispStr(DispStr.MSG_ENTER_HRD_PASSWORD_204) /*Enter HRD Password*/
        font.pointSize: fontSizeTextBoxHeading
        width : 215
    }

    TextEditBoxPassword
    {
        id:passWord
        anchors.bottom: passwordEntryect.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: passwordEntryect.horizontalCenter
        isCenterAligned: true
        maxChar: (doorFirmwareType ? 4 : 6)    // Validation for Maximum Characters
                                               // 6 --> Direct Door
                                               // 4 --> Panel Door
        checkForSpecialChar : true
        onInvalidCharacterReceived:
        {
           title = dispStr.readDispStr(DispStr.MSG__CHARACTERS_ARE_NOT_SUPPORTED_7) /*"Characters Not Supported: #, /, +"*/
        }
     }

    function processDeviceResponse (msgId,rcvData)
    {
        passWord.value = ""
        passWord.forceActiveFocus()
        loadAlphaNumKeypad()
    }
}
