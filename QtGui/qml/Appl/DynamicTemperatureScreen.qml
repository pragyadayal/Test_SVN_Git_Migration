import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: dynamicTempScreen
    height : lcd_height
    width  : lcd_width
    property int topMarginUserRect : 60

    OpacityRectangle
    {

    }

    property string heading       : ""
    property int imgIdx           : 0
    property string imgPath       : ""
    property string textInfoLine1 : ""
    property string dynamicTempImgSrc  : ""

    property variant dynamicTempImg:
    [
        "DetectingTemperature.gif",
        "SensorDisconnected.gif",
        "FullApproachSensor.gif",
        "PartialApproachSensor.png",
    ]

    MxText
    {
        id: dynamicTempScreenText
        visible : (text.length > 0) ? true : false
        width: lcd_width - 20
        text: heading
        font.pointSize: 16
        color: "white"
        anchors.centerIn: parent
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

    AnimatedImage
    {
        id : dynamictempScreenImg
        width: lcd_width
        height: lcd_height
        anchors.centerIn: parent
        source : dynamicTempImgSrc
        smooth : true
        cache : false
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MxUserInfoRectangle
    {
        id : userInfo
        anchors.top: dynamictempScreenImg.top
        anchors.topMargin: 60
    }


    function processDeviceResponse (msgId,rcvData)
    {
        var footerF = false;
        var ImagHideF = true;
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
        heading = tmpPayload[0]
        imgIdx = tmpPayload[1]

        setFooterConfigIdx(EnumClass.FOOTER_HOME)
        footerF = parseInt(tmpPayload[2],10)
        footerBarHideF = (footerF ? true : false)

        ImagHideF = parseInt(tmpPayload[3],10)
        userInfo.userId=tmpPayload[4]
        userInfo.userIdForPhoto=tmpPayload[5]
        userInfo.userName=tmpPayload[6]

        if(ImagHideF == 0)
        {
           // Dynamic temperature screen for Direct door
            dynamicTempImgSrc = imageLocation + "/Temperature/" + dynamicTempScreen.dynamicTempImg[imgIdx]
            dynamictempScreenImg.playing = true
        }
        else
        {
            dynamicTempImgSrc = ""
           userInfo.visible = false
        }
        userInfo.loadUserPhoto()
        if((dynamicTempImg[imgIdx] == "SensorDisconnected.gif") || (userInfo.userId == ""))
        {
            userInfo.visible = false
        }
    }
}
