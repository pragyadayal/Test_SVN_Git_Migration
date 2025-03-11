import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id: selectionPopUpRectId
    width: lcd_width
    height: lcd_height

    property alias boxHeading: heading.text
    property int selectedValue
    property variant selectionIconIdx : [0,0,0,0]
    property variant selectionList
    property string selectionBoxIdentity

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        property int frameHeight : 370
        property int frameWidth : 200
        id : popUpRectId
        height : frameHeight
        width : frameWidth
        anchors.centerIn: parent

        MxText
        {
            id: heading
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 5
            font.pointSize: fontSizeTextLabel
            color: mxColorBlue
            font.bold: true
        }

        Column
        {
            id : gridColumn
            property int spaceBetIcons : 15
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20

            spacing : spaceBetIcons
            Repeater
            {
                id : columnRepeater
                anchors.horizontalCenter: parent.horizontalCenter
                model : 3
                Row
                {
                    id: first
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater
                    {
                        id : rowRepeater
                        model:2
                        property int rowIdx : index

                        Item
                        {
                            property int itemHeight : 90
                            property int itemWidth : 90
                            id : temp
                            height: itemHeight
                            width : itemWidth

                            MxIcon
                            {
                                id : iconElement
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.top: parent.top
                                property int currIdx  : ((rowRepeater.rowIdx * 2) + index )

                                iconIdx: selectionPopUpRectId.selectionIconIdx[currIdx]

                                onClicked:
                                {
                                    selectionDone(currIdx)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    function selectionDone(choiceIdx)
    {
        selectedValue = choiceIdx
        mainLoader.item.selectionDone(selectionBoxIdentity,selectedValue)
        popUpLoader.unloadPopUp()
    }

    function processDeviceResponse(title,identity,options,iconIdxList)
    {
        boxHeading = title
        selectionList = options
        selectionBoxIdentity = identity
        selectionIconIdx = iconIdxList
    }
}
