import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: ambientTempDetectingItem
    width: lcd_width
    height: lcd_height

    property int countDown : 5

    OpacityRectangle
    {

    }

    MxText
    {
        id: stepAwayText
        wrapMode: Text.Wrap
        anchors.bottom: detectionTempText.top
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        text: dispStr.readDispStr(DispStr.MSG_STEP_AWAY_FROM_THE_DEVICE_990)
        color   : "white"
        font.pointSize: 15
    }

    MxText
    {
        id: detectionTempText
        wrapMode: Text.Wrap
        anchors.top: stepAwayText.bottom
        anchors.topMargin: 10
        anchors.centerIn: parent
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        text: dispStr.readDispStr(DispStr.MSG_DETECTING_AMBIENT_TEMPERATURE_IN_991)
        color   : "white"
        font.pointSize: 15
    }

    Rectangle
    {
        id: countDownCircle
        height: 50
        width:  50
        radius: 50
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        color: "black"
        border.color: "black"
        border.width: 2
        opacity: 0.5
    }

    MxText
    {
        id: tempCountdown
        wrapMode: Text.Wrap
        anchors.centerIn: countDownCircle
        horizontalAlignment: Text.AlignHCenter
        text: (countDown != 0) ? countDown : 1
        color   : "white"
        font.pointSize: 30
    }

    Behavior on countDown
    {
        NumberAnimation
        {
            duration : 5000
            from : countDown
            to : 0
        }
    }

    onCountDownChanged:
    {
        if(countDown == 0)
        {
            sendOnlyKeepAlive()

            appData.data = EnumClass.GUI_MSG_AMBIENT_COUNTDOWN_COMPLETED +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
        }
    }

    Component.onCompleted:
    {
        countDown = 5
    }
}
