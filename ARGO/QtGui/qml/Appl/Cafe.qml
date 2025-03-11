import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : cafeMenuMainRect
    width: lcd_width
    height: lcd_height

    property int     indexSelected   : 255
    property int     numberOfItems   : 0

    property variant itemName        : [""]
    property variant itemPrice       : [""]
    property variant itemNumber      : [""]
    property variant itemCode        : [""]
    property variant itemSoldOutFlag : [""]
    property variant itemQty         : [""]
    property variant itemAmount      : [""]

    property int  defltItemEnbl      : 0
    property bool valUpdate          : false
    property int finalTotal          : 0
    property string finalDecTotal    : ""
    property string finalFracTotal   : ""
    property string dfltAmount       : "00.00"
    property int pageBtnRowHeight    : 30

    property int cafeMenuPage        : 0
    property int currentPgIndex      : 0

    OpacityRectangle
    {

    }

    Image
    {
        id                : titleBar
        anchors.top       : parent.top
        anchors.topMargin : topMarginCafe
        source            : imageLocation + "/Controls/CafeHeaderBarBase.png"

        MxText
        {
            id                 : itemTitle
            color              : mxColorBlue
            anchors.left       : parent.left
            anchors.leftMargin : 70
            anchors.verticalCenter: parent.verticalCenter
            text               : dispStr.readDispStr(DispStr.MSG_ITEM_301) /*qsTr("Item")*/
            font.pointSize     : fontSizeCafeListTitle
        }

        MxText
        {
            id                 : quantityTitle
            color              : mxColorBlue
            anchors.left       : parent.left
            anchors.leftMargin : 200
            anchors.verticalCenter: parent.verticalCenter
            text               : dispStr.readDispStr(DispStr.MSG_QTY_474) /*qsTr("Qty")*/
            font.pointSize     : fontSizeCafeListTitle
        }

        MxText
        {
            id                 : amountTitle
            color              : mxColorBlue
            anchors.left       : parent.left
            anchors.leftMargin : 250
            anchors.verticalCenter: parent.verticalCenter
            text               : dispStr.readDispStr(DispStr.MSG_AMOUNT_44)  /*qsTr("Amt")*/
            font.pointSize     : fontSizeCafeListTitle
        }
    }

    Component
    {
        id : cafedelegate

        Item
        {
            id     : compItem
            width  : menuBase.sourceSize.width
            height : 60

            Image
            {
                id              : menuBase
                anchors.centerIn:  parent
                source          : imageLocation + "/Controls/CafeItemBase"+
                         ( (QUANTITY > 0) ? "Selected" : ( (SOLDOUT != 0) ? "SoldOut" : (defltItemEnbl && (index == 0) && (finalTotal == 0)) ? "Default" : ""))
                         + ".png"
            }

            MouseArea
            {
                id           : itemMouseArea
                anchors.fill : compItem
                onClicked    :
                {
                    if(SOLDOUT == 0)
                    {
                        cafeMenuMainRect.indexSelected = index

                        if(QUANTITY == 0)
                        {
                            if(spinBoxArea.dispQty < spinBoxArea.topValidation)
                            {
                                spinBoxArea.value = spinBoxArea.dispQty + 1
                            }
                        }
                    }
                    else
                    {
                        sendOnlyKeepAlive()
                        cafeMenuMainRect.indexSelected = 255
                    }
                }
            }

            Row
            {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left

                MxText
                {
                    id                      : itemNoText
                    width                   : 18
                    anchors.verticalCenter  : parent.verticalCenter
                    text                    : (ITEMNUMBER + 1).toString()
                    font.pointSize          : fontSizeCafeListText
                    color                   : "black"
                    horizontalAlignment     : Text.AlignRight
                }

                Column
                {
                    anchors.verticalCenter : parent.verticalCenter
                    width : 169
                    spacing : 4
                    MxText
                    {
                        id                     : itemtext
                        width                  : 115
                        text                   : ITEMNAME
                        font.pointSize         : fontSizeCafeListText
                        color                  : (SOLDOUT == 0) ? "white" : mxColorDullGrey
                        horizontalAlignment    : Text.AlignLeft
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    MxText
                    {
                        id                     : itempricetext
                        width                  : 115
                        text                   : PRICE
                        font.pointSize         : fontSizeCafeListText
                        color                  : (SOLDOUT == 0) ? "white" : mxColorDullGrey
                        horizontalAlignment    : Text.AlignLeft
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                MxText
                {
                    property int  spinDsp  : QUANTITY

                    id                     : quantityofItem
                    width                  : 50
                    anchors.verticalCenter : parent.verticalCenter
                    text                   : (SOLDOUT == 0) ? QUANTITY : ""
                    font.pointSize         : fontSizeCafeListText
                    color                  : "white"
                    horizontalAlignment    : Text.AlignHCenter

                    onSpinDspChanged:
                    {
                        spinBoxArea.listIndx = spinDsp
                        spinBoxArea.dispQty = spinDsp

//                        console.log("spinBoxArea.dispQty "+spinDsp);
                    }

                    CafeSpinBox
                    {
                        z : 1
                        id              : spinBoxArea
                        anchors.centerIn: parent
                        visible         : (index === cafeMenuMainRect.indexSelected ) ? true : false
                        value           : QUANTITY
                        onValueChanged  :
                        {
//                            console.log("Value changed in CafeSpinBox:" + value);
                            cafemodel.setProperty(index, "QUANTITY", value)
                            generateItemSelectionEvent(index);
                        }
                    }
                }

                MxText
                {
                    id                     : currentTotalText
                    width                  : 61
                    anchors.verticalCenter : parent.verticalCenter
                    text                   : ITEMCOSTSTR
                    font.pointSize         : fontSizeCafeListText
                    color                  : "white"
                    horizontalAlignment    : Text.AlignRight
                    visible                : (SOLDOUT == 0) ? true : false
                }
            }

            Component.onCompleted:
            {
                cafelist.width = width
                cafelist.height = (height * 5)
            }
        }
    }

    ListModel
    {
        id:cafemodel
    }

    Item
    {
        id : cafeDisplayRect
        height : cafelist.height
        width : lcd_width
        anchors.verticalCenter: cafelist.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        ScrollBar
        {
            noOfElements            : numberOfItems
            visibleElements         : 5
            position                : cafelist.visibleArea.yPosition
        }
    }

    ListView
    {
        id                      : cafelist

        anchors.top             : titleBar.bottom
        anchors.left            : parent.left

        model                   : cafemodel
        delegate                : cafedelegate

        snapMode                : ListView.SnapToItem
        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        cacheBuffer             : flickableCacheBuffer
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
            indexSelected = 255
            sendOnlyKeepAlive()
        }
    }


   Item
   {
       id: paginationRow
       width : parent.width
       height : pageBtnRowHeight
       anchors.top: cafelist.bottom
       anchors.topMargin: 20

       Image
       {
           id: previousBtn
           height: parent.height
           width: height
           anchors.left: parent.left
           anchors.leftMargin: 12
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

       Row
       {
           height: parent.height
           anchors.verticalCenter: parent.verticalCenter
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: paginationRow.top
           anchors.topMargin:  10
           spacing: 15
           Repeater
           {
               model : cafeMenuPage
               Rectangle
               {
                   height : (currentPgIndex == index) ? 8 : 6
                   width : (currentPgIndex == index) ? 8 : 6
                   radius: 5
                   color : mxColorBlue
                   border.color : mxColorBlue
                   border.width : (currentPgIndex == index) ? 2 : 0
               }
           }
       }

       Image
       {
           id                   : finalTotalImage
           width                : 200
           anchors.top          : parent.top
           anchors.topMargin    : -10
           anchors.horizontalCenter: parent.horizontalCenter
           source               : imageLocation + "/Controls/CafeTotalBase.png"
           visible              : parseInt(finalTotal,10) > 0

           MxText
           {
               id                     : tot
               text                   : dispStr.readDispStr(DispStr.MSG_TOTAL_1079)+ " " + finalFracTotal
               font.pointSize         : fontSizeCafeListText
               anchors.centerIn       : parent
               color                  : "black"
           }
       }


       Image
       {
           id: nextBtn
           height: parent.height
           width: height
           anchors.right: parent.right
           anchors.rightMargin: 12
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

    CafeFooter
    {
        id : footer
        cafeFooterF : startApplF
        clearMenu   : (finalTotal > 0) ? true : false
        payBill     : (finalTotal > 0) ? true : false
        menu        : true
        onPayBillClicked: generateDataPayment()
        onClearMenuClicked:
        {
           fillCafeList()
        }
    }

    Component.onCompleted:
    {
         cafemodel.clear()
    }

    function fillCafeList()
    {
        cafemodel.clear()
        indexSelected = 255
        finalTotal = 0
        finalDecTotal = ""
        finalFracTotal = ""

        cafemodel.clear()
        for(var index=0 ; (index < numberOfItems ) ; index++)
        {
            cafemodel.insert(index,
                             {
                                  "ITEMNUMBER" : itemNumber[index],
                                  "ITEMNAME"   : itemName[index],
                                  "PRICE"      : itemPrice[index],
                                  "SOLDOUT"    : itemSoldOutFlag[index],
                                  "QUANTITY"   : 0,
                                  "ITEMCODE"   : itemCode[index],
								  "ITEMCOSTSTR"  : "0.00",
                                  "ITEMCOST"   : 0,
                             })
        }
    }

    function generateDataPayment()
    {
        var tempDataPayment

        tempDataPayment = EnumClass.GUI_BUTTON_CAFE_BILL_PAY
                       + String.fromCharCode(uiMsgFsp).toString()
                       + String.fromCharCode(eotFsp).toString()

        //console.log(tempDataPayment)
        appData.data = tempDataPayment
    }

    function getNextCafeMenuList()
    {
       appData.data =  EnumClass.GUI_BUTTON_NEXT_MAIN_CAFE_MENU_SCREEN_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function getPreviousCafeMenuList()
    {
       appData.data =  EnumClass.GUI_BUTTON_PREVIOUS_MAIN_CAFE_MENU_SCREEN_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        fillCafeList()
    }

    function processItemSelectionResponse(msgId,rcvData)
    {
         PayloadLib.parsePayload(msgId, rcvData)
    }

    function getItemCost(itemIndex)
    {
        var  intPrice, priceSep

        if(itemIndex !== 255)
        {
            priceSep = cafemodel.get(itemIndex).PRICE.split(".")
            intPrice = (parseInt(priceSep[0], 10) * 100) + parseInt(priceSep[1], 10)
            cafemodel.get(itemIndex).ITEMCOST = parseInt(cafemodel.get(itemIndex).QUANTITY, 10) * intPrice

            intPrice = cafemodel.get(itemIndex).ITEMCOST / 100
            cafemodel.get(itemIndex).ITEMCOSTSTR = parseInt(intPrice, 10).toString()
            cafemodel.get(itemIndex).ITEMCOSTSTR += "."
            intPrice = cafemodel.get(itemIndex).ITEMCOST % 100
            if(intPrice <= 9)
            {
                cafemodel.get(itemIndex).ITEMCOSTSTR += "0"
            }
            cafemodel.get(itemIndex).ITEMCOSTSTR += intPrice.toString()
        }
    }

    function generateItemSelectionEvent(itemIndex)
    {
        var tempItemData = ""

        if(parseInt(cafemodel.get(itemIndex).QUANTITY, 10) > 0)
        {
           var tempQuantity = parseInt(cafemodel.get(itemIndex).QUANTITY, 10)

        }

        tempItemData = EnumClass.MSG_CAFE_ITEM_SELECTION_EVENT
                     + String.fromCharCode(uiMsgFsp).toString()
                     + cafemodel.get(itemIndex).ITEMNUMBER
                     + String.fromCharCode(deshFsp).toString()
                     + tempQuantity
                     + String.fromCharCode(deshFsp).toString()
                     + String.fromCharCode(uiMsgFsp).toString()
                     + String.fromCharCode(eotFsp).toString()

        console.log("tempQuantity="+tempQuantity)

        appData.data = tempItemData
    }

    function updateMenuItem(itmIdx)
    {
        cafemodel.set(itmIdx, {
                              "QUANTITY"    : itemQty[itmIdx],
                              "ITEMCOSTSTR" : itemAmount[itmIdx],
                         })
    }
}
