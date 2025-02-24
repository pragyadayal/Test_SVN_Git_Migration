import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      :   aboutDevicePage
    height  :   lcd_height
    width   :   lcd_width

    property variant labelName          : [""]
    property variant labelValue         : [""]
    property string  title              : ""
    property variant isExpandable       : [false]

    signal elementClicked(string label)

    PageHeading
    {
        title: aboutDevicePage.title
    }

    Component
    {
        id          : deviceDelegate

        Item
        {
            id      : deviceItem
            width   : aboutDevicePage.width
            height  : 43                                 // Height of Each Row of profile

            AboutDeviceElement
            {
                label : LabelName
                value : LabelValue
                isExpandable: IsExpandable
                anchors.horizontalCenter: parent.horizontalCenter
                onElementClicked:
                {
                    aboutDevicePage.elementClicked(label)
                }
            }

            Component.onCompleted:
            {
                deviceList.contentWidth   = aboutDevicePage.width
                deviceList.contentHeight += deviceItem.height * labelName.length
            }
        }
    }

    ListModel
    {
        id  : deviceModel
    }

    ListView
    {
        id                      : deviceList
        height                  : 344                  //43 * 8 = 215  we show five items each of 35 height
        width                   : parent.width
        anchors.top             : parent.top
        anchors.topMargin       : 60

        model                   : deviceModel
        delegate                : deviceDelegate

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
            id : scroll
            noOfElements        : labelName.length
            visibleElements     : 8
            position            : deviceList.visibleArea.yPosition
        }
    }

    function fillList ()
    {
        deviceModel.clear()
        for(var index = 0; (index < labelName.length); index++)
        {
            deviceModel.insert (index,
                               {
                                   "LabelName"    :labelName[index] ,
                                   "LabelValue"   :labelValue[index],
                                   "IsExpandable" :isExpandable[index]
                               })
        }
    }
}
