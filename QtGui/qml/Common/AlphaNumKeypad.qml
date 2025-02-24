import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: alphanumKeyPad
    height : alphaNumKeypadGrid.sourceSize.height
    width  : alphaNumKeypadGrid.sourceSize.width
    anchors.bottom: parent.bottom

    signal vkpressed(string key)
    signal hidePress()

    property alias capsFlag : keyalphanum.isCapsOn
    property alias symbolFlag : keyalphanum.isSymbolOn

    Image
    {
        id: alphaNumKeypadGrid
        anchors.bottom : parent.bottom
        source: imageLocation + "/Keypad/AlphaNumeric/KeypadBg" + (symbolFlag ? "" : "Caps") +".png"

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
        }

        Item
        {
            id               : backButton
            height           : 35
            width            : 40
            opacity          : backFooterArea.pressed ? 0.5 : 1
            anchors.bottom   : alphaNumKeypadGrid.top
            anchors.left     : alphaNumKeypadGrid.left
            visible          : backVisibleF

            Image
            {
                id : backImage
                source: imageLocation + "/Footer/Back.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
                anchors.centerIn: parent
                height:  backFooterArea.pressed ? (backImage.sourceSize.height * 0.9) : (backImage.sourceSize.height)
                width :  backFooterArea.pressed ? (backImage.sourceSize.width * 0.9) : (backImage.sourceSize.width)
            }
            MouseArea
            {
                id: backFooterArea
                anchors.fill: parent
                onPressed:
                {
                    sendBackButtonPressed()
                }
            }
        }

        Item
        {
            id               : homeButton
            height           : 35
            width            : 40
            opacity          : homeFooterArea.pressed ? 0.5 : 1
            anchors.bottom   : alphaNumKeypadGrid.top
            anchors.right    : alphaNumKeypadGrid.right
            anchors.rightMargin: (backVisibleF ? 0 : ((alphaNumKeypadGrid.sourceSize.width/2)-(width/2)) )
            visible          : homeVisibleF

            Image
            {
                id : homeImage
                source: imageLocation + "/Footer/Home.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
                anchors.centerIn: parent
                height:  homeFooterArea.pressed ? (homeImage.sourceSize.height * 0.9) : (homeImage.sourceSize.height)
                width :  homeFooterArea.pressed ? (homeImage.sourceSize.width * 0.9) : (homeImage.sourceSize.width)
            }
            MouseArea
            {
                id: homeFooterArea
                anchors.fill: parent
                onPressed:
                {
                    appData.data = EnumClass.GUI_BUTTON_HOME +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
                }
            }
        }

        Column
        {
            id: keyalphanum
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 9
            spacing: keyalphanum.spaceBetKeysV
            //key sets
            property variant firstKeySet:      ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0" ]
            property variant secondUpperKeySet:["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P" ]
            property variant secondLowerKeySet:["q", "w", "e", "r", "t", "y", "u", "i", "o", "p" ]
            property variant thirdUpperKeySet: ["A", "S", "D", "F", "G", "H", "J", "K", "L"]
            property variant thirdLowerKeySet: ["a", "s", "d", "f", "g", "h", "j", "k", "l"]
            property variant forthUpperKeySet: ["CAPS", "Z", "X", "C", "V", "B", "N", "M", "BACK"]
            property variant forthLowerKeySet: ["CAPS", "z", "x", "c", "v", "b", "n", "m", "BACK"]
            property variant fifthKeySet:      ["?*&", "LEFT", "SPACE", "RIGHT", ".", "HIDE"]
            property variant symbolfirstKeySet:["@", "#", "%", "&", "*", "/", "-", "+", "(", ")"]
            property variant symbolsecondKeySet:["~", "?", "!", "\u0022", "'", ":", ";", "^", "|", "\u005c" ]
            property variant symbolthirdKeySet:["[", "]", "{", "}", "_", ",", "<", ">", "`"]
            property variant symbolforthKeySet:["BLANK","\u20b1", "$", "\u20ac", "\u00a3", "\u00a5", "\u00a2", "=", "BACK"]
            property variant symbolfifthKeySet:["Abc", "LEFT", "SPACE", "RIGHT", ".", "HIDE"]

            property bool isCapsOn : false
            property bool isSymbolOn : false

            property int buttonSize_W:22
            property int buttonSize_H:22

            //width of the clear, enter, escape and capitalize button
            property int specialButtonWidth:37
            property int specialButtonWidth1:54
            property int specialButtonWidth2:37

            property int sideButtonWidth : 22

            //property int space bar button width
            property int spaceBarWidth:54
            property real spaceBetKeysH:9
            property real spaceBetKeysV:7

            //row of 1st key set
            Row
            {
                id: first
                spacing:keyalphanum.spaceBetKeysH
                anchors.horizontalCenter: parent.horizontalCenter
                //repeat the keys
                Repeater
                {
                    model: keyalphanum.firstKeySet.length
                    //button for the keys
                    MxKeypadButton
                    {
                        width:keyalphanum.buttonSize_W
                        height:keyalphanum.buttonSize_H
                        keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                        operation: keyalphanum.isSymbolOn? keyalphanum.symbolfirstKeySet[index] :keyalphanum.firstKeySet[index]
                        onKeyPressed:vkpressed(keyalphanum.isSymbolOn? keyalphanum.symbolfirstKeySet[index] :keyalphanum.firstKeySet[index])
                    }
                }
            }
            //row of 2nd key set
            Row
            {
                id: second
                spacing:keyalphanum.spaceBetKeysH
                anchors.horizontalCenter: parent.horizontalCenter
                //repeat the keys
                Repeater
                {
                    model: keyalphanum.secondUpperKeySet.length
                    //button for the keys
                    MxKeypadButton
                    {
                        width:keyalphanum.buttonSize_W
                        height:keyalphanum.buttonSize_H
                        keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                        operation:keyalphanum.isSymbolOn? keyalphanum.symbolsecondKeySet[index]: (keyalphanum.isCapsOn? keyalphanum.secondUpperKeySet[index]: keyalphanum.secondLowerKeySet[index])
                        onKeyPressed:
                        {
                            if(keyalphanum.isSymbolOn == true)
                            {
                                    vkpressed( keyalphanum.symbolsecondKeySet[index])
                            }
                            else
                            {
                                if(keyalphanum.isCapsOn == true)
                                {
                                    vkpressed( keyalphanum.secondUpperKeySet[index])
                                }
                                else
                                {
                                    vkpressed( keyalphanum.secondLowerKeySet[index])
                                }
                            }
                        }
                    }
                }
            }

            //row of 3rd key set
            Row
            {
                id : third
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:keyalphanum.spaceBetKeysH
                //repeat the keys
                Repeater
                {
                    model: keyalphanum.thirdLowerKeySet.length
                    MxKeypadButton
                    {
                        width:keyalphanum.buttonSize_W
                        height:keyalphanum.buttonSize_H
                        keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                        operation: keyalphanum.isSymbolOn? keyalphanum.symbolthirdKeySet[index]: (keyalphanum.isCapsOn ? keyalphanum.thirdUpperKeySet[index]: keyalphanum.thirdLowerKeySet[index])
                        onKeyPressed:
                        {
                            if(keyalphanum.isSymbolOn == true)
                            {
                                    vkpressed( keyalphanum.symbolthirdKeySet[index])
                            }
                            else
                            {
                                if(keyalphanum.isCapsOn == true)
                                {
                                    vkpressed( keyalphanum.thirdUpperKeySet[index])
                                }
                                else
                                {
                                    vkpressed( keyalphanum.thirdLowerKeySet[index])
                                }
                            }
                        }
                    }
                }
            }

            //row of 4th key set
            Row
            {
                id:forth
                spacing:keyalphanum.spaceBetKeysH
                anchors.horizontalCenter: parent.horizontalCenter
                //button for caps
                MxKeypadButton
                {
                    width: keyalphanum.specialButtonWidth
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation:keyalphanum.isSymbolOn ?  keyalphanum.symbolforthKeySet[0] : keyalphanum.forthUpperKeySet[0]
                    onKeyPressed:
                    {
                        sendAlnumKeepAlive()
                        if(keyalphanum.isSymbolOn != true)
                        {
                            if(keyalphanum.isCapsOn == true)
                            {
                                keyalphanum.isCapsOn = false
                            }
                            else
                            {
                                keyalphanum.isCapsOn = true
                            }
                        }
                    }
                }
                //repeat the keys
                Repeater
                {
                    model:keyalphanum.forthUpperKeySet.length-2
                    MxKeypadButton
                    {
                        width:keyalphanum.buttonSize_W
                        height:keyalphanum.buttonSize_H
                        keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                        operation:keyalphanum.isSymbolOn? keyalphanum.symbolforthKeySet[index+1]: (keyalphanum.isCapsOn ? keyalphanum.forthUpperKeySet[index+1]: keyalphanum.forthLowerKeySet[index+1])
                        onKeyPressed:
                        {
                            if(keyalphanum.isSymbolOn == true)
                            {
                                    vkpressed( keyalphanum.symbolforthKeySet[index+1])
                            }
                            else
                            {
                                if(keyalphanum.isCapsOn == true)
                                {
                                    vkpressed( keyalphanum.forthUpperKeySet[index+1])
                                }
                                else
                                {
                                    vkpressed( keyalphanum.forthLowerKeySet[index+1])
                                }
                            }
                        }
                    }
                }
                //button for back space
                MxKeypadButton
                {
                    width: keyalphanum.specialButtonWidth2
                    height: keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.forthLowerKeySet[8]
                    onKeyPressed:
                    {
                        vkpressed( keyalphanum.forthLowerKeySet[8])
                    }
                    onKeyLongPressed:
                    {
                        vkpressed("LONGBACK")
                    }
                }
            }
            //row of fifth keyset
            Row
            {
                id:fifth
                spacing:keyalphanum.spaceBetKeysH
                anchors.horizontalCenter: parent.horizontalCenter
                //button for Sym and abc
                MxKeypadButton
                {
                    width: keyalphanum.specialButtonWidth1
                    height: keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.isSymbolOn ? keyalphanum.symbolfifthKeySet[0] : keyalphanum.fifthKeySet[0]
                    onKeyPressed:
                    {
                        sendOnlyKeepAlive()
                        if(keyalphanum.isSymbolOn == true)
                        {
                            keyalphanum.isSymbolOn= false
                            currentHoverKey = "Abc"
                        }
                        else
                        {
                            keyalphanum.isSymbolOn =true
                            currentHoverKey = "?*&"
                        }
                    }
                }
                //button for Left key
                MxKeypadButton
                {
                    width: keyalphanum.sideButtonWidth
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.fifthKeySet[1]
                    onKeyPressed:
                    {
                        vkpressed( keyalphanum.fifthKeySet[1])
                    }
                    onKeyLongPressed:
                    {
                        vkpressed("LONGLEFT")
                    }
                }
                //button for Space
                MxKeypadButton
                {
                    width: keyalphanum.spaceBarWidth
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.fifthKeySet[2]
                    onKeyPressed:
                    {
                        vkpressed( keyalphanum.fifthKeySet[2])
                    }
                }
                //button for Right
                MxKeypadButton
                {
                    width: keyalphanum.sideButtonWidth
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.fifthKeySet[3]
                    onKeyPressed:
                    {
                        vkpressed( keyalphanum.fifthKeySet[3])
                    }
                    onKeyLongPressed:
                    {
                        vkpressed("LONGRIGHT")
                    }
                }
                //button for .
                MxKeypadButton
                {
                    width: keyalphanum.specialButtonWidth1
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.fifthKeySet[4]
                    onKeyPressed:
                    {
                        vkpressed( keyalphanum.symbolfifthKeySet[4])
                    }
                }
                //button for HIDE
                MxKeypadButton
                {
                    width: keyalphanum.specialButtonWidth1
                    height:keyalphanum.buttonSize_H
                    keypadType : EnumClass.KEYPAD_ALPHANUMERIC
                    operation: keyalphanum.fifthKeySet[5]
                    onKeyPressed:
                    {
						hidePress()
                    }
                }
            }
        }
    }
}
