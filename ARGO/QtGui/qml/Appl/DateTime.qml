import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib


Item
{
    id                      : dateTimeItemID
    height                  : lcd_height
    width                   : lcd_width

    property bool  manualF  : false
    property bool  dateTimeUpdateF         // Date --> true
                                           // Time --> false
    property string imgPath : imageLocation + "/Controls/LargeButtonBackground.png"

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_DATE_AND_TIME_109)  /*DATE_AND_TIME*/
    }

    MxText
    {
        id                   : modeTextId
        anchors.top          : parent.top
        anchors.topMargin    : topMarginDisplaySetting
        anchors.left         : dateRectId.left
        anchors.leftMargin   : 5
        color                : mxColorBlue
        text                 : dispStr.readDispStr(DispStr.MSG_MODE_797) /*"Mode :"*/
        font.pointSize       : 15
    }

    MxText
    {
        id                   : modeTypeId
        anchors.top          : parent.top
        anchors.topMargin    : topMarginDisplaySetting
        anchors.left         : modeTextId.right
        anchors.leftMargin   : 10
        color                : "white"
        text                 : manualF ? dispStr.readDispStr(DispStr.MSG_MANUAL_857)/*"Manual"*/:dispStr.readDispStr(DispStr.MSG_AUTO_50) /*"Auto"*/
        font.pointSize       : 15
    }

    Image
    {
        id                   : dateRectId
        height               : sourceSize.height
        width                : sourceSize.width
        anchors.top          : modeTextId.bottom
        anchors.topMargin    : 10               //start below mode text + 20
        anchors.horizontalCenter : parent.horizontalCenter
        source               : imgPath
        smooth               : true
        opacity              : 1

        Column
        {
            id : dateColumnId
            width  : dateRectId.sourceSize.width - 30
            anchors.centerIn : parent
            spacing : 5

            MxText
            {
                id : dateFormatLableId
                color : "white"
                anchors.left : parent.left
                font.bold : true
                font.pointSize : 13
                text: dispStr.readDispStr(DispStr.MSG_SET_DATE_DD_MM_YYYY_537)
            }

            MxText
            {
                id : completeDateTextId
                text : Qt.formatDateTime(new Date(currentYear,currentMonth,currentDate,hours,minutes,seconds), "dd/MM/yyyy" )
                anchors.left : parent.left
                color: "white"
                font.pointSize: 13
            }
        }

        MouseArea
        {
            id : setDateMouseArea
            anchors.fill : parent
            onPressed :
            {
                sendOnlyKeepAlive()

                dateTimeUpdateF = true
                popUpLoader.loadDateTimeSettingScreen()
                popUpLoader.item.updateDateTimeData(dateTimeUpdateF,
                                                          completeDateTextId.text.slice(0,2),
                                                          completeDateTextId.text.slice(3,-5),
                                                          completeDateTextId.text.slice(-4),
                                                          completeTimeTextId.text.slice(0,2),
                                                          completeTimeTextId.text.slice(3,-3),
                                                          completeTimeTextId.text.slice(-2))
            }
        }
    }

    Image
    {
        id                   : timeRectId
        height               : sourceSize.height
        width                : sourceSize.width
        anchors.top          : dateRectId.bottom
        anchors.topMargin    : 10
        anchors.horizontalCenter : parent.horizontalCenter
        source               : imgPath
        smooth               : true
        opacity              : 1

        Column
        {
            id : timeColumnId
            width  : dateRectId.sourceSize.width - 30
            anchors.centerIn : parent
            spacing : 5

            MxText
            {
                id : timeFormatLableId
                color : "white"
                anchors.left : parent.left
                font.bold : true
                font.pointSize : 13
                text: dispStr.readDispStr(DispStr.MSG_SET_TIME_HH_MM_SS_542)   /*Set Time(HH:MM:SS*/
            }

            MxText
            {
                id : completeTimeTextId
                text : Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                              hours,minutes,seconds), "hh:mm:ss" )
                anchors.left : parent.left
                color: "white"
                font.pointSize: 13
            }
        }

        MouseArea
        {
            id : setTimeMouseArea
            anchors.fill : parent
            onPressed :
            {
                sendOnlyKeepAlive()

                dateTimeUpdateF = false
                popUpLoader.loadDateTimeSettingScreen()
                popUpLoader.item.updateDateTimeData(dateTimeUpdateF,
                                                          completeDateTextId.text.slice(0,2),
                                                          completeDateTextId.text.slice(3,-5),
                                                          completeDateTextId.text.slice(-4),
                                                          completeTimeTextId.text.slice(0,2),
                                                          completeTimeTextId.text.slice(3,-3),
                                                          completeTimeTextId.text.slice(-2))
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
