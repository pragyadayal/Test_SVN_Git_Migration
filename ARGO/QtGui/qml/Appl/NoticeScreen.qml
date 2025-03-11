import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : mainNoticeScreenRect
    height : lcd_height
    width  : lcd_width

    property int  noticeImageIndex : 0
    property bool pinButtonVisibleF: false
    property bool escButtonVisibleF: false
    //spacingRelated
    property int topMarginNoticeScreen : 80

    property string noticeText1 : ""
    property string noticeText2 : ""

    property string imgPath : noticeImageIndex ? (imageLocation + "/AllowDenyInput/" + noticeImagePath[noticeImageIndex]) : ""

    property variant noticeImagePath:
    [
        "",
        "RemoveFinger.gif",
        "ShowYourId.png",
        "ShowCard.gif",
        "Sucess.png",
        "Failed.png",
        "PlaceFinger.gif",
    ]

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : imgRect
        height : msgBoxHeight
        width : msgBoxWidth
        anchors.top: parent.top
        anchors.topMargin: topMarginNoticeScreen
        anchors.horizontalCenter: parent.horizontalCenter
        isBlankBox: true

        Column
        {
            anchors.centerIn: parent
            spacing : 10

            AnimatedImage
            {
                id : noticeImage
                anchors.horizontalCenter: parent.horizontalCenter
                source : imgPath
                cache :  false
            }

            MxText
            {
                id : noticeText1Id
                width : msgBoxTextWidth
                color : "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold : true
                text : noticeText1
             //   wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }            

            MxText
            {
                id : noticeText2Id
                width : msgBoxTextWidth
                color : "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold : true
                text : noticeText2
                horizontalAlignment: Text.AlignHCenter
            }

        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        setFooterBar(pinButtonVisibleF,escButtonVisibleF)
        noticeImage.playing = true
        
        if(deviceInactive == true && enrollPopUpActive !== true)
        {
            enrollPopUpActive = true
        }       
    }

    Component.onCompleted: {
       statusBarId.unloadSlideScreen()
    }
}
