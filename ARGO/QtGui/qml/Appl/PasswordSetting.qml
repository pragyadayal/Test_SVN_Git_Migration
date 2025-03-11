import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib
Item
{
    width: lcd_width
    height: lcd_height

    property string currentPassword : ""
    property string tempNewPassword : ""
    property alias enteredValue     : passwordValue.value
    property alias buttonText       : buttons.buttonRightText
    property alias title            : passwordSettingLable.text
    property int currentState       : 0
    property bool backOnHomeF       : false

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id                       : passwordSettingRectId
        width                    : 220
        height                   : 80
        anchors.top              : parent.top
        anchors.topMargin        : topMarginPswPin
        anchors.horizontalCenter : parent.horizontalCenter
    }

    MxText
    {
        id: passwordSettingLable
        anchors.top: passwordSettingRectId.top
        width : 215
     //   wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter

        anchors.topMargin: 10
        anchors.horizontalCenter: passwordSettingRectId.horizontalCenter
        color: "white"
        font.pointSize: fontSizeTextBoxHeading
    }

    TextEditBoxPassword
    {
        id:passwordValue
        anchors.bottom: passwordSettingRectId.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: passwordSettingRectId.horizontalCenter
        isCenterAligned: true
        maxChar: 6
        checkForSpecialChar : true
        onValueChanged:
        {
            if(currentState == 1)
            {
                if(value.length == 0)
                {
                    if( (abtDeviceScreenF == true) && (isAdminHrdF == true) )
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_ADMIN_PASSWORD_927) /*"Choose New Admin Password"*/
                    }
                    else
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_PASSWORD_83) /*"Choose New Password"*/
                    }
                    buttons.buttonRightEnable = false
                }
                else if(value.length >= 6)
                {
                    title = dispStr.readDispStr(DispStr.MSG_MAX_6_CHARACTERS_ALLOWED_339) /*"Max. 6 Characters allowed..!"*/
                    buttons.buttonRightEnable = true
                }
                else
                {
                    buttons.buttonRightEnable = true
                }
           }
            else if(currentState == 2)
            {
                if( (abtDeviceScreenF == true) && (isAdminHrdF == true) )
                {
                    title = dispStr.readDispStr(DispStr.MSG_CONFIRM_NEW_ADMIN_PASSWORD_928) /*"Confirm New Admin Password"*/
                }
                else
                {
                    title = dispStr.readDispStr(DispStr.MSG_CONFIRM_NEW_PASSWORD_95) /*"Confirm New Password"*/
                }
            }
        }

        onInvalidCharacterReceived:
        {
            title = dispStr.readDispStr(DispStr.MSG__CHARACTERS_ARE_NOT_SUPPORTED_7) /*"Characters Not Supported: #, /, +"*/
        }
    }

    ButtonElement
    {
        id : buttons
        anchors.bottomMargin: 165       // Should not be affected on Keypad Hide
        buttonRowVisibleF: true
		isInImageFormat:true
        leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath: imageLocation + "/ButtonIcons/continue.png"
        onButtonLeftPress:
        {
            if(backOnHomeF == false)
            {
                appData.data = EnumClass.GUI_BUTTON_BACK +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }
            else
            {
                appData.data = EnumClass.GUI_BUTTON_HOME +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }

            if(currentFeatureAccessed != -1)
            {
                currentFeatureAccessed = -1
            }
        }

        onButtonRightPress:
        {
            okButtenPressed()
        }
    }

    DynamicValidationResp
    {
       id : responseData
    }

    function okButtenPressed()
    {
        switch(currentState)
        {
            case 0 :
            {
                if(enteredValue == currentPassword)
                {
                    currentState = 1
                    resetPara()

                    if( (abtDeviceScreenF == true) && (isAdminHrdF == true) )
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_ADMIN_PASSWORD_927) /*"Choose New Admin Password"*/
                    }
                    else
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_PASSWORD_83) /*"Choose New Password"*/
                    }

                    //buttonText = "Continue"
                    buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/continue.png"
                    passwordValue.checkForSpecialChar = true
                }
                else
                {
                    resetPara()
                    title = dispStr.readDispStr(DispStr.MSG_INCORRECT_CURRENT_PASSWORD_TRY_AGAIN_273)  /*"Incorrect 'Current Password'\nTry Again!"*/
                }
                break
            }

            case 1 :
            {
                if(enteredValue.length < 4)
                {
                    currentState = 1
                    resetPara()
                    title = dispStr.readDispStr(DispStr.MSG_MINIMUM_4_CHARACTERS_REQUIRED_864) /*"Min. 4 Characters required..!"*/
                     buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/continue.png"
                }
                else
                {
                    currentState = 2
                    tempNewPassword = enteredValue
                    resetPara()
                    if( (abtDeviceScreenF == true) && (isAdminHrdF == true) )
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CONFIRM_NEW_ADMIN_PASSWORD_928) /*"Confirm New Admin Password"*/
                    }
                    else
                    {
                        title = dispStr.readDispStr(DispStr.MSG_CONFIRM_NEW_PASSWORD_95) /*"Confirm New Password"*/
                    }
                    //buttonText = "Ok"
                    buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/ok and set.png"
                }
                break
            }

            case 2 :
            {
                if(tempNewPassword == enteredValue)
                {
                    if(isAdminHrdF == true)
                    {
                        appData.data = EnumClass.MSG_ADMIN_PW_VALIDATION_EVENT +
                                   String.fromCharCode(uiMsgFsp).toString()+
                                   currentPassword + String.fromCharCode(uiMsgFsp).toString() +
                                   tempNewPassword+ String.fromCharCode(uiMsgFsp).toString() +
                                   enteredValue + String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()

                        if(currentFeatureAccessed != -1)
                        {
                            appData.data= EnumClass.MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    currentFeatureAccessed +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    tempNewPassword +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()
                        }
                    }
                    else
                    {
                        appData.data = EnumClass.MSG_HRD_PW_VALIDATION_EVENT +
                                   String.fromCharCode(uiMsgFsp).toString()+
                                   currentPassword + String.fromCharCode(uiMsgFsp).toString() +
                                   tempNewPassword+ String.fromCharCode(uiMsgFsp).toString() +
                                   enteredValue + String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    }
                }
                else
                {
                    currentState = 1
                    resetPara()
                    tempNewPassword = ""
                    title = dispStr.readDispStr(DispStr.MSG_NEW_CONFIRM_PASSWORDS_DON_T_MATCH_ENTER_NEW_PASSWORD_1028)//New & Confirm Passwords don’t match, Enter New Password!
                    //buttonText = "Continue"
                    buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/continue.png"
                }
                break
            }
        }
    }

    function resetPara()
    {
        enteredValue = ""
        passwordValue.forceActiveFocus()
        loadAlphaNumKeypad()
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)

        if(currentFeatureAccessed != -1)
        {
            abtDeviceScreenF = true
            isAdminHrdF = true
        }

        resetPara()
        if(currentPassword.length > 0)
        {
            title = dispStr.readDispStr(DispStr.MSG_CONFIRM_CURRENT_PASSWORD_94) /*"Confirm Current Password"*/
            //buttonText = "Ok"
            buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/ok and set.png"
        }
        else
        {
            currentState = 1
            if( (abtDeviceScreenF == true) && (isAdminHrdF == true) )
            {
                title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_ADMIN_PASSWORD_927) /*"Choose New Admin Password"*/
            }
            else
            {
                title = dispStr.readDispStr(DispStr.MSG_CHOOSE_NEW_PASSWORD_83) /*"Choose New Password"*/
            }

            //buttonText = "Continue"
            buttons.rightBtnImagePath = imageLocation + "/ButtonIcons/continue.png"
            backOnHomeF = true
            buttons.buttonRightEnable = false
            homeVisibleF = false
            backVisibleF = false
        }
    }
}
