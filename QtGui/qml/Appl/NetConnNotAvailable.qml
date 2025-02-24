import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : mainRectId
    width: lcd_width
    height: lcd_height

    OpacityRectangle
    {

    }

    AnimatedImage
    {
        id : img
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20
        source : imageLocation + "/Others/NetworkNotAvailable.png"
        smooth : true
        cache : false
    }

    MxText
    {
        id : text1
        width : parent.width
        color : mxColorRed
        font.pointSize: fontSizePopUpLabel
        font.bold: true
        anchors.top:img.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text : dispStr.readDispStr(DispStr.MSG_SORRY_568) /*"Sorry,"*/
        visible : (text.length > 0) ? true : false
    //    wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }
    MxText
    {
        id : text2
        width : parent.width
        color : mxColorRed
        font.pointSize: fontSizePopUpLabel
        font.bold: true
        anchors.top:text1.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text : dispStr.readDispStr(DispStr.MSG_NETWORK_CONNECTION_NOT_AVAILABLE_840) /*"Network Connection not available."*/
        visible : (text.length > 0) ? true : false
     //   wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }
    MxText
    {
        id : text3
        width : parent.width
        color : mxColorRed
        font.pointSize: fontSizePopUpLabel
        font.bold: true
        anchors.top:text2.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text : dispStr.readDispStr(DispStr.MSG_TRY_AGAIN_LATER_841) /*"Try again later."*/
        visible : (text.length > 0) ? true : false
     //   wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }
}

