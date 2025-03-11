import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : denyRect
    height : lcd_height
    width  : lcd_width

    OpacityRectangle
    {
        visible: true
    }

    MxRectangleFrame
    {
        id: unidentifiedUser
        height  : msgBoxHeight
        width   : msgBoxWidth
        anchors.top: denyRect.top
        anchors.topMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
        isBlankBox : true

        Column
        {
            id:connectFailedInfo
            anchors.centerIn: parent
            spacing: 5

            Image
            {
                id: connectFailedImage
                anchors.horizontalCenter: parent.horizontalCenter
                source: imageLocation + "/AllowDenyInput/Failed.png"
            }

            MxText
            {
                id: denyText1
                width : msgBoxTextWidth
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.pointSize: fontSizePopUpLabel
                font.bold: true
                text:""
                visible : (text.length > 0) ? true : false
            }
        }
    }


    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        denyText1.text = tmpPayload[0]

     }
}
