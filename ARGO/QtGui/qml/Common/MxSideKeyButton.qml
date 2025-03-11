import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id : sideKeyButton
    signal keyPressed(string key)

    Row
    {
        id : mxSideKeyRow
        anchors.centerIn: parent
        MxKeypadButton
        {
            height : sideKeyButton.height
            width : sideKeyButton.width/2
            keypadType : EnumClass.KEYPAD_DIALPAD
            operation: "LEFT"
            currentHoverKey: "LEFT"
            onKeyPressed:sideKeyButton.keyPressed("LEFT")
            onKeyLongPressed: sideKeyButton.keyPressed("LONGLEFT")

        }
        MxKeypadButton
        {
            height : sideKeyButton.height
            width : sideKeyButton.width/2
            keypadType : EnumClass.KEYPAD_DIALPAD
            operation: "RIGHT"
            currentHoverKey: "RIGHT"
            onKeyPressed:sideKeyButton.keyPressed("RIGHT")
            onKeyLongPressed: sideKeyButton.keyPressed("LONGRIGHT")
        }
    }
}
