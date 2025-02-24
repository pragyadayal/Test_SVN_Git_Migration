import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Row
{
    id:iprow
    spacing:2

    property alias firstField   :ipadd1.value
    property alias secondField  :ipadd2.value
    property alias thirdField   :ipadd3.value
    property alias forthField   :ipadd4.value
    property bool  isEnableF    : true

    property bool isDotPressed       : false
    property int  dotPressedFieldIdx : 0
    property int  dotPressedFieldLen : 0
    property bool interDigitF        : interDigitTimerF
    property bool showCursor         : (ipadd1.showCursor || ipadd2.showCursor || ipadd3.showCursor || ipadd4.showCursor)

    property int ipFieldHeight      : lcd_height / 20
    property int ipFieldwidth       : lcd_width / 8

    property string dotFsp  : '.'

    signal activated

    onShowCursorChanged:
    {
        if(showCursor == true)
        {
            activated()
        }
    }

    onInterDigitFChanged:
    {
        if((interDigitF == false) && isDotPressed && (dotPressedFieldLen == getCursorPos(dotPressedFieldIdx)))
        {
            isDotPressed = false
            setFieldFocus(dotPressedFieldIdx+1)
            dotPressedFieldIdx = 0
            dotPressedFieldLen = 0
        }
    }

    TextEditBox
    {
        id:ipadd1
        height : ipFieldHeight
        width  : ipFieldwidth
        maxChar: 3
        validation: IntValidator {bottom: 0; top:255}
        isEnable: isEnableF
        isCenterAligned : true
        isIpField: true

        onValueChanged:
        {
            if(prevCurPos == 2 && cursorPos == 3)
            {
                setFieldFocus(2)
            }
        }

        onDotPressed:
        {
            if(interDigitF == false)
            {
                setFieldFocus(2)
            }
            else
            {
                isDotPressed = true
                dotPressedFieldIdx = 1
                dotPressedFieldLen = cursorPos
            }
        }

        onRightPressed:
        {
            if(cursorPos == value.length)
            {
                setFieldFocus(2)
            }
        }
    }

    TextEditBox
    {
        id:ipadd2
        height : ipFieldHeight
        width  : ipFieldwidth
        validation: IntValidator{bottom: 0; top:255}
        isEnable:isEnableF
        maxChar: 3
        isCenterAligned : true
        isIpField: true
        onValueChanged:
        {
            if(prevCurPos == 2 && cursorPos == 3)
            {
                setFieldFocus(3)
            }
        }

        onDotPressed:
        {
            if(interDigitF == false)
            {
                setFieldFocus(3)
            }
            else
            {
                isDotPressed = true
                dotPressedFieldIdx = 2
                dotPressedFieldLen = cursorPos
            }
        }

        onBackPressed :
        {
            if(cursorPos == 0)
            {
                setFieldFocus(1)
            }
        }

        onLeftPressed:
        {
            if(cursorPos == 0)
            {
                setFieldFocus(1)
            }
        }

        onRightPressed:
        {
            if(cursorPos == value.length)
            {
                setFieldFocus(3)
            }
        }
    }
    TextEditBox
    {
        id:ipadd3
        height : ipFieldHeight
        width  : ipFieldwidth
        validation: IntValidator{bottom: 0; top:255}
        isEnable: isEnableF
        maxChar: 3
        isCenterAligned : true
        isIpField: true

        onValueChanged:
        {
            if(prevCurPos == 2 && cursorPos == 3)
            {
                setFieldFocus(4)
            }
        }

        onDotPressed:
        {
            if(interDigitF == false)
            {
                setFieldFocus(4)
            }
            else
            {
                isDotPressed = true
                dotPressedFieldIdx = 3
                dotPressedFieldLen = cursorPos
            }
        }

        onBackPressed :
        {
            if(cursorPos == 0)
            {
                setFieldFocus(2)
            }
        }

        onLeftPressed:
        {
            if(cursorPos == 0)
            {
                setFieldFocus(2)
            }
        }

        onRightPressed:
        {
            if(cursorPos == value.length)
            {
                setFieldFocus(4)
            }
        }
    }
    TextEditBox
    {
        id:ipadd4
        height : ipFieldHeight
        width  : ipFieldwidth
        validation: IntValidator{bottom: 0; top:255}
        isEnable: isEnableF
        maxChar: 3
        isCenterAligned : true
        isIpField: true

        onValueChanged:
        {
            if (prevCurPos == 2 && cursorPos == 3)
            {
                ipfieldCompleteF =true
            }
        }

        onBackPressed :
        {
            if(cursorPos == 0)
            {
                setFieldFocus(3)
            }
        }

        onLeftPressed:
        {
            if(cursorPos == 0)
            {
                setFieldFocus(3)
            }
        }
    }

    function isIpFieldFilled()
    {
        if ( (firstField.length > 0) &&
                (secondField.length > 0) &&
                (thirdField.length > 0) &&
                (forthField.length > 0) )
        {
            return true
        }
        else
        {
            return false
        }
    }

    function isIpFieldEmpty()
    {
        if ( (firstField.length == 0) &&
                (secondField.length == 0) &&
                (thirdField.length == 0) &&
                (forthField.length == 0) )
        {
            return true
        }
        else
        {
            return false
        }
    }

    function clearIpField()
    {
        firstField = ""
        secondField = ""
        thirdField = ""
        forthField = ""
    }

    function getIpFieldValue()
    {
        return (firstField + "." + secondField  + "." + thirdField   + "." + forthField)
    }

    function setIpFieldValue(str)
    {
        var tempIpData
        tempIpData = str.split(dotFsp)

        if(tempIpData.length == 4)
        {
            firstField  = tempIpData[0]
            secondField = tempIpData[1]
            thirdField  = tempIpData[2]
            forthField  = tempIpData[3]
        }
        else
        {
            clearIpField()
        }
    }

    function setFieldFocus(idx)
    {
        switch(idx)
        {
            case 1:
                ipadd1.cursorPos = ipadd1.value.length
                ipadd1.focus=true
                ipadd1.showCursor =true
                break
            case 2:
                ipadd2.cursorPos = ipadd2.value.length
                ipadd2.focus=true
                break
            case 3:
                ipadd3.cursorPos = ipadd3.value.length
                ipadd3.focus=true
                break
            case 4:
                ipadd4.cursorPos = ipadd4.value.length
                ipadd4.focus=true
                break
        }
        ipfieldCompleteF = true
    }

    function getCursorPos(idx)
    {
        var temp
        switch(idx)
        {
            case 1:
                temp = ipadd1.cursorPos
                break
            case 2:
                temp = ipadd2.cursorPos
                break
            case 3:
                temp = ipadd3.cursorPos
                break
            case 4:
                temp = ipadd4.cursorPos
                break
        }
        return temp
    }
    function setSelected()
    {
        ipadd1.setSelected()
    }

}
