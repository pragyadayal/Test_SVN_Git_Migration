import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    width : lcd_width
    height: lcd_height

    property int    triggerType     : 0
    property string pinData         : ""
    property variant headerData     : [
                                        "",
                                        dispStr.readDispStr(DispStr.MSG_PRESS_THE_WPS_PUSH_BUTTON_OF_YOUR_WI_FI_ACCESS_POINT_WITHIN_464),/*"Press the WPS Push Button of your Wi-Fi Access Point within"*/
                                        dispStr.readDispStr(DispStr.MSG_ENTER_THE_NUMBER_209)/*"Enter the number"*/  +pinData +
                                                dispStr.readDispStr(DispStr.MSG_IN_YOUR_WI_FI_ACCESS_POINT_WITHIN_862)/*"in your Wi-Fi Access Point within"*/,
                                      ]
    property int progress
    property int noOfSec : 120
    property int dur : (noOfSec * 10 )

    Behavior on progress
    {
        NumberAnimation{ duration: dur * 100; from : noOfSec}
    }

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : popUpRect
        height : 160
        width : 200
        anchors.centerIn: parent

        MxText
        {
            id: headerText
            width : 180
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            text : headerData[triggerType]
            color: "white"
            clip : true
            font.pointSize: fontSizePopUpLabel
        //    wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
        MxText
        {
            id: progressText
            width : 180
            anchors.top: headerText.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            text : (progress +" "+dispStr.readDispStr(DispStr.MSG_SEC_863)/*"seconds..."*/)
            color: "white"
            clip : true
            font.pointSize: fontSizePopUpLabel
         //   wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle
        {
            height : 10
            width : 164
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 35
            color : "transparent"
            border.color: "grey"
            border.width: 2
            smooth : true

            Rectangle
            {
                id : progressRect
                height : 6
                width  : 0
                anchors.verticalCenter: parent.verticalCenter
                anchors.left : parent.left
                anchors.leftMargin: 2
                color : (progress < (noOfSec * 0.25) ? mxColorRed : "#0a70a2")

                Behavior on width
                {
                    NumberAnimation{ duration: dur * 100; from : 160}
                }
            }
        }
    }

    Component.onCompleted:
    {
        progressRect.width = 0
        progress = 0
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
