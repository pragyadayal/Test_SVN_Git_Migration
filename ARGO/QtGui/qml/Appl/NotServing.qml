import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    width   : lcd_width
    height  : lcd_height

    property string imgPath : imageLocation + "/PopUp/CafeNotServing.png"
    property string cardFingerimgPath : imageLocation + "/PopUp/Card_Finger.png"

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id      : imgRect
        height  : msgBoxHeight
        width   : msgBoxWidth
        anchors.centerIn : parent
        isBlankBox: true

        Image
        {
            id : notServingImg
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            source  : imgPath
        }
        MxText
        {
            id : notServingText
            anchors.top : notServingImg.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: mxColorRed
            font.pointSize: fontSizePopUpLabel
            font.bold: true
            text:dispStr.readDispStr(DispStr.MSG_NOT_SERVING_NOW_405) /*"Not Serving Now !"*/
        }
        Image
        {
            id : cardFingerImg
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: notServingText.bottom
            anchors.topMargin: 15
            source  : cardFingerimgPath
        }
        MxText
        {
            id : showCredText
            anchors.top: cardFingerImg.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            font.pointSize: fontSizeTextLabel
            font.bold: true
        //    wrapMode: Text.WordWrap
            text:internalFpSupport ? dispStr.readDispStr(DispStr.MSG_SHOW_CARD_FINGER_TO_794)+" "+dispStr.readDispStr(DispStr.MSG_VIEW_BALANCE_641) : dispStr.readDispStr(DispStr.MSG_SHOW_CARD_TO_1042)+" "+dispStr.readDispStr(DispStr.MSG_VIEW_BALANCE_641) /*"Show Card/Finger to view balance."*/
        }
    }

    CafeFooter
    {
        cafeFooterF      : true
        clearMenu        : false
        payBill          : false
        menu             : true
    }
}
