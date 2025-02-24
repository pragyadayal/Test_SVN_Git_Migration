import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: backGround
    width  : lcd_width
    height : lcd_height

    property int previousIndex : wallpaperIndex
    property int previewIndex  : wallpaperIndex

    property int dur : 4

    signal localWallPaperChange(int idx)
    signal indicatorPress(int pageIdx)

    property int wallspacing    : 15
    property int image_height   : 60
    property int image_width    : 60

    onLocalWallPaperChange:
    {
        previousIndex = previewIndex
        previewIndex = idx
        sendOnlyKeepAlive()
    }

    onPreviewIndexChanged:
    {
        previewBackground.opacity = 1
    }

    Image
    {
        anchors.fill : parent
        source :appData.checkImage( allWallpaper[previousIndex])
                ? appData.createImagePath(allWallpaper[previousIndex])
                : imageLocation + "/Others/DefaultWallpaper.jpg"
        cache : false
    }

    Image
    {
        id : previewBackground
        anchors.fill : parent
        source :appData.checkImage( allWallpaper[previewIndex])
                ? appData.createImagePath(allWallpaper[previewIndex])
                : imageLocation + "/Others/DefaultWallpaper.jpg"
        cache : false

        Behavior on opacity
        {
            NumberAnimation{ duration: dur * 100; from : 0}
        }
    }

    PageHeading
    {
        title :  dispStr.readDispStr(DispStr.MSG_SET_WALLPAPER_545) /*"Set Wallpapers"*/
    }

    VisualItemModel
    {
        id: itemModel

        Item
        {
            height  : page1.height
            width   : lcd_width

            Grid
            {
                id : page1
                rows    : 4
                columns : 3
                spacing : 15
                anchors.centerIn: parent

                Repeater
                {
                    model: 12
                    Image
                    {
                        id: clm1
                        source: appData.checkImage(allWallpaper[index])
                                ? appData.createImagePath(allWallpaper[index])
                                : ""
                        cache : false
                        height:image_height
                        width: image_width
                        smooth: true
                        Rectangle
                        {
                          id:wallrect1
                          color: "transparent"
                          anchors.fill: parent
                          border.color:mouse1.pressed?"blue":"white"
                          border.width: 2
                          visible: (clm1.status == Image.Ready) ? true : false
                          radius: 2
                        }
                        MouseArea
                        {
                            id:mouse1
                            anchors.fill: parent
                            onClicked:
                            {
                                if(clm1.status == Image.Ready)
                                {
                                    localWallPaperChange(index)
                                }
                            }
                        }
                    }
                }
            }
        }

//        Item
//        {
//            height  : page2.height
//            width   : lcd_width

//            Grid
//            {
//                id : page2
//                rows    : 2
//                columns : 3
//                spacing : 15
//                anchors.centerIn: parent

//                Repeater
//                {
//                    model: 6
//                    Image
//                    {
//                        id: clm2
//                        source: appData.checkImage(allWallpaper[(index + 6)])
//                                ? appData.createImagePath(allWallpaper[(index + 6)])
//                                : ""
//                        cache : false
//                        height:image_height
//                        width: image_width
//                        smooth: true
//                        Rectangle
//                        {
//                          id:wallrect2
//                          color: "transparent"
//                          anchors.fill: parent
//                          border.color:mouse2.pressed?"blue":"white"
//                          border.width: 2
//                          visible: (clm2.status == Image.Ready) ? true : false
//                          radius: 2
//                        }
//                        MouseArea
//                        {
//                            id:mouse2
//                            anchors.fill: parent
//                            onClicked:
//                            {
//                                if(clm2.status == Image.Ready)
//                                {
//                                    localWallPaperChange(index + 6)
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
    }

    ListView
    {
        id                      : view
        width                   : lcd_width
        height                  : 290
        anchors.top             : parent.top
        anchors.topMargin       : topMarginDisplaySetting

        model                   : itemModel

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
    }

    onIndicatorPress:
    {
        view.currentIndex = pageIdx
    }

//    Item
//    {
//        id : pageIndicator
//        height : 25
//        width : lcd_width
//        anchors.top: view.bottom
//        anchors.topMargin: 5

//        Row
//        {
//            anchors.centerIn: parent
//            spacing : 15
//            Repeater
//            {
//                model : 2
//                Rectangle
//                {
//                    id : indicatorRect
//                    height : (view.currentIndex == index) ? 10 : 8
//                    width : (view.currentIndex == index) ? 10 : 8
//                    radius: 4
//                    color : mxColorBlue

//                    border.color : mxColorBlue
//                    border.width : (view.currentIndex == index) ? 2 : 0

//                    MouseArea
//                    {
//                        height: parent.height+10
//                        width : parent.width+10
//                        anchors.centerIn: parent

//                        onPressed:
//                        {
//                            indicatorPress(index)
//                        }
//                    }
//                }
//            }
//        }
//    }

    ButtonElement
    {
        id : buttons
        buttonCenterVisibleF: true
		isInImageFormat:true
        centerBtnImagePath: imageLocation + "/ButtonIcons/save.png"

        //buttonCenterText: "Save"
        onButtonCenterPress:
        {
            var tempBackGround
            wallpaperIndex = previewIndex
            tempBackGround = wallpaperIndex.toString() +
                    String.fromCharCode(uiMsgFsp).toString()

            appData.data = EnumClass.MSG_BACKGROUND_SET_EVENT +
                    String.fromCharCode(uiMsgFsp).toString() +
                    tempBackGround +
                    String.fromCharCode(eotFsp).toString()
        }
    }
}

