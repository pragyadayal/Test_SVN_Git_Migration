import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id     : brightnessSettingsScreen
    height : lcd_height
    width  : lcd_width

    property int minValueOfSetBrightness     : 30
    property int maxValueOfSetBrightness     : 999
    property int prevScreenSleepTimerValue   : 00
    property int prevActiveScreenSpinBoxValue: 00
    property int spinBoxBottomValidation     : 02
    property int spinBoxTopValidation        : 10

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :  dispStr.readDispStr(DispStr.MSG_SET_BRIGHTNESS_963) /*"Set Brightness"*/
    }

    MxRectangleFrame
    {
        id                          : brightnessRectId
        height                      : (alphaNumKeypadF) ? (lcd_height - anchors.topMargin - 165 - buttons.height) : (lcd_height - anchors.topMargin) // 165 - buttonsBottomMargin
        width                       : lcd_width
        anchors.top                 : parent.top
        anchors.topMargin           : topMarginSettingScreen + 10 // 65 + additionalMargin
        anchors.horizontalCenter    : parent.horizontalCenter
        ScrollBar
        {
            noOfElements        : 2 // 2 Rows
            visibleElements     : (alphaNumKeypadF ? 1 : 3)
            position            : flickableArea.visibleArea.yPosition
        }
    }

    Flickable
    {
        id                      : flickableArea
        height                  : brightnessRectId.height
        width                   : brightnessRectId.width
        anchors.centerIn        : brightnessRectId

        contentHeight           : row1.height + activeScreenSpinBox.height + 26 // 26 - additionalHeight
        contentWidth            : row1.width

        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
             sendOnlyKeepAlive()
        }

        Item
        {
              id    : brightnessControlItemId

            Row
            {
                id                  : row1
                spacing             : spacingBtwSettingLabel
                anchors.top         : parent.top
                anchors.topMargin   : 5
                anchors.left        : parent.left

                TextLabel
                {
                    label               : dispStr.readDispStr(DispStr.MSG_SLEEP_TIMER_964) /*"Sleep Timer"*/
                }
                TextEditBox
                {
                    id          : sleepTimer
                    isEnable    : true
                    validation: RegExpValidator {
                        regExp: /[0-9]|[0-9][0-9]|[0-9][0-9][0-9]/
                        //regExp: /^[3-9][0-9]$|^[0-2][0-9]$|^300$/
                    }
                    onActivated :
                    {
                        flickableArea.contentY = setVisibleArea(1)
                    }
                }
            }

            Row
            {
                id                      :   row2
                anchors.top             :   row1.top
                anchors.topMargin       :   row1.height + 15 + 22 + 25 // additionaGap + plusRect.height(22) + half_NameRect.height(50/2)
                anchors.left            :   parent.left
                anchors.horizontalCenter:   parent.horizontalCenter

                TextLabel
                {
                    label               : dispStr.readDispStr(DispStr.MSG_BRIGHTNESS_965) /*"Brightness"*/
                }

                SpinBoxSmall
                {
                    id                  : activeScreenSpinBox
                    bottomValidation    : spinBoxBottomValidation
                    topValidation       : spinBoxTopValidation
                    noOfDigit           : 2
                    x                   : sleepTimer.x
                    y                   : -37// 37 units upwards from initial position

                    onSpinBoxClicked:
                    {
                        setBrightnessPayload()
                    }
                }
            }
        }
    }

    DynamicValidationResp
    {
        id : responseData
    }

    Connections
    {
        target      : alphaNumKeypadId
        onVkpressed :
        {
            responseData.validationF = false
        }
    }

    ButtonElement
    {
        id                  :   buttons
        isInImageFormat     :   true
        leftBtnImagePath    :   imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath   :   imageLocation + "/ButtonIcons/ok and set.png"
        buttonRowVisibleF   :   true
        y                   :   row2.y + 45 // 45 - Half_NameRect.height(50/2) + additionalGap(20)

        //buttonLeftText: "Cancel"
        onButtonLeftPress:
        {
            sleepTimer.value = prevScreenSleepTimerValue.toString()
            activeScreenSpinBox.value = prevActiveScreenSpinBoxValue.toString()
            generateSetBrightnessPayload()
        }

        //buttonRightText: "Set"
        onButtonRightPress:
        {
            generateSetBrightnessPayload()
        }
    }


    function processDeviceResponse (msgId,rcvData)
    {
        switch(msgId)
        {
            case EnumClass.GUI_MSG_BRIGHTNESS_SELECTION_SCREEN:
            {
                var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
                sleepTimer.value = tmpPayload[0]
                prevScreenSleepTimerValue =   parseInt(sleepTimer.value)
                activeScreenSpinBox.value = tmpPayload[1]
                prevActiveScreenSpinBoxValue = parseInt(tmpPayload[1])
                break
            }
            default:break
         }
    }

    function setBrightnessPayload()
    {
        appData.data   =    EnumClass.GUI_MSG_BRIGHTNEES_SET_FROM_SPIN_BOX +
                            String.fromCharCode(uiMsgFsp).toString() +
                            sleepTimer.value +
                            String.fromCharCode(uiMsgFsp).toString() +
                            activeScreenSpinBox.value+
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
    }

    function generateSetBrightnessPayload()
    {
        var tempSleepTimerValue
        var tempActiveScreenSpinBoxValue
        var tempSleepTimerData
        var validF = true

        // Validations (Mandatory Fields)
        if(sleepTimer.value.length <= 0)
        {
            validF = false
            responseData.responseIndex = EnumClass.VALIDATION_RESP_BLANK_SLEEP_TIMER_VALUE /* "Enter Sleep Timer Value" */
            responseData.validationF = true
            sleepTimer.value = ""
        }

        // Sleep Timer
        if(sleepTimer.value.length > 0)
        {
            if(sleepTimer.value < minValueOfSetBrightness || sleepTimer.value > maxValueOfSetBrightness)
            {
                validF = false
                responseData.responseIndex = EnumClass.VALIDATION_RESP_INVALID_SLEEP_TIMER_VALUE /* "Enter Valid Sleep Timer Value" */
                responseData.validationF = true
            }
            tempSleepTimerValue = sleepTimer.value + String.fromCharCode(uiMsgFsp).toString()

        }

        // Spin Box
        tempActiveScreenSpinBoxValue = activeScreenSpinBox.value + String.fromCharCode(uiMsgFsp).toString()

        // Validation
        if(validF == true)
        {
            tempSleepTimerData   =  EnumClass.GUI_MSG_BRIGHTNESS_VALIDATION_EVENT +
                                    String.fromCharCode(uiMsgFsp).toString() +
                                    tempSleepTimerValue +
                                    tempActiveScreenSpinBoxValue +
                                    String.fromCharCode(eotFsp).toString()
            appData.data = tempSleepTimerData
        }

    }
}
