import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : denyRect
    height  : lcd_height
    width   : lcd_width

    property bool   userInfoAvailableF   : false
    property int    denyReasonImageIndex : 0
    property string imgPath              : denyReasonImageIndex ? (imageLocation + "/AllowDenyInput/" + denyReasonImagePath[denyReasonImageIndex]) : ""

    property int    msgBoxTextWidthCafe  : 310
    property variant denyReasonImagePath:
    [
        "",
        "AccessDenied.png",
        "FingerNotIdentified.png",
        "CardNotIdentified.png",
        "PinNotIdentified.png",
        "FaceNotFound.png",
        "Failed.png"
    ]

    OpacityRectangle
    {

    }

    UserInfoRectangle
    {
        id                  : userInfoRectId
        anchors.top         : parent.top
        anchors.topMargin   : topMarginCafe
        visible             : userInfoAvailableF ? true : false
    }

    MxRectangleFrame
    {
        id                      : imgRect
        height                  : 100
        width                   : 120
        anchors.top             : userInfoRectId.bottom
        anchors.topMargin       : 10
        anchors.horizontalCenter: parent.horizontalCenter

        Image
        {
            id : cafeImg
            anchors.centerIn : parent
            source : imgPath
        }

    }

    Column
    {
        id              : normalColumn
        spacing         : 5
        anchors.top     : imgRect.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter


        MxText
        {
            id: denyText1
            width : msgBoxTextWidthCafe
        //    wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            color: mxColorRed
            font.bold : true
            font.pointSize: fontSizePopUpLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text:""
            visible : (text.length > 0) ? true : false
        }
        MxText
        {
            id: denyText2
            width : msgBoxTextWidthCafe
        //    wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            font.pointSize: fontSizePopUpLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text:""
            visible : (text.length > 0) ? true : false
        }
        MxText
        {
            id: denyText3
            width : msgBoxTextWidthCafe
        //    wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            font.pointSize: fontSizePopUpLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text:""
            visible : (text.length > 0) ? true : false
            maximumLineCount:2
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        userInfoAvailableF = parseInt(tmpPayload[0],10)
        userInfoRectId.userId = tmpPayload[1]
        userInfoRectId.userName = tmpPayload[2]
        userInfoRectId.userPhotoF = tmpPayload[3]
        userInfoRectId.userIdForPhoto = tmpPayload[8]
        userInfoRectId.loadUserPhoto()

        denyText1.text = tmpPayload[4]
        denyText2.text = tmpPayload[5]
        denyText3.text = tmpPayload[6]
        denyReasonImageIndex = parseInt(tmpPayload[7],10)
     }
}
