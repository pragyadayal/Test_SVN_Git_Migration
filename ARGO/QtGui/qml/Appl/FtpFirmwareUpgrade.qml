import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   ftpFmwUpgrade
    height  :   lcd_height
    width   :   lcd_width

    OpacityRectangle
    {

    }

    Column
    {
        id                      : ftpFmwUpgradeText
        height                  : 100
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : 100
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                      : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_INITIALIZING_FIRMWARE_523) /*"Do you want to enroll"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
        MxText
        {
            id                      : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                    : dispStr.readDispStr(DispStr.MSG_UPGRADE_573)	/*"your finger?"*/
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeSpinBoxIndex
            smooth                  : true
            color                   : "white"
        }
    }

    ButtonElement
    {
        id : buttons
        addBtnBottomMargin  : 50
        buttonRowVisibleF   : true
        isInImageFormat     : true
        leftBtnImagePath    : imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath   : imageLocation + "/ButtonIcons/ok and set.png"

        //buttonLeftText: "Cancel"
        onButtonLeftPress:
        {
            appData.data = EnumClass.GUI_BUTTON_FTP_FMW_UPGRADE_CANCEL +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }

        //buttonRightText: "Ok"
        onButtonRightPress:
        {
            appData.data = EnumClass.GUI_BUTTON_FTP_FMW_UPGRADE_OK +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }
}
