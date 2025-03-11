import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : enrollSplRect
    width: lcd_width
    height: lcd_height

    property int configuredNoCard : 1
    property int maxCardCnt       : 1
    //spacingRelated
    property int topMarginEnrollSplFnc  : 65

    OpacityRectangle
    {

    }

    PageHeading
    {
        id : heading
        title:dispStr.readDispStr(DispStr.MSG_ENROLL_SPECIAL_FUNCTION_CARDS_193) /*"Enroll Special Function Card"*/
    }

    Item
    {
        id : enrollSplFunItem
        height : 350
        width : 230
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : parent.top
        anchors.topMargin : topMarginEnrollSplFnc

        GridView
        {
            id                      : enrollSplFuncGrid
            width                   : 230                     //total two element in row
            height                  : 350
            anchors.top             : parent.top
            anchors.centerIn        : parent

            cellWidth               : 100                 //width of each cell
            cellHeight              : 110                //height of each cell

            model : EnrollSplFuncModel{}

            //TODO
            //maximumFlickVelocity: 1500

            snapMode                : GridView.SnapToRow
            boundsBehavior          : Flickable.StopAtBounds
            clip                    : true
            cacheBuffer             : flickableCacheBuffer
            pressDelay              : flickablePressDelay
            smooth                  : true
            flickDeceleration       : flickableFlickDeceleration

            delegate :
            Column
            {
                spacing : 30
                Item
                {
                    id      : iconRect
                    height  : 110               //as per above
                    width   : 100

                    MxIcon
                    {
                        id : iconElement
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        iconIdx: Idx
                        onClicked:
                        {
                            sendOnlyKeepAlive()
                            popUpLoader.loadEnrollSplFunctionScreen()
                            popUpLoader.item.processDeviceResponse(index, configuredNoCard, maxCardCnt)
                        }
                    }
                }
            }

            onMovementStarted:
            {
                sendOnlyKeepAlive()
            }
        }
        ScrollBar
        {
            id : scrollbar
            noOfElements        : 24
            visibleElements     : 6
            position            : enrollSplFuncGrid.visibleArea.yPosition
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {

        //Parse Paylod
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
        configuredNoCard = parseInt(tmpPayload[0],10)
        maxCardCnt = parseInt(tmpPayload[1],10)

    }
 }
