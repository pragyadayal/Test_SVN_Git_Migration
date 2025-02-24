import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id      : textbox
    height  : 24
    width   : 130

    property alias value        : textboxData.originalData
    property alias maxChar      : textboxData.maximumLength
    property alias cursorPos    : textboxData.cursorPosition
    property bool  showCursor   : textboxData.cursorVisible
    property alias textOpacity  : textboxData.opacity
    property bool  isEnable         : true
    property bool  isCenterAligned  : false
    property bool  isIpField        : false
    property bool  isLargeTextBox   : false
    property int   prevCurPos
    property bool  checkForSpecialChar : false

//    property string bgImgPath : (imageLocation + "/Controls/" + (isIpField ? "SmallTextBox" : (isLargeTextBox ? "LargeTextBox" :"TextEditBox")) + (isEnable ? "" : "Disable") + ".png" )

    signal dotPressed
    signal backPressed
    signal rightPressed
    signal leftPressed
    signal activated
    signal invalidCharacterReceived

    onShowCursorChanged:
    {
        if(showCursor == true)
        {
            activated()
        }
    }

    property bool keypadAvailableF : (alphaNumKeypadF || numKeypadF)
    onKeypadAvailableFChanged:
    {
        if(keypadAvailableF == false)
        {
            textboxData.focus = false
            textboxData.cursorVisible = false
        }
    }

    onValueChanged:
    {
        if(textboxData.text.length != value.length)
        {
            refreshTextboxData()
        }
    }

    onActiveFocusChanged:
    {
        if(( isEnable == true ) && ( activeFocus == true ) && (textBoxF == true) )
        {
            textboxData.forceActiveFocus()
        }
    }

    MouseArea
    {
        id : touchArea
        height : parent.height + 10
        width  : parent.width + 10
        anchors.centerIn: parent
        onClicked:
        {
            if(isEnable == true)
            {
                textboxData.forceActiveFocus()
                if(alphaNumKeypadF == false)
                {
                   loadAlphaNumKeypad()
                }
            }
        }
    }    

    Rectangle
    {
        color   : "black"
        height  : textbox.height
        width   : textbox.width
        visible : true
        opacity : 0.3
    }
    TextInput
    {
        id:textboxData
        property string originalData : ""
        width : (isCenterAligned) ? parent.width : parent.width - 12
        anchors.centerIn: parent
        horizontalAlignment : (isCenterAligned) ? TextInput.AlignHCenter : TextInput.AlignLeft
        font.pointSize:fontSizeTextInputs*0.8
        text : ""
        color : "white"
        smooth : true
        font.bold : true
        opacity : isEnable? 1 : 0.5
        clip : true

        onTextChanged:
        {
            if(acceptableInput == false)
            {
                text = textboxData.text.toString().slice(0, (prevCurPos)) + textboxData.text.toString().slice(prevCurPos + 1, textboxData.text.length)
                originalData += originalData.slice(0, (prevCurPos)) + originalData.slice(prevCurPos + 1, originalData.length)
                cursorPos = prevCurPos
            }
        }
        onActiveFocusChanged:
        {
            if( (isEnable == true) && (activeFocus == true) && (textBoxF == true) )
            {
                sendOnlyKeepAlive()
                cursorVisible = true
                if(alphaNumKeypadF == false)
                {
                   loadAlphaNumKeypad()
                }
            }
            else
            {
                focus = false
                cursorVisible = false
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

        tempCurPos = cursorPos
        textLen = textboxData.originalData.length
        textboxData.text = ""

        while(indx < textLen)
        {
            textboxData.text += "\u25CF"
            indx++
        }
        cursorPos = tempCurPos
    }

    Connections
    {
        target: alphaNumKeypadId
        onHidePress:
        {
            textboxData.focus = false
            textboxData.cursorVisible = false
        }

        onVkpressed:
        {
            if( ipfieldCompleteF == true)
            {
                ipfieldCompleteF = false
            }
            else
            {
                if( textboxData.focus==true)
                {
                    prevCurPos = cursorPos

                    if(refreshTextboxTmr.running == true)
                    {
                        refreshTextboxData()
                    }

                    if (key === "LONGBACK")
                    {
                        textboxData.text = ""
                        textboxData.originalData = ""
                    }
                    else if ( (key === "BACK") || (key === "VIRTUALBACK") )
                    {
                        backPressed()
                        if(prevCurPos != 0)
                        {
                            textboxData.text=textboxData.text.toString().slice(0, (cursorPos-1)) + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                            textboxData.originalData = textboxData.originalData.slice(0, (prevCurPos-1)) + textboxData.originalData.slice(prevCurPos, textboxData.originalData.length)
                            prevCurPos--
                            cursorPos = prevCurPos
                        }
                    }
                    else if(key === "LEFT")
                    {
                        leftPressed()
                        if(cursorPos > 0)
                        {
                            cursorPos--
                        }
                    }
                    else if(key === "LONGLEFT")
                    {
                        cursorPos = 0
                    }
                    else if(key === "RIGHT")
                    {
                        rightPressed()
                        if(cursorPos < value.length)
                        {
                           cursorPos++
                        }
                    }
                    else if(key === "LONGRIGHT")
                    {
                        cursorPos = value.length
                    }
                    else if( (checkForSpecialChar == true) && ( (key === "#") || (key === "/") || (key === "+") ) )
                    {
                        invalidCharacterReceived()
                    }
                    else
                    {
                        if (key === ".")
                        {
                            dotPressed()
                        }
                        if(textboxData.text.length != maxChar)
                        {
                            if(key === "SPACE")
                            {
                                textboxData.text = textboxData.text.toString().slice(0, (cursorPos)) + " " + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                                textboxData.originalData = textboxData.originalData.slice(0, (prevCurPos)) + " " + textboxData.originalData.slice(prevCurPos, textboxData.originalData.length)
                            }
                            else
                            {
                                textboxData.text = textboxData.text.toString().slice(0, (cursorPos)) + key + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                                textboxData.originalData = textboxData.originalData.slice(0, (prevCurPos)) + key + textboxData.originalData.slice(prevCurPos, textboxData.originalData.length)
                            }

                            if(prevCurPos == cursorPos)
                            {
                                cursorPos = prevCurPos
                            }
                            else
                            {
                                prevCurPos++
                                cursorPos = prevCurPos
                            }
                        }
                    }
                    refreshTextboxTmr.restart()
                }
            }
        }
    }
}
