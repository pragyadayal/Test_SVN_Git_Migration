import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id : mainGrid
    width: 320
    height: 225

    anchors.top : parent.top
    anchors.topMargin: 50

    color: " transparent"

    property variant iconIdxList                // 0       3
                                                //     2
                                                // 1       4
    property variant iconEnableList : []

    signal iconPress(int idx)

    opacity : 1
    scale : 1

    property int menuPageLoadDuration : 3       // Resolution 100 msec

    Behavior on opacity
    {
        NumberAnimation{ duration: menuPageLoadDuration * 100; from : 0}
    }

    Behavior on scale
    {
        NumberAnimation{ duration: menuPageLoadDuration * 100; from : 0}
    }

    Row
    {
        id : mainRow
        anchors.centerIn: parent
        property int spaceBetIconsRow : 15
        spacing : spaceBetIconsRow

        Column
        {
            id : gridColumn
            property int spaceBetIcons : 20
            anchors.verticalCenter: parent.verticalCenter
            spacing : spaceBetIcons
            Repeater
            {
                id : columnRepeater
                anchors.horizontalCenter: parent.horizontalCenter
                model : 2

                Item
                {
                    id : temp
                    height: 90
                    width : 80

                    MxIcon
                    {
                        id : iconElement
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
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

        Item
        {
            id : temp1
            height: 90
            width : 80
            anchors.verticalCenter: parent.verticalCenter

            MxIcon
            {
                id : iconElement1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                property int currIdx  : 2

                iconIdx: iconIdxList[currIdx]
                iconEnableF: mainGrid.iconEnableList[currIdx]

                onClicked:
                {
                    iconPress(idx)
                }
            }
        }

        Column
        {
            id : gridColumn2
            property int spaceBetIcons : 20
            anchors.verticalCenter: parent.verticalCenter
            spacing : spaceBetIcons
            Repeater
            {
                id : columnRepeater2
                anchors.horizontalCenter: parent.horizontalCenter
                model : 2

                Item
                {
                    id : temp2
                    height: 90
                    width : 80

                    MxIcon
                    {
                        id : iconElement2
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        property int currIdx  : 3 + index

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
