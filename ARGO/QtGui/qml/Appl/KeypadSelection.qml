import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : keypadSelectionId
    height : lcd_height
    width :  lcd_width

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :dispStr.readDispStr(DispStr.MSG_SELECT_KEYPAD_519)  /*"Select Keypad"*/
    }

    VisualItemModel
    {
        id: itemModel

        Item
        {
            id : page1
            height : quartykeypadImg.sourceSize.height + quartykeypadText.height + 10
            width  : lcd_width

            MxText
            {
                id : quartykeypadText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom : parent.top
                font.pointSize: 13
                text : dispStr.readDispStr(DispStr.MSG_QWERTY_KEYPAD_479)  /*"Qwerty Keypad"*/
                color : "white"
            }

            Image
            {
                id : quartykeypadImg
                anchors.top : quartykeypadText.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                source: imageLocation + "/Others/qwertyKeypad.png"
            }
        }

        Item
        {
            id : page2
            height : dialpadImg.sourceSize.height + dialpadText.height + 10
            width  : lcd_width

            MxText
            {
                id : dialpadText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom : parent.top
                font.pointSize: 13
                text : dispStr.readDispStr(DispStr.MSG_DIAL_PAD_137) /*"Dialpad"*/
                color : "white"
            }

            Image
            {
                id : dialpadImg
                anchors.top : dialpadText.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                source: imageLocation + "/Others/dialpad.png"
            }
        }
    }

    ListView
    {
        id                      : view
        height                  : 150
        width                   : lcd_width
        anchors.top             : parent.top
        anchors.topMargin       : topMarginDisplaySetting

        model                   : itemModel

        preferredHighlightBegin : 0
        preferredHighlightEnd   : 0
        highlightRangeMode      : ListView.StrictlyEnforceRange
        orientation             : ListView.Horizontal
        currentIndex            : keypadOption

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
        anchors.bottomMargin: 100

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
        buttonCenterVisibleF: true
		isInImageFormat:true
        centerBtnImagePath:  imageLocation + "/ButtonIcons/save.png"
        //buttonCenterText: "Save"
        onButtonCenterPress:
        {
            keypadOption = view.currentIndex
            appData.data = EnumClass.MSG_KEYPAD_SET_EVENT +
                    String.fromCharCode(uiMsgFsp).toString() +
                    view.currentIndex.toString() +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }
}
