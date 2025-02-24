import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: textbox
    height: 24
    width : lcd_width * 0.6

    property alias value        : textboxData.text
    property alias maxChar      : textboxData.maximumLength
    property alias validation   : textboxData.validator
    property alias cursorPos    : textboxData.cursorPosition
    property alias  showCursor   : textboxData.cursorVisible
    property alias textOpacity  : textboxData.opacity
    property bool  isEnable     : true
    property bool isCenterAligned : false
    property int prevCurPos
    property bool isIpField : false
    property bool isLargeTextBox : false
    property bool maxValidationOccuranceF : false
    property bool dotEventF : false
    property bool isAcceptable : true
    property bool textBoxActivatedF : textBoxF
    property bool  checkForSpecialCharForUserName : false

 //   property string bgImgPath : (imageLocation + "/Controls/" + (isIpField ? "SmallTextBox" : (isLargeTextBox ? "LargeTextBox" :"TextEditBox")) + (isEnable ? "" : "Disable") + ".png" )

    signal dotPressed
    signal backPressed
    signal rightPressed
    signal leftPressed
    signal activated
    signal invalidCharacterReceivedForUserName

    onTextBoxActivatedFChanged:
    {
        if(textBoxActivatedF == true)
        {
            cursorPos = 0
        }
    }

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

    onActiveFocusChanged:
    {
        if(( isEnable == true ) && ( activeFocus == true ) && (textBoxActivatedF == true) )
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
        font.family: textFontFamily
        width : (isCenterAligned) ? parent.width : parent.width - 12
        anchors.centerIn: parent
        horizontalAlignment : (isCenterAligned) ? TextInput.AlignHCenter : TextInput.AlignLeft
        font.pointSize:fontSizeTextInputs
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
                cursorPos = prevCurPos
                isAcceptable = false
            }
            else
            {
                isAcceptable = true
            }
        }
        onActiveFocusChanged:
        {
            if( (isEnable == true) && (activeFocus == true) && (textBoxActivatedF == true) )
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
                    if (key === "LONGBACK")
                    {
                        textboxData.text = ""
                    }
                    else if (key === "BACK")
                    {
                        backPressed()
                        if(prevCurPos != 0)
                        {
                            textboxData.text=textboxData.text.toString().slice(0, (cursorPos-1)) + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                            prevCurPos--
                            cursorPos = prevCurPos
                        }
                    }
                    else if (key === "VIRTUALBACK")
                    {
                        if(isAcceptable && (cursorPos != 0) && (!maxValidationOccuranceF))
                        {
                            backPressed()
                            if(prevCurPos != 0)
                            {
                                textboxData.text=textboxData.text.toString().slice(0, (cursorPos-1)) + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                                prevCurPos--
                                cursorPos = prevCurPos
                            }
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
                    else if( (checkForSpecialCharForUserName == true) && (isKeyAlphaNumeric(key) == false) && (isCharValidForUserName(key) ==false) )
                    {
                        invalidCharacterReceivedForUserName()
                    }
                    else
                    {
                        isAcceptable = true
                        if (key === ".")
                        {
                            dotPressed()
                        }
                        if(textboxData.text.length != maxChar)
                        {
                            maxValidationOccuranceF = false
                            if(key === "SPACE")
                            {
                                textboxData.text = textboxData.text.toString().slice(0, (cursorPos)) + " " + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
                            }
                            else
                            {
                                textboxData.text = textboxData.text.toString().slice(0, (cursorPos)) + key + textboxData.text.toString().slice(cursorPos, textboxData.text.length)
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
                        else
                        {
                            maxValidationOccuranceF = true
                        }
                    }
                }
            }
        }
    }
    function setSelected()
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
    
    function isKeyAlphaNumeric(key)
    {
	switch(key)
	{
	  case "a":
	  case "A":
	  case "b":
	  case "B":
	  case "c":
	  case "C":
	  case "d":
	  case "D":
	  case "e":
	  case "E":
	  case "f":
	  case "F":
	  case "g":
	  case "G":
	  case "h":
	  case "H":
	  case "i":
	  case "I":
	  case "j":
	  case "J":
	  case "k":
	  case "K":
	  case "l":
	  case "L":
	  case "m":
	  case "M":
	  case "n":
	  case "N":
	  case "o":
	  case "O":
	  case "p":
	  case "P":
	  case "q":
	  case "Q":
	  case "r":
	  case "R":
	  case "s":
	  case "S":
	  case "t":
	  case "T":
	  case "u":
	  case "U":
	  case "v":
	  case "V":
	  case "w":
	  case "W":
	  case "x":
	  case "X":
	  case "y":
	  case "Y":
	  case "z":
	  case "Z":
	  case "0":
	  case "1":
	  case "2":
	  case "3":
	  case "4":
	  case "5":
	  case "6":
	  case "7":
	  case "8":
	  case "9":
	  {
	    return true;
	  }
	  default : 
	  {
	     return false;
	  }
	  
	}
      
    }
    
    function isCharValidForUserName(key)
    {
	switch(key)
	{
	  case "SPACE":
	  case "-":
	  case "_":
	  case ".":
	  case "(":
	  case ")":
	  {
	      return true;
	  }
	  default:
	  {
	      return false;
	  }
	  
	}
    }
}
