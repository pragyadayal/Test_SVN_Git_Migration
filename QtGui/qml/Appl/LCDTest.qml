import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: lcdTest
    width : lcd_width
    height: lcd_height

    property variant colorCode : ["#FF0000","#00FFFF","#0000FF","#0000A0","#ADD8E6",
                                  "#800080","#FFFF00","#00FF00","#FF00FF","#000000",
                                  "#FFFFFF"]
    Column
    {
        id: firstPattern1
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
        Repeater
        {
            id:colorCodeRepeater1
            model: colorCode.length
            Rectangle
            {
                width: lcdTest.width
                height: lcdTest.height/colorCode.length
                color: colorCode[index]
            }
        }
    }
    Row
    {
        id: firstPattern2
        anchors.verticalCenter: parent.verticalCenter
        visible: false
        Repeater
        {
            id:colorCodeRepeater2
            model: colorCode.length
            Rectangle
            {
                width: lcdTest.width/colorCode.length
                height: lcdTest.height
                color: colorCode[index]
            }
        }
    }
    Timer
    {
        id:patternTimer
        repeat: false
        onTriggered:
        {
            if(firstPattern1.visible == true)
            {
                firstPattern1.visible = false
            }
            else
            {
                firstPattern1.visible = true
            }

            if(firstPattern2.visible == true)
            {
                firstPattern2.visible = false
            }
            else
            {
                firstPattern2.visible = true
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
        patternTimer.interval = (parseInt(tmpPayload[0],10) / 2) * 1000
        patternTimer.start()
    }
}
