import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   autoTest
    height  :   lcd_height
    width   :   lcd_width

    OpacityRectangle
    {

    }

    Column
    {
        id                      : seftEnrollText
        height                  : 80
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : 100
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_DO_YOU_WANT_TO_START_934) /*"Do you want to start"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_SYSTEM_TEST_935)	/*"System Test?"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }

    Row
    {
        id : autoTestbuttonRow
        spacing : 5
        anchors.top: seftEnrollText.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        ButtonWithLabel
        {
            id : cancelAutoTestment
            button_Text:dispStr.readDispStr(DispStr.MSG_NO_385)	/*"No"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass. GUI_BUTTON_AUTO_SYSTEM_TEST_CANCEL+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
        ButtonWithLabel
        {
            id : startAutoTestment
            button_Text: dispStr.readDispStr(DispStr.MSG_YES_672) /*"Yes"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass.GUI_BUTTON_AUTO_SYSTEM_TEST_START +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
    }
}
