import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

import "../payLoadHandler.js" as PayloadLib

Item
{
    id: enrollSplFunctionCardSelectionPopUp
    height : lcd_height
    width  : lcd_width

    property int specialFunctionIndex

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id                       : popUpWindow
        height                   : 220
        width                    : 220
        anchors.centerIn         : parent

        MxText
        {
            id: heading
            text : dispStr.readDispStr(DispStr.MSG_SELECT_NO_OF_CARDS_520)  /*"Select No. Of Cards"*/
            color: "white"
            font.pointSize: fontSizePopUpLabel
            anchors.top: parent.top
            anchors.topMargin : 10
            anchors.horizontalCenter: popUpWindow.horizontalCenter
            font.bold: true
        }

        SpinBox
        {
            id: spin
            anchors.centerIn: parent
            bottomValidation    : 1
        }

        ButtonElement
        {
            id : buttons
			
			isInImageFormat:true
            leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
            rightBtnImagePath: imageLocation + "/ButtonIcons/enroll.png"

            anchors.bottomMargin: 5
            buttonRowVisibleF: true
            //buttonLeftText: "Cancel"
            onButtonLeftPress:
            {
                popUpLoader.unloadPopUp()
            }

            //buttonRightText: "Enroll"
            onButtonRightPress:
            {
                generateEnrollSplFuncPayload()
                popUpLoader.unloadPopUp()
            }
        }
    }
    function generateEnrollSplFuncPayload()
    {
        var tempSplFuncPayload

        tempSplFuncPayload = EnumClass.MSG_ENROL_SPL_FN_VALIDATION_EVENT +
                             String.fromCharCode(uiMsgFsp).toString() +
                             specialFunctionIndex.toString() +
                             String.fromCharCode(uiMsgFsp).toString()+
                             spin.value+
                             String.fromCharCode(uiMsgFsp).toString()+
                             String.fromCharCode(eotFsp).toString()

        //console.log("Enroll Special Func Payload----->" +tempSplFuncPayload)

        appData.data = tempSplFuncPayload
    }

    function processDeviceResponse(tempSplFuncIndex, tempConfigCardNo, maxCardNo)
    {
        specialFunctionIndex = tempSplFuncIndex
        spin.value = tempConfigCardNo
        spin.topValidation = maxCardNo
    }
}

