import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Rectangle
{
    id : mainGrid
    width: 320
    height: 225

    anchors.top : parent.top
    anchors.topMargin: 50

    color: " transparent"

    property variant iconIdxList

    property variant iconEnableList : []

    signal iconPress(int idx)

    Column
    {
        id : mainRow
        anchors.centerIn: parent
        property int spaceBetIconsRow : 25/*20*/
        spacing : spaceBetIconsRow

        Row
        {
            id : gridColumn
            property int spaceBetIcons : 25
            anchors.horizontalCenter: parent.horizontalCenter
            spacing : spaceBetIcons
            Repeater
            {
                id : columnRepeater
                anchors.verticalCenter: parent.verticalCenter
                model : 2

                Item
                {
                    id : temp
                    height: 90
                    width : 80

                    MxIcon
                    {
                        id : iconElement
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        property int currIdx  : index
                        iconIdx: iconIdxList[currIdx]
                        iconEnableF: mainGrid.iconEnableList[currIdx]

                        onClicked:
                        {
                            iconPress(idx)
                        }
                    }
                }
            }
        }

        Row
        {
            id : gridColumn2
            property int spaceBetIcons : 25
            anchors.horizontalCenter: parent.horizontalCenter
            spacing : spaceBetIcons
            Repeater
            {
                id : columnRepeater2
                anchors.verticalCenter: parent.verticalCenter
                model :(!doorFirmwareType) ? 2 : 0

                Item
                {
                    id : temp2
                    height: 90
                    width : 80

                    MxIcon
                    {
                        id : iconElement2
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        property int currIdx  : 2 + index

                        iconIdx: iconIdxList[currIdx]
                        iconEnableF: mainGrid.iconEnableList[currIdx]

                        onClicked:
                        {
                            iconPress(idx)
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted:
    {
        mainGrid.opacity = 1
        mainGrid.scale = 1
    }
}
