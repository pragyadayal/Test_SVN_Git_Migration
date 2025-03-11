import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    height              : lcd_height
    width               : lcd_width
    anchors.centerIn    : parent

    property bool   changeDoorModeF : true
    property int    serverMode      : 0

    OpacityRectangle
    {

    }

    Column
    {
        height                  : 200
        width                   : 300
        spacing                 : 3
        anchors.top             : parent.top
        anchors.topMargin       : 50
        anchors.horizontalCenter: parent.horizontalCenter

        MxText
        {
            id                  : confirmDefltline1
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_STORED_EVENTS_AND_676)/*"Events Stored Will"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline2
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_CONFIGURATIONS_WILL_683)/*"Events Stored Will"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline3
            anchors.horizontalCenter: parent.horizontalCenter
            text                :dispStr.readDispStr(DispStr.MSG_BE_DELETED_799)/*"Be Deleted"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline4
            anchors.horizontalCenter: parent.horizontalCenter
            text                : "    "
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline5
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_PRESS_ENTER_TO_800)/*"Press Enter To"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : confirmDefltline6
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_PROCEED_801)/*"Proceed"*/
            horizontalAlignment: TextInput.AlignHCenter
            font.pointSize      : fontSizeSpinBoxIndex
            smooth              : true
            color               : "white"
        }
        ButtonWithLabel
        {
            id : confirmDefltBtn
            anchors.horizontalCenter: parent.horizontalCenter
//          isLargeButton: true
            isButtonInImageFormat: true
            buttonImagePath: imageLocation + "/ButtonIcons/enter.png"
            //button_Text: "Enter"
            onButtonwithLabelPress:
            {
                if(samayModeF === 0)
                {
                    popUpLoader.loadRestartMsg()
                }
                else
                {
                    if(changeDoorModeF == true)
                    {
                        appData.data = EnumClass.GUI_BUTTON_DOOR_MODE_CHANGE +
                             String.fromCharCode(uiMsgFsp).toString() +
                             String.fromCharCode(eotFsp).toString()
                    }
                    else
                    {
                        appData.data = EnumClass.GUI_BUTTON_SERVER_MODE_CHANGE +
                             String.fromCharCode(uiMsgFsp).toString() +
                             serverMode +
                             String.fromCharCode(uiMsgFsp).toString() +
                             String.fromCharCode(eotFsp).toString()
                    }


                }

            }
         }
    }
}

