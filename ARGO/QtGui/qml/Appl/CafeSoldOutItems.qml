import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id     : itemAvailable
    width  : lcd_width
    height : lcd_height

    property string  imgPath          : imageLocation + "/Controls/"
    property int     numberOfItems    : 0
    property int     numberOfRect     : 0
    property variant itemName         : [""]
    property variant itemSoldOutFlag  : [""]
    property variant itemNumber       : [""]
    property variant itemCode         : [""]
    property variant selectedItem     : [ ]
    property variant itemPreserveFlag
    property variant itemPrice

    property int defltItemEnbl     : 0
    property int totalShownItems   : 0
    property int pageBtnRowHeight  : 30

    property int cafeMenuPage      : 0
    property int currentPgIndex    : 0
    property int idx       		   : 0


    OpacityRectangle
    {

    }

    Image
    {
        id                : titleBar
        anchors.top       : parent.top
        anchors.topMargin : topMarginCafe
        anchors.left      : parent.left
        width             : lcd_width
        height            : 35
        source            : imageLocation + "/Controls/CafeHeaderBarBase.png"

        MxText
        {
            id                 : pageTitle
            color              : mxColorBlue
            anchors.centerIn   : parent
            text               : dispStr.readDispStr(DispStr.MSG_LIST_OF_ITEMS_321) /*"List of Items"*/
            font.pointSize     : fontSizeCafeListTitle
        }
    }

    Component
    {
        id : soldlistDelegate

        Item
        {
            id              : compItem
            height          : 35
            width           : lcd_width
            anchors.right   : parent.right

            Item
            {
                anchors.verticalCenter  : parent.verticalCenter
                anchors.left            : parent.left
                anchors.right           : parent.right
                width                   : parent.width

                MxText
                {
                    id                      : itemNoText
                    anchors.verticalCenter  : parent.verticalCenter
                    width                   : 20
                    clip                    : true
                    text                    : (ITEMNUMBER + 1).toString()
                    font.pointSize          : fontSizeProfileInfo
                    color                   : "white"
                    horizontalAlignment     : Text.AlignRight
                    anchors.left            : parent.left
                    anchors.leftMargin      : 8
                }

                MxText
                {
                    id                     : itemtext
                    anchors.verticalCenter : parent.verticalCenter
                    width                  : 155
                    clip                   : true
                    text                   : ITEMNAME
                    font.pointSize         : fontSizeProfileInfo
                    color                  : "white"
                    anchors.left           : itemNoText.right
                    anchors.leftMargin     : 8
                }

                Image
                {
                    id                        : checkboxImg
                    anchors.verticalCenter    : parent.verticalCenter
                    anchors.right             : parent.right
                    anchors.rightMargin       : 15
                    source                    : ( imgPath+ "CheckBox"+ ( (SOLDOUT == false) ? "" : "Selected") + ".png")
                }
            }

            Seperator
            {

            }

            MouseArea
            {
                id           : mouse1
                anchors.fill : parent
                onClicked:
                {
                    sendOnlyKeepAlive()
                    var tmpSelectedItem = new Array()
                    if (SOLDOUT == false)
                    {
                        soldOutModel.setProperty(index,"SOLDOUT", true)
                        numberOfRect++
                        tmpSelectedItem = selectedItem
                        tmpSelectedItem.push(soldOutModel.get(index).ITEMNUMBER)
                    }
                    else
                    {
                        soldOutModel.setProperty(index,"SOLDOUT", false)
                        tmpSelectedItem = selectedItem
                        if(numberOfRect > 0)
                        {
                           for(var tmpCnt = 0; tmpCnt < selectedItem.length; tmpCnt++)
                           {
                               if(tmpSelectedItem[tmpCnt] == (soldOutModel.get(index).ITEMNUMBER) )
                               {
                                    tmpSelectedItem.splice(tmpCnt, 1)
                               }
                           }
                           numberOfRect--
                        }
                    }
                    selectedItem = tmpSelectedItem
                 }
            }

            Component.onCompleted:
            {
                soldOutList.contentWidth  = parent.width
                soldOutList.contentHeight = compItem.height * numberOfItems
            }
        }
    }

    ListModel
    {
        id : soldOutModel
    }

    ListView
    {
        id                      : soldOutList
        height                  : 35 * 9        // 9 items each of height 35
        width                   : lcd_width-10
        anchors.top             : titleBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        model                   : soldOutModel
        delegate                : soldlistDelegate

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

        ScrollBar
        {
            id                 : scrollbar
            position           : soldOutList.visibleArea.yPosition
            visibleElements    : 9
            noOfElements       : totalShownItems
        }
    }

    Component.onCompleted:
    {
        soldOutModel.clear()
    }

    Item
    {
        id: paginationRow
        width : parent.width
        height : pageBtnRowHeight
        anchors.top: soldOutList.bottom
        anchors.topMargin: 10

        Image
        {
            id: previousBtn
            height: parent.height
            width: height
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            source: imageLocation + "/Controls/Left.png"

            MouseArea
            {
                id: mousePreviousBtn
                anchors.fill: parent
                onClicked:
                {
                    //Display Previous
                    getPreviousCafeMenuList()
                }
            }
        }

        Item
        {
            height                   : 40
            width                    : 175
            anchors.horizontalCenter : parent.horizontalCenter
            anchors.verticalCenter   : parent.verticalCenter

            Image
            {
                id                       : soldOutButton
                width                    : soldOutArea.pressed ? soldOutButton.sourceSize.width * 0.9 : soldOutButton.sourceSize.width
                height                   : soldOutArea.pressed ? soldOutButton.sourceSize.height * 0.9 : soldOutButton.sourceSize.height
                source                   : imgPath + "LargeButton.png"
                anchors.centerIn         : parent
                opacity                  : (numberOfRect == 0) ? 0.25  : 1
                MxText
                {
                    id               : soldOutText
                    anchors.centerIn : parent
                    text             : dispStr.readDispStr(DispStr.MSG_MARK_AS_SOLD_OUT_335) /*"Mark as Sold Out"*/
                    font.pointSize   : soldOutArea.pressed ? fontSizeCafeButton * 0.9 : fontSizeCafeButton
                    color            : "white"
                }

                MouseArea
                {
                    id           : soldOutArea
                    anchors.fill : parent
                    enabled      : (numberOfRect == 0) ? false : true
                    onClicked:
                    {
                        sendOnlyKeepAlive()
                        generateSoldOutData()
                    }
                }
            }
        }

        Image
        {
            id: nextBtn
            height: parent.height
            width: height
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            source: imageLocation + "/Controls/Right.png"

            MouseArea
            {
                id: mouseNeBtnBtn
                anchors.fill: parent
                onClicked:
                {
                    //Display Next
                    getNextCafeMenuList()
                }
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        itemPreserveFlag = itemSoldOutFlag
        sendOnlyKeepAlive()
        fillSoldOutList()
    }

    function fillSoldOutList()
    {
        totalShownItems = 0
        soldOutModel.clear()
        for(var index=0 ; (index < numberOfItems ) ; index++)
        {

            if (itemPreserveFlag[index] == false)
            {
                soldOutModel.insert(totalShownItems,
                                    {
                                        "ITEMNUMBER" : itemNumber[index],
                                        "ITEMNAME": itemName[index],
                                        "SOLDOUT" : itemSoldOutFlag[index],
                                        "ITEMCODE"   : itemCode[index]
                                    })

                if(selectedItem !== undefined)
                {
                    for(var tmpCnt = 0; tmpCnt < selectedItem.length; tmpCnt++)
                    {
                        if(selectedItem[tmpCnt] == (itemNumber[index]) )
                        {
                           soldOutModel.setProperty(totalShownItems,"SOLDOUT", true)
                        }
                    }
                }
                totalShownItems++
            }
        }
    }


    function getNextCafeMenuList()
    {
       appData.data =  EnumClass.GUI_BUTTON_NEXT_CAFE_MENU_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function getPreviousCafeMenuList()
    {
       appData.data =  EnumClass.GUI_BUTTON_PREVIOUS_CAFE_MENU_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function generateSoldOutData()
    {
        var tempSelectedItems = 0
        var tempItemData = ""
        var msgBuf

        for(var itemIndex = 0; itemIndex < selectedItem.length; itemIndex++)
        {

            tempItemData  += selectedItem[itemIndex].toString()
                    + String.fromCharCode(uiMsgFsp).toString()

            tempSelectedItems++
        }

        msgBuf =  EnumClass.MSG_CAFE_SOLDOUT_VALIDATION_EVENT
                + String.fromCharCode(uiMsgFsp).toString()
                + selectedItem.length
                + String.fromCharCode(uiMsgFsp).toString()
                + tempItemData
                + String.fromCharCode(eotFsp).toString()

        //console.log(msgBuf)

        appData.data = msgBuf
    }
}
