import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: systemTest

    height : lcd_height
    width  : lcd_width

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : popUpRect
        height : msgBoxHeight
        width : msgBoxWidth
        anchors.centerIn: parent
        isBlankBox: true

        Column
        {
            spacing : 5
            anchors.centerIn: parent

            MxText
            {
                id: popUpTextHeading
                width : msgBoxTextWidth
                color : mxColorBlue
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                visible : (text.length > 0) ? true : false
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            MxText
            {
                id: popUpInfoLine1
                width : msgBoxTextWidth
                color: "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                visible : (text.length > 0) ? true : false
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            MxText
            {
                id: popUpInfoLine2
                width : msgBoxTextWidth
                color: "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                visible : (text.length > 0) ? true : false
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            MxText
            {
                id: popUpInfoLine3
                width : msgBoxTextWidth
                color: "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                visible : (text.length > 0) ? true : false
            //   wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
        popUpTextHeading.text = tmpPayload[0]
        popUpInfoLine1.text = tmpPayload[1]
        popUpInfoLine2.text = tmpPayload[2]
        popUpInfoLine3.text = tmpPayload[3]
    }
}
