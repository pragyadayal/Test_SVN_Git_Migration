import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   adaptiveFr
    height  :   lcd_height
    width   :   lcd_width

    property string payLoad : ""
    property int noOfSec
    property int marginBwImgAndText
    property int marginBwText
    property int fontSize
    property int fontSizeTimerText
    property int sideWidthMargin
    OpacityRectangle
    {

    }
    Timer
    {
        id: frTimer
        interval: 1000
        repeat: true
        property int timerCount: noOfSec
        onTriggered: {
            timerCount--
            if(timerCount === 0){
            appData.data = EnumClass.GUI_BUTTON_ADAPTIVE_FR_RESP +
                    String.fromCharCode(uiMsgFsp).toString() +
                    (2).toString() +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
                frTimer.stop()
            }
        }
    }

    Rectangle
    {
        id                      : adaptiveFrInfo
        width                   : (lcd_width - sideWidthMargin)
        anchors.centerIn        : parent
        color                   : "transparent"

        Image
        {
            id                      : warningImg
            anchors.top             : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode                : Image.PreserveAspectFit
            source                  : imageLocation + "/Others/WarningIcon.png"
        }
        MxText
        {
            id                      : adaptiveText
            width                   : parent.width
            anchors.top             : warningImg.bottom
            anchors.topMargin       : marginBwImgAndText
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSize
            wrapMode                : Text.Wrap
            color                   : "white"
        }
        MxText
        {
            id                      : adaptiveTimerCnt
            width                   : parent.width
            anchors.top             : adaptiveText.bottom
            anchors.topMargin       : marginBwText
            horizontalAlignment     : TextInput.AlignHCenter
            font.pointSize          : fontSizeTimerText
            text                    : dispStr.readDispStr(DispStr.MSG_AUTOMATICALLY_ABORT_IN_1017) + " " + (frTimer.timerCount).toString() + " " + dispStr.readDispStr(DispStr.MSG_SEC_1018)
            wrapMode                : Text.Wrap
            color                   : "#a4a4a4"
        }
        Item
        {
            id: buttons
            anchors.top: adaptiveTimerCnt.bottom
            anchors.topMargin: marginBwImgAndText
            anchors.horizontalCenter: parent.horizontalCenter
            height: 36
            Row
            {
                id : buttonRow
                spacing : 5
                anchors.centerIn: parent
                ButtonWithLabel
                {
                    id : leftButton
                    button_Text: ""
                    enableF : true
                    isButtonInImageFormat: true
                    buttonImagePath: imageLocation + "/ButtonIcons/ok and set.png"

                    onButtonwithLabelPress:
                    {
                        appData.data = EnumClass.GUI_BUTTON_ADAPTIVE_FR_RESP +
                                String.fromCharCode(uiMsgFsp).toString() +
                                (1).toString() +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()

                        payLoad += (EnumClass.GUI_MSG_DYNAMIC_POP_UP_SCREEN).toString() + String.fromCharCode(uiMsgFsp).toString() + "" + String.fromCharCode(uiMsgFsp).toString() + (55).toString() + String.fromCharCode(uiMsgFsp).toString() +
                                dispStr.readDispStr(DispStr.MSG_PROCESSING_467) +  String.fromCharCode(uiMsgFsp).toString() + "" + String.fromCharCode(uiMsgFsp).toString() + (1).toString() +  String.fromCharCode(uiMsgFsp).toString() + String.fromCharCode(eotFsp).toString()
                        frTimer.stop()
                        appData.slotData(payLoad)
                    }
                }
                ButtonWithLabel
                {
                    id : rightButton
                    button_Text:""
                    enableF : true
                    isButtonInImageFormat: true
                    buttonImagePath: imageLocation + "/ButtonIcons/cancel.png"

                    onButtonwithLabelPress:
                    {
                        appData.data = EnumClass.GUI_BUTTON_ADAPTIVE_FR_RESP +
                                String.fromCharCode(uiMsgFsp).toString() +
                                (0).toString() +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
                        frTimer.stop()
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        if (deviceType === EnumClass.DEVICE_TYPE_VEGA)
        {
             sideWidthMargin    = 10
             marginBwImgAndText = 15
             marginBwText       = 10
             fontSize           = 15
             fontSizeTimerText  = 14
        }
        else if(deviceType === EnumClass.DEVICE_TYPE_ARGO)
        {
            sideWidthMargin    = 20
            marginBwImgAndText = 18
            marginBwText       = 12
            fontSize           = 19
            fontSizeTimerText  = 17
        }
        else
        {
            sideWidthMargin    = 10
            marginBwImgAndText = 18
            marginBwText       = 12
            fontSize           = 18
            fontSizeTimerText  = 16
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {

        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        noOfSec = parseInt(tmpPayload[0],10)
        frTimer.start()
        adaptiveText.text = dispStr.readDispStr(DispStr.MSG_FACE_WILL_ENROLL_AUTOMATICALLY_FOR_1016) +  " \"" + tmpPayload[2] + " " + "(" + tmpPayload[1] + ")" + "\"" + ". " + dispStr.readDispStr(DispStr.MSG_CONTINUE_103) + "?"
        adaptiveFrInfo.height = warningImg.implicitHeight + 2 * marginBwImgAndText + adaptiveText.implicitHeight + marginBwText + adaptiveTimerCnt.implicitHeight + buttons.height
    }
}
