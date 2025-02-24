import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id:numericKeyPad
    height : numericKeyPadGrid.sourceSize.height
    width  : numericKeyPadGrid.sourceSize.width
    signal vkpressed(string key)
    signal hidePress()

    Image
    {
        id: numericKeyPadGrid
        anchors.bottom : parent.bottom
        source: imageLocation + "/Keypad/Numeric/Background.png"

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
        }

        Column
        {
            id: keypadnum
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom:parent.bottom
            anchors.bottomMargin:6

            //key sets
            property variant firstKeySet:["1", "2", "3"]
            property variant secondKeySet:["4", "5", "6"]
            property variant thirdKeySet:["7", "8", "9"]
            property variant fourthKeySet:["0", "BACK"]

            property int buttonSize_W:100
            property int buttonSize_H:75
            property int hSpaceBetKeys:4
            property int vSpaceBetKeys:4
            property int zeroButtonSize_W: (2*buttonSize_W + hSpaceBetKeys)
            spacing: vSpaceBetKeys

            //row of 1st key set
            Row
            {
                 spacing:keypadnum.hSpaceBetKeys
                 //repeat the keys
                 Repeater
                 {
                        model: keypadnum.firstKeySet.length
                        //button for the keys
                        MxKeypadButton
                        {
                            width:keypadnum.buttonSize_W
                            height:keypadnum.buttonSize_H
                            keypadType : EnumClass.KEYPAD_NUMERIC
                            operation: keypadnum.firstKeySet[index]
                            onKeyPressed:vkpressed(keypadnum.firstKeySet[index])
                            fontSizeKeypad: fontSizePinEntryKey
                            fontBoldF: 1
                         }
                  }
            }
            //row of 2nd key set
            Row
            {
                  spacing:keypadnum.hSpaceBetKeys
                  //repeat the keys
                  Repeater
                  {
                        model: keypadnum.secondKeySet.length
                        //button for the keys
                        MxKeypadButton
                        {
                            width:keypadnum.buttonSize_W
                            height:keypadnum.buttonSize_H
                            keypadType : EnumClass.KEYPAD_NUMERIC
                            operation:keypadnum.secondKeySet[index]
                            onKeyPressed:vkpressed( keypadnum.secondKeySet[index])
                            fontSizeKeypad: fontSizePinEntryKey
                            fontBoldF: 1
                        }
                  }
            }
            //row of 3rd key set
            Row
            {
                  spacing:keypadnum.hSpaceBetKeys
                  //repeat the keys
                  Repeater
                  {
                        model: keypadnum.thirdKeySet.length
                        MxKeypadButton
                        {
                            width:keypadnum.buttonSize_W
                            height:keypadnum.buttonSize_H
                            keypadType : EnumClass.KEYPAD_NUMERIC
                            operation:keypadnum.thirdKeySet[index]
                            onKeyPressed:vkpressed(keypadnum.thirdKeySet[index])
                            fontSizeKeypad: fontSizePinEntryKey
                            fontBoldF: 1
                         }
                   }
             }
             //row of 4th key set
             Row
             {
                  spacing:keypadnum.hSpaceBetKeys
                  //0 key
                  MxKeypadButton
                  {
                      width:keypadnum.zeroButtonSize_W
                      height:keypadnum.buttonSize_H
                      keypadType : EnumClass.KEYPAD_NUMERIC
                      operation:keypadnum.fourthKeySet[0]
                      onKeyPressed:vkpressed( keypadnum.fourthKeySet[0])
                      fontSizeKeypad: fontSizePinEntryKey
                      fontBoldF: 1
                  }

                  //Back Button
                  MxKeypadButton
                  {
                      width:keypadnum.buttonSize_W
                      height:keypadnum.buttonSize_H
                      keypadType : EnumClass.KEYPAD_NUMERIC
                      operation: keypadnum.fourthKeySet[1]
                      onKeyPressed:vkpressed( keypadnum.fourthKeySet[1])
                      onKeyLongPressed: vkpressed("LONGBACK")
                  }
             }
        }
    }
}
