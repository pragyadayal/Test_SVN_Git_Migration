import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : clockSelectionId
    height: lcd_height
    width :  lcd_width

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :dispStr.readDispStr(DispStr.MSG_SELECT_CLOCK_510) /*"Select Clock"*/
    }

    VisualItemModel
    {
        id: itemModel

        Item
        {
            id     : page1
            height : 160        //10+10+140 text+spacing+clockimage
            width  : lcd_width

            MxText
            {
                id : analogClockText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom : parent.top
                font.pointSize: 13
                text : dispStr.readDispStr(DispStr.MSG_ANALOG_CLOCK_45) /*"Analog Clock"*/
                color : "white"
            }

            Item
            {
                id : analogclockRect
                width   :lcd_width
                height  :140
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top : parent.top
                anchors.topMargin: 10

                AnalogClock
                {
                    id : analogClock
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Item
        {
            id : page2
            height : 140
            width  : lcd_width

            MxText
            {
                id : digitalClockText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom : parent.top
                font.pointSize: 13
                text : dispStr.readDispStr(DispStr.MSG_DIGITAL_CLOCK_138) /*"Digital Clock"*/
                color : "white"
            }

            Item
            {
                id : digitalClockRect
                width   :lcd_width
                height  :120
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top : parent.top
                anchors.topMargin: 10

                DigitalClock
                {
                    id : digital
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    ListView
    {
        id                      : view
        height                  : 170            //VisualItemModel + 10
        width                   : lcd_width
        anchors.top             : parent.top
        anchors.topMargin       : topMarginDisplaySetting

        model                   : itemModel

        preferredHighlightBegin : 0
        preferredHighlightEnd   : 0
        highlightRangeMode      : ListView.StrictlyEnforceRange
        orientation             : ListView.Horizontal
        currentIndex            : clockIndex

        snapMode                : ListView.SnapToItem
        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        cacheBuffer             : flickableCacheBuffer
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration
    }

    signal indicatorPress(int idx)

    onIndicatorPress:
    {
        sendOnlyKeepAlive()
        view.currentIndex = idx
    }

    Item
    {
        id : pageIndicator
        height : 25
        width : 320
        anchors.bottom: buttons.top
        anchors.bottomMargin: 80

        Row
        {
            anchors.centerIn: parent
            spacing : 15
            Repeater
            {
                model : 2
                Rectangle
                {
                    id : indicatorRect
                    height : (view.currentIndex == index) ? 10 : 8
                    width : (view.currentIndex == index) ? 10 : 8
                    radius: 4
                    color : mxColorBlue

                    border.color : mxColorBlue
                    border.width : (view.currentIndex == index) ? 2 : 0

                    MouseArea
                    {
                        height: parent.height+10
                        width : parent.width+10
                        anchors.centerIn: parent

                        onPressed:
                        {
                            indicatorPress(index)
                        }
                    }
                }
            }
        }
    }

    ButtonElement
    {
        id : buttons
        isInImageFormat:true
        centerBtnImagePath: imageLocation + "/ButtonIcons/save.png"

        buttonCenterVisibleF: true

        //buttonCenterText: "Save"
        onButtonCenterPress:
        {
            clockIndex = view.currentIndex
            appData.data = EnumClass.MSG_CLOCK_SET_EVENT +
                    String.fromCharCode(uiMsgFsp).toString() +
                    clockIndex.toString() +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }
}
