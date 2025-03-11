import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: buttonMainRect
    height  : 36
    width   : (isLargeButton ? 115 : 75)
    opacity : enableF ? 1 : 0.6

    property bool  isButtonInImageFormat : false
    property string buttonImagePath : buttonRect.source
    property alias button_Text : buttonText.text
    property bool enableF : true
    property bool isLargeButton : false
    signal buttonwithLabelPress
    Image
    {
        id     : buttonRect
        width  : (buttonPress.pressed && enableF) ? (parent.width * 0.80) : parent.width
        height : (buttonPress.pressed && enableF) ? (parent.height * 0.80) : parent.height
        anchors.centerIn: parent
        source : (isButtonInImageFormat) ? buttonImagePath : imageLocation + "/Controls/Button.png"
    }
    MouseArea
    {
        id: buttonPress
        anchors.centerIn: parent
        height  : parent.height +10
        width   : parent.width +10
        onPressed:
        {
            if(enableF)
            {
                sendOnlyKeepAlive()
    //            unloadKeypad()
                buttonMainRect.forceActiveFocus()
                buttonwithLabelPress()
            }
        }
    }
    MxText
    {
        id : buttonText
        color : "white"
        font.pointSize : (buttonPress.pressed && enableF) ? (fontSizeButton * 0.80) : fontSizeButton
        anchors.centerIn: parent
        smooth : true
        visible: (isButtonInImageFormat) ? false: true
        text : ""
    }
}


