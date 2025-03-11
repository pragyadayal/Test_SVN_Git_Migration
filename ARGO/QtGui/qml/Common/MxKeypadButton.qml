import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id: mxkeypadbutton
    property int keypadType : EnumClass.KEYPAD_NONE  // 1 for AlphaNumeric
                                                     // 2 for Numeric
                                                     // 3 for DialPad
    property int timerInterval : isMultiKey ? 1000 : 300
    property bool isHoverEnable : hoverEnableF
    property bool showHoverF : ( (mouseArea.pressed || keyTimer.running) && (operation != "BLANK") )

    property string operation
    property string key:buttonText.text
    signal keyPressed(string key)
    signal keyLongPressed(string key)

    property int fontSizeDialpad : 13
    property int fontSizeAlphanumKeypad : 13
    property int fontSizeKeypad : 17
    property int hoverTextSize : 1
    property int fontBoldF: 0
    //Dialpad related additional information
    property string currentHoverKey : operation
    property bool isMultiKey : false

    color:"transparent"
    smooth : true

    Rectangle
    {
        anchors.fill: parent
        color : "black"
        opacity : showHoverF ? 0.4 : 0
    }

    property bool   keyImgF         : ( (operation == "BACK") || (operation == "HIDE") || (operation == "SPACE" ) || (operation == "CAPS") || (operation == "LEFT") || (operation == "RIGHT") || (operation == "BLANK")) ? true : false
    property bool   hoverImageF     : ( (hoverText == "BACK") || (hoverText == "HIDE") || (hoverText == "SPACE" ) || (hoverText == "CAPS") || (hoverText == "LEFT") || (hoverText == "RIGHT")) ? true : false
    property string hoverText       : currentHoverKey
    property string imgFolderPath   : imageLocation + "/Keypad/AlphaNumeric/"

    Image
    {
        id: keyPressedImage
        source: (imgFolderPath + "KeyHoverNum.png")
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible : (isHoverEnable && showHoverF)

        Rectangle
        {
            id : hoverRect
            color : "transparent"
            width : keyPressedImage.width
            height :keyPressedImage.height - 10
            anchors.horizontalCenter: keyPressedImage.horizontalCenter
            anchors.top: keyPressedImage.top

            MxText
            {
                id: keyPressedText
                anchors.centerIn: parent
                color: mxColorBlue
                text: hoverText
                font.pointSize: mxkeypadbutton.hoverTextSize
                visible : (! hoverImageF)
            }
        }
        Image
        {
            id : keyPressedHoverImage
            anchors.centerIn: keyPressedImage
            source: (imgFolderPath + (hoverImageF ? (hoverText + "_hover.png") : "KeyHoverNum.png"))
            visible : (hoverImageF)
        }
    }
    MxText
    {
        id: buttonText
        anchors.centerIn: parent
        color: "white"
        text: operation
        visible : ! ((keypadType == EnumClass.KEYPAD_DIALPAD) || keyImgF)
    }
    MouseArea
    {
        id: mouseArea
        anchors.centerIn: parent
        height: parent.height + 4
        width : parent.width + 4
        onPressed:
        {
            sendAlnumKeepAlive()
            keyPressed(key)
        }

        onPressAndHold:
        {
            sendAlnumKeepAlive()
            keyLongPressed(key)
        }

        onReleased:
        {
            keyTimer.start()
        }
    }

    Timer
    {
        id : keyTimer
        interval: timerInterval
        running : false
    }

    onCurrentHoverKeyChanged:
    {
        keyTimer.stop()
    }

    Component.onCompleted:
    {
        switch(keypadType)
        {
            case EnumClass.KEYPAD_ALPHANUMERIC :
            {
                buttonText.font.pointSize = fontSizeAlphanumKeypad
                hoverTextSize = (fontSizeAlphanumKeypad * 1.25)
                imgFolderPath = imageLocation + "/Keypad/AlphaNumeric/"
                break
            }
            case EnumClass.KEYPAD_NUMERIC:
            {
                buttonText.font.pointSize = fontSizeKeypad
                buttonText.font.bold = fontBoldF
                hoverTextSize = (fontSizeKeypad * 2 )
                imgFolderPath = imageLocation + "/Keypad/Numeric/"
                break
            }
            case EnumClass.KEYPAD_DIALPAD:
            {
                buttonText.font.pointSize = fontSizeDialpad
                hoverTextSize = (fontSizeDialpad * 1.3)
                imgFolderPath = imageLocation + "/Keypad/Dialpad/"
                break
            }
        }
    }
}
