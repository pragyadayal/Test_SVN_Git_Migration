import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id : dialPadMainRect
    height : backImage.sourceSize.height
    width  : backImage.sourceSize.width
    anchors.bottom: parent.bottom

    property alias stateIdx : dialPadMainRect.currentState
    property int currentState : 0

    signal dialKeyPressed(string key, int dummy)
    signal vkpressed(string key)
    signal hidePress()

    property string currentKey : ""
    property int currentInstance : 0
    property bool timerRunningF : false
    property string currHoverKeyText : ""
    property bool isSpecialState : (currentState == 1 || currentState == 2) ? true : false

    property string bgImgPath : imageLocation + "/Keypad/Dialpad/Background_" + currentState + ".png"
    Item
    {
        id               : backButton
        height           : 35
        width            : 40
        opacity          : backFooterArea.pressed ? 0.5 : 1
        anchors.bottom   : backImage.top
        anchors.left     : backImage.left
        visible          : backVisibleF

        Image
        {
            id : backImg
            source: imageLocation + "/Footer/Back.png"
            fillMode: Image.PreserveAspectFit
            smooth: true
            anchors.centerIn: parent
            height:  backFooterArea.pressed ? (backImg.sourceSize.height * 0.9) : (backImg.sourceSize.height)
            width :  backFooterArea.pressed ? (backImg.sourceSize.width * 0.9) : (backImg.sourceSize.width)
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
        anchors.bottom   : backImage.top
        anchors.right    : backImage.right
        anchors.rightMargin: (backVisibleF ? 0 : ((backImage.sourceSize.width/2)-(width/2)) )
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

    Image
    {
        id : backImage
        source: bgImgPath
        anchors.bottom: parent.bottom

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
        }

        Column
        {
            id : dialPad
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            spacing : vSpaceBetKeys

            property int buttonSize_W:70
            property int buttonSize_H:32
            property int vSpaceBetKeys:4
            property int hSpaceBetKeys:6

            property variant alphaKey :
            [
                [
                    [".","1"],
                    ["a","b","c","2"],
                    ["d","e","f","3"],
                    ["g","h","i","4"],
                    ["j","k","l","5"],
                    ["m","n","o","6"],
                    ["p","q","r","s","7"],
                    ["t","u","v","8"],
                    ["w","x","y","z","9"],
                ],
                [
                    [".","1"],
                    ["A","B","C","2"],
                    ["D","E","F","3"],
                    ["G","H","I","4"],
                    ["J","K","L","5"],
                    ["M","N","O","6"],
                    ["P","Q","R","S","7"],
                    ["T","U","V","8"],
                    ["W","X","Y","Z","9"],
                ]
            ]

            property variant dialKey :
            [
                [
                    ["1","2","3","BACK"],
                    ["4","5","6","SIDE"],                  //State : 0
                    ["7","8","9","."],
                    ["HIDE","0","SPACE","Abc"]
                ],
                [
                    [".","abc","def","BACK"],
                    ["ghi","jkl","mno","SIDE"],            //State : 1
                    ["pqrs","tuv","wxyz","CAPS"],
                    ["HIDE","0","SPACE","?*&"]
                ],
                [
                    [".","ABC","DEF","BACK"],
                    ["GHI","JKL","MNO","SIDE"],            //State : 2
                    ["PQRS","TUV","WXYZ","CAPS"],
                    ["HIDE","0","SPACE","?*&"]
                ],
                [
                    ["@","#","%","BACK"],
                    ["+","/","*","SIDE"],                  //State : 3
                    ["-","_","^","1/4"],
                    ["HIDE",".","SPACE","123"]
                ],
                [
                    ["&","?","!","BACK"],
                    ["(","{","[","SIDE"],                  //State : 4
                    [")","}","]","2/4"],
                    ["HIDE",".","SPACE","123"]
                ],
                [
                    ["|","\u005c","~","BACK"],
                    ["<",">","=","SIDE"],                  //State : 5
                    [",",";",":","3/4"],
                    ["HIDE",".","SPACE","123"]
                ],
                [
                    ["\u20b1", "$", "\u20ac","BACK"],
                    ["\u00a3","\u00a5","\u00a2","SIDE"],   //State : 6
                    ["\u0022","'","`","4/4"],
                    ["HIDE",".","SPACE","123"]
                ]
            ]

            Row
            {
                id: firstRow
                spacing:dialPad.hSpaceBetKeys
                //repeat the keys
                Repeater
                {
                    id : rowRepeater1
                    model:4
                    property int rowIdx : 0
                    //button for the keys
                    MxKeypadButton
                    {
                        id : keyButton1
                        width:dialPad.buttonSize_W
                        height:dialPad.buttonSize_H
                        keypadType: EnumClass.KEYPAD_DIALPAD
                        operation : dialPad.dialKey[currentState][rowRepeater1.rowIdx][index]
                        currentHoverKey: currHoverKeyText
                        isMultiKey : ( isSpecialState && (index < 3)) ? true : false
                        onKeyPressed:
                        {
                            dialKeyPressed(operation,((3 * rowRepeater1.rowIdx) + index))
                        }
                        onKeyLongPressed:
                        {
                            if(operation == "BACK")
                            {
                                vkpressed("LONGBACK")
                            }
                            else if( isMultiKey )
                            {
                                vkpressed("VIRTUALBACK")
                                var currKey = ((3 * rowRepeater1.rowIdx) + index + 1).toString()
                                vkpressed(currKey)
                                currHoverKeyText = currKey
                            }
                        }
                    }
                }
            }

            Row
            {
                id: secondRow
                spacing:dialPad.hSpaceBetKeys
                //repeat the keys
                Repeater
                {
                    id : rowRepeater2
                    model:3
                    property int rowIdx : 1
                    //button for the keys
                    MxKeypadButton
                    {
                        id : keyButton2
                        width:dialPad.buttonSize_W
                        height:dialPad.buttonSize_H
                        keypadType: EnumClass.KEYPAD_DIALPAD
                        operation : dialPad.dialKey[currentState][rowRepeater2.rowIdx][index]
                        currentHoverKey: currHoverKeyText
                        isMultiKey : ( isSpecialState && (index < 3)) ? true : false
                        onKeyPressed:
                        {
                            dialKeyPressed(operation,((3 * rowRepeater2.rowIdx) + index))
                        }
                        onKeyLongPressed:
                        {
                            if( isMultiKey )
                            {
                                vkpressed("VIRTUALBACK")
                                var currKey = ((3 * rowRepeater2.rowIdx) + index + 1).toString()
                                vkpressed(currKey)
                                currHoverKeyText = currKey
                            }
                        }
                    }
                }

                MxSideKeyButton
                {
                    id : sideKeyId
                    width:dialPad.buttonSize_W
                    height:dialPad.buttonSize_H
                    onKeyPressed:
                    {
                        vkpressed(key)
                    }
                }
            }
            Row
            {
                id: thirdRow
                spacing:dialPad.hSpaceBetKeys
                //repeat the keys
                Repeater
                {
                    id : rowRepeater3
                    model:4
                    property int rowIdx : 2
                    //button for the keys
                    MxKeypadButton
                    {
                        id : keyButton3
                        width:dialPad.buttonSize_W
                        height:dialPad.buttonSize_H
                        keypadType: EnumClass.KEYPAD_DIALPAD
                        operation : dialPad.dialKey[currentState][rowRepeater3.rowIdx][index]
                        currentHoverKey: currHoverKeyText
                        isMultiKey : ( isSpecialState && (index < 3)) ? true : false
                        onKeyPressed:
                        {
                            dialKeyPressed(operation,((3 * rowRepeater3.rowIdx) + index))
                        }
                        onKeyLongPressed:
                        {
                            if( isMultiKey )
                            {
                                vkpressed("VIRTUALBACK")
                                var currKey = ((3 * rowRepeater3.rowIdx) + index + 1).toString()
                                vkpressed(currKey)
                                currHoverKeyText = currKey
                            }
                        }
                    }
                }
            }
            Row
            {
                id: forthRow
                spacing:dialPad.hSpaceBetKeys
                //repeat the keys
                Repeater
                {
                    id : rowRepeater4
                    model:4
                    property int rowIdx : 3
                    //button for the keys
                    MxKeypadButton
                    {
                        id : keyButton4
                        width:dialPad.buttonSize_W
                        height:dialPad.buttonSize_H
                        keypadType: EnumClass.KEYPAD_DIALPAD
                        operation : dialPad.dialKey[currentState][rowRepeater4.rowIdx][index]
                        currentHoverKey: currHoverKeyText
                        onKeyPressed:
                        {
                            dialKeyPressed(operation,((3 * rowRepeater4.rowIdx) + index))
                        }
                    }
                }
            }
        }
    }

    onDialKeyPressed:
    {
        currHoverKeyText = key
        if(key === "HIDE")
        {
            hidePress()
        }
        else if(key === "?*&")
        {
            currentState = 3
        }
        else if(key === "Abc")
        {
            currentState = 1
        }
        else if(key === "123")
        {
            currentState = 0
        }
        else if(key === "CAPS")
        {
            if(currentState == 1)
            {
                currentState = 2
            }
            else if (currentState == 2)
            {
                currentState = 1
            }
        }
        else if(key === "1/4")
        {
            currentState = 4
        }
        else if(key === "2/4")
        {
            currentState = 5
        }
        else if(key === "3/4")
        {
            currentState = 6
        }
        else if(key === "4/4")
        {
            currentState = 3
        }
        else if( !isSpecialState )
        {
            vkpressed(key)
        }
        else if( (key === "BACK") || (key === "SPACE") || (key === "0") || (key === "LEFT") || (key === "LONGLEFT") || (key === "RIGHT") || (key ==="LONGRIGHT"))
        {
            vkpressed(key)
        }
        else
        {
            if(interDigitTimerF == false)
            {
                keyTimer.start()
                currentKey = key
                currentInstance = 1
                interDigitTimerF = true
            }
            else
            {
                if(currentKey === key)
                {
                    keyTimer.restart()
                    currentInstance++
                    if( (currentInstance == 6) || (( currentInstance == 5) && (dummy != 6) && (dummy != 8)) || (( currentInstance == 3) && (dummy == 0)))
                    {
                        currentInstance = 1
                    }
                    vkpressed("VIRTUALBACK")
                }
                else
                {
                    keyTimer.start()
                    currentKey = key
                    currentInstance = 1
                    interDigitTimerF = true
                }
            }

            var tempKey = dialPad.alphaKey[(currentState - 1)][dummy][currentInstance-1]
            vkpressed(tempKey)
            currHoverKeyText = tempKey
        }
    }

    Timer
    {
        id : keyTimer
        interval: 1000
        running : false

        onTriggered:
        {
            currentKey = ""
            currentInstance = 0
            interDigitTimerF = false
        }
    }
}
