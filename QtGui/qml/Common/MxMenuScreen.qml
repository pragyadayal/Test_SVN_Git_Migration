import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id : mainGrid
    width: lcd_width
    height: 350

    anchors.top : parent.top
    anchors.topMargin: 60

    color: "transparent"

    property variant iconIdxList

    property int iconGridHeight : 350
    property int noOfIcon : iconIdxList.length
    property variant iconEnableList : []

    property int maxIconPerPage : 9
    property int maxIconPerRow  : 3
    property int columnRepeatCnt : 3
    property int spaceBetIcons : 15

    signal iconPress(int idx)
    signal indicatorPress(int pageIdx)

    VisualItemModel
    {
        id: itemModel
        property variant iconIdx : iconIdxList.concat(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
        property variant iconEnable : iconEnableList.concat(true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true)

        Item
        {
            height  : page1.height
            width   : page1.width
            clip    : true
            MxIconGrid
            {
                id : page1

                noOfIconOnPage: noOfIcon < maxIconPerPage ? noOfIcon : maxIconPerPage
                noOfIconOnRow : maxIconPerRow
                iconIdxList : itemModel.iconIdx.slice(0,maxIconPerPage)
                iconEnableList: itemModel.iconEnable.slice(0,maxIconPerPage)
                onIconClick:
                {
                    iconPress(idx)
                }
            }
        }

        Item
        {
            height  : page2.height
            width   : page2.width
            clip    :  true

            MxIconGrid
            {
                id : page2
                noOfIconOnPage: noOfIcon < maxIconPerPage ? 0 : noOfIcon - maxIconPerPage
                noOfIconOnRow : maxIconPerRow
                iconIdxList :  itemModel.iconIdx.slice(maxIconPerPage,2*maxIconPerPage)
                iconEnableList: itemModel.iconEnable.slice(maxIconPerPage,2*maxIconPerPage)
                onIconClick:
                {
                    iconPress(idx)
                }
            }
        }
    }

    ListView
    {
        id                      : view
        width                   : lcd_width
        height                  : 350
        anchors.top             : parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        model                   : itemModel

        interactive             : (noOfIcon > maxIconPerPage) ? true : false
        preferredHighlightBegin : 0
        preferredHighlightEnd   : 0
        highlightRangeMode      : ListView.StrictlyEnforceRange
        orientation             : ListView.Horizontal

        snapMode                : ListView.SnapToItem
        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        cacheBuffer             : flickableCacheBuffer
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
            sendOnlyKeepAlive()
        }

        onMovementEnded:
        {
            if(noOfIcon <= maxIconPerPage)
            {
                currentIndex = 0
            }
        }
    }

    onIndicatorPress:
    {
        view.currentIndex = pageIdx
    }

    Item
    {
        id : pageIndicator
        height : 25
        width : lcd_width
        anchors.bottom: parent.bottom
        visible : (noOfIcon > maxIconPerPage) ? true : false

        Row
        {
            anchors.centerIn: parent
            spacing : 15
            Repeater
            {
                model : (noOfIcon > maxIconPerPage) ? 2 : 1
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
}
