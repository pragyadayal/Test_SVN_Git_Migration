import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : pinEntryRectId
    width   : lcd_width
    height  : lcd_height - 325

    property int prevCurPos : 0

    OpacityRectangle
    {

    }

    ButtonElement
    {
        id : buttons

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        buttonRowVisibleF: true
		isInImageFormat:true
        leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath: imageLocation + "/ButtonIcons/ok and set.png"

        //buttonLeftText: "Cancel"
        onButtonLeftPress:
        {
            appData.data = EnumClass.GUI_BUTTON_CANCEL +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }

        //buttonRightText: "Ok"
        onButtonRightPress:
        {
            appData.data = EnumClass.MSG_PIN_VALIDATION_EVENT +
                    String.fromCharCode(uiMsgFsp).toString() +
                    pinData.originalPin +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }

    MxRectangleFrame
    {
        id: passwordEntryect
        anchors.top : parent.top
        anchors.topMargin: 25+6//topMarginPswPin
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        height: 70
    }

    MxText
    {
        id: passwordLable
        anchors.top: passwordEntryect.top
        anchors.topMargin: 10
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: passwordEntryect.horizontalCenter
        color: "white"
        text:dispStr.readDispStr(DispStr.MSG_ENTER_YOUR_PIN_211) /*"Enter Your PIN:"*/
        font.pointSize: fontSizePinEntry
    }

    Rectangle
    {
        id      : textBoxImg
        color   : "black"
        anchors.bottom			: passwordEntryect.bottom
        anchors.bottomMargin	: 10
        anchors.horizontalCenter: passwordEntryect.horizontalCenter
        height  : 26                    // height/width value reference from another file
        width   : 130
        visible : true
        opacity : 0.3
    }
    TextInput
    {
        id          : pinData
        property string originalPin : ""
        width       : textBoxImg.width
        focus       : true
        color       : "white"
        anchors.centerIn    : textBoxImg
        horizontalAlignment : TextInput.AlignHCenter
        font.pointSize      : fontSizeTextInputs * 0.8
        cursorVisible       : true
        maximumLength       : 15

        onOriginalPinChanged:
        {
            if(text.length != originalPin.length)
            {
                refreshTextboxData()
            }
        }
    }

    Timer
    {
        id:refreshTextboxTmr
        interval:500
        onTriggered:
        {
            refreshTextboxData()
        }
    }

    function refreshTextboxData()
    {
        if(refreshTextboxTmr.running == true)
        {
            refreshTextboxTmr.stop()
        }

        var indx = 0, textLen, tempCurPos

        tempCurPos = pinData.cursorPosition
        textLen = pinData.originalPin.length
        pinData.text = ""

        if(showPin == 0)
        {
            while(indx < textLen)
            {
                pinData.text += "\u25CF"
                indx++
            }
        }
        else
        {
            pinData.text = pinData.originalPin
        }

        pinData.cursorPosition = tempCurPos
    }

    Connections
    {
        target: numKeyadId

        onVkpressed:
        {
            prevCurPos = pinData.cursorPosition

            if(refreshTextboxTmr.running == true)
            {
                refreshTextboxData()
            }

            if(key === "LONGBACK")
            {
                pinData.text = ""
                pinData.originalPin = ""
            }
            else if (key === "BACK")
            {
                if(prevCurPos != 0)
                {
                    pinData.text = pinData.text.toString().slice(0, (pinData.cursorPosition-1)) + pinData.text.toString().slice(pinData.cursorPosition, pinData.text.length)
                    pinData.originalPin = pinData.originalPin.slice(0, (prevCurPos-1)) + pinData.originalPin.slice(prevCurPos, pinData.originalPin.length)
                    prevCurPos--
                }
            }
            else
            {
                if(pinData.text.length != pinData.maximumLength)
                {
                    pinData.text = pinData.text.toString().slice(0, (pinData.cursorPosition)) + key + pinData.text.toString().slice(pinData.cursorPosition, pinData.text.length)
                    pinData.originalPin = pinData.originalPin.slice(0, (prevCurPos)) + key + pinData.originalPin.slice(prevCurPos, pinData.originalPin.length)
                    prevCurPos++
                }
            }
            refreshTextboxTmr.restart()
            pinData.cursorPosition = prevCurPos
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        pinData.forceActiveFocus()
        pinData.text = ""
        pinData.originalPin = ""
        loadNumericKeypad()
    }
}
