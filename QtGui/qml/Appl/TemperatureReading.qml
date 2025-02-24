import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: temperatureReadingItem
    width: lcd_width
    height: lcd_height

    property int marginBwRectAndSideEdge
    property int marginBwRectandTopEdge
    property int fontSizeTempHeading        : 16

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_TEMPERATURE_READING_987) /*"Settings"*/
    }

    Rectangle
    {
        id                          : temperatureReadingRect
        width                       : lcd_width - (2 * marginBwRectAndSideEdge)
        height                      : lcd_height - (2 * marginBwRectandTopEdge)
        anchors.verticalCenter      : parent.verticalCenter
        anchors.horizontalCenter    : parent.horizontalCenter
        opacity                     : 0.2
        radius                      : 4
        color                       : "black"
        border.color                : "black"
        border.width                : 2
    }

    Column
    {
        id                          : temperatureReadingCol
        spacing                     : 8
        anchors.verticalCenter      : parent.verticalCenter
        anchors.horizontalCenter    : parent.horizontalCenter

        MxText
        {
            id      : currentTemperatureText
            color   : "white"
            smooth  : true
            text    : dispStr.readDispStr(DispStr.MSG_CURRENT_TEMPERATURE_988)
            font.pointSize : fontSizeTempHeading
            anchors.left : parent.left
            anchors.leftMargin: 20
        }

        Image
        {
            id : currentTempValueTextBox
            source: imageLocation + "/Controls/TextEditBox.png"
            height  : 30
            width   : 200
            fillMode: Image.PreserveAspectFit

            MxTextInput
            {
                id          : currentTemperatureValue
                readOnly    : true
                smooth      : true
                color       : "white"
                font.pointSize : fontSizeTempHeading
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row
        {
            id: ambientTemperatureRow

            MxText
            {
                id: ambientTemperatureText
                text: dispStr.readDispStr(DispStr.MSG_AMBIENT_TEMPERATURE_989)
                anchors.left: parent.left
                color   : "white"
                smooth  : true
                font.pointSize : fontSizeTempHeading
                anchors.leftMargin: 20
            }

            Image
            {
                id: refreshImage
                fillMode: Image.PreserveAspectFit
                smooth: true
                source: imageLocation + "/Others/RefreshIcon.png"
                anchors.left: ambientTemperatureText.right
                anchors.leftMargin: 10

                MouseArea
                {
                    id: refreshImageMouseArea
                    anchors.fill: parent

                    onClicked:
                    {
                        appData.data = EnumClass.GUI_MSG_TEMPERATURE_REFRESH +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    }
                }
            }
        }

        Image
        {
            id : ambientTempValueTextBox
            source: imageLocation + "/Controls/TextEditBox.png"
            height  : 30
            width   : 200
            fillMode: Image.PreserveAspectFit

            MxTextInput
            {
                id: ambientTemperatureValue
                readOnly: true
                smooth      : true
                color   : "white"
                font.pointSize : fontSizeTempHeading
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MxText
        {
            id              : lastUpdatedTime
            font.pointSize  : 12
            text            : dispStr.readDispStr(DispStr.MSG_LAST_UPDATE_ON_992)
            color           : "white"
            smooth          : true
        }
    }

    Component.onCompleted:
    {
        if(deviceType == EnumClass.DEVICE_TYPE_VEGA)
        {
            // Change after verifying in the device
            marginBwRectAndSideEdge = 10
            marginBwRectandTopEdge = 60
        }
        else if(deviceType == EnumClass.DEVICE_TYPE_ARGO)
        {
            // Change after verifying in the device
            marginBwRectAndSideEdge = 15
            marginBwRectandTopEdge = 75
        }
        else
        {
            // Change after verifying in the device
            marginBwRectAndSideEdge = 15
            marginBwRectandTopEdge = 80
        }
    }

    function processDeviceResponse(msgId, rcvData)
    {
        /* Reset old values */
        currentTemperatureValue.text = ""
        ambientTemperatureValue.text = ""
        lastUpdatedTime.text = dispStr.readDispStr(DispStr.MSG_LAST_UPDATE_ON_992)

        var tmpPayLoad = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        currentTemperatureValue.text = tmpPayLoad[0]
        ambientTemperatureValue.text = tmpPayLoad[1]
        lastUpdatedTime.text += tmpPayLoad[2]
    }
}
