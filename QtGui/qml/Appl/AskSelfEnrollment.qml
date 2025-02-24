import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   selfEnroll
    height  :   lcd_height
    width   :   lcd_width

    OpacityRectangle
    {

    }

    Column
    {
        id                      : seftEnrollText
        height                  : 100
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : 50
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_DO_YOU_WANT_TO_ENROLL_783) /*"Do you want to enroll"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_YOUR_FINGER_791)	/*"your finger?"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }

    Row
    {
        id : selfEnrollbuttonRow
        spacing : 5
        anchors.top: seftEnrollText.baseline
        anchors.centerIn: parent

        ButtonWithLabel
        {
            id : cancelSelfEnrollment
            button_Text:dispStr.readDispStr(DispStr.MSG_NO_385)	/*"No"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass. GUI_BUTTON_SELF_ENROLLMENT_CANCEL+
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
        ButtonWithLabel
        {
            id : startSelfEnrollment
            button_Text: dispStr.readDispStr(DispStr.MSG_YES_672) /*"Yes"*/
            onButtonwithLabelPress:
            {
                appData.data = EnumClass.GUI_BUTTON_SELF_ENROLLMENT_START +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
    }
}
