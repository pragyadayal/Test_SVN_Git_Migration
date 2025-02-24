import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : allowRect
    height  : lcd_height
    width   : lcd_width

    property bool       cafeTextSepF    : false
    property int        imgIdx          : 0
    property variant    imgPath         : [imageLocation + "/AllowDenyInput/Success.png",
                                           imageLocation + "/AllowDenyInput/ViewBalance_Success.png"
                                          ]

    OpacityRectangle
    {

    }

    UserInfoRectangle
    {
        id                  : userInfoRectId
        anchors.top         : parent.top
        anchors.topMargin   : topMarginCafe
    }

    MxRectangleFrame
    {
        id                      : imgRect
        height                  : 100
        width                   : 120
        anchors.top             : userInfoRectId.bottom
        anchors.topMargin       : 10
        anchors.horizontalCenter: userInfoRectId.horizontalCenter

        Image
        {
            id      : cafeImg
            anchors.centerIn: parent
            source  : imgPath[imgIdx]
        }
    }

    Column
    {
        id                      :allowInformation
        anchors.top             : imgRect.bottom
        anchors.topMargin       : 5
        anchors.horizontalCenter: parent.horizontalCenter
        spacing                 : 4
        width                   : lcd_width - 30

        CafeAllowText
        {
            id : cafeRow1
        }
        CafeAllowText
        {
            id : cafeRow2
        }
        CafeAllowText
        {
            id : cafeRow3
        }
        CafeAllowText
        {
            id : cafeRow4
        }
        CafeAllowText
        {
            id : cafeRow5
        }
    }

    Column
    {
        id                      : normalColumn
        anchors.top             : imgRect.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin       : 5
        spacing                 : 4
        visible                 : (! cafeTextSepF)

        MxText
        {
            id              : cafeAllowText1
            anchors.horizontalCenter: parent.horizontalCenter
            color           : "white"
            font.pointSize  : fontSizePopUpLabel
            text            :""
        }
        MxText
        {
            id              : cafeAllowText2
            anchors.horizontalCenter: parent.horizontalCenter
            color           : "white"
            font.pointSize  : fontSizePopUpLabel
            text            :""
        }
        MxText
        {
            id              : cafeAllowText3
            anchors.horizontalCenter: parent.horizontalCenter
            color           : "white"
            font.pointSize  : fontSizePopUpLabel
            text            :""
        }
        MxText
        {
            id              : cafeAllowText4
            anchors.horizontalCenter: parent.horizontalCenter
            color           : "white"
            font.pointSize  : fontSizePopUpLabel
            text            :""
        }
        MxText
        {
            id              : cafeAllowText5
            anchors.horizontalCenter: parent.horizontalCenter
            color           : "white"
            font.pointSize  : fontSizePopUpLabel
            text            :""
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
