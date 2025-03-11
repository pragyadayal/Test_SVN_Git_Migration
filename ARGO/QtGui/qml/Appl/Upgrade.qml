import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    width: lcd_width
    height: lcd_height

    property string progressData
    property string headerData

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : popUpRect
        height : 160
        width : 200
        anchors.centerIn: parent
        isBlankBox: true

        MxText
        {
            id: headerText
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 15
            font.pointSize: fontSizePopUpLabel
            color: mxColorBlue
            text : headerData
        }

        AnimatedImage
        {
            id: processImage
            width: 160
            height: 60
            fillMode: Image.PreserveAspectFit
            smooth: true
            anchors.centerIn: parent
            source: imageLocation + "/Others/ProcessBar.gif"
        }

        MxText
        {
            id: processData
            text : progressData +"%"
            anchors.centerIn: processImage
            font.pointSize: fontSizePopUpLabel
            color: "white"
            visible: ( progressData != "255" ) ? true : false
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        processImage.playing = true
    }

    Component.onCompleted: {
       statusBarId.startStopSlideScreen(false)
    }
}
