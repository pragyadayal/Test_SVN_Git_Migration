import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id : mainGridId
    height : iconGridHeight
    width : lcd_width

    property int noOfIconOnPage
    property int noOfIconOnRow
    property variant iconIdxList
    property variant iconEnableList
    color : "transparent"
    clip : true
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

    signal iconClick (int idx)

    Column
    {
        id : gridColumn
        anchors.centerIn: parent
        spacing : spaceBetIcons
        Repeater
        {
            id : columnRepeater
            anchors.horizontalCenter: parent.horizontalCenter
            model : columnRepeatCnt
            Row
            {
                id: first
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater
                {
                    id : rowRepeater
                    model: noOfIconOnRow
                    property int rowIdx : index

                    Item
                    {
                        id : temp
                        height: 90
                        width : 100

                        MxIcon
                        {
                            id : iconElement
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            property int currIdx  : ((rowRepeater.rowIdx * columnRepeatCnt) + index )
                            visible : (currIdx < noOfIconOnPage) ? true : false

                            iconIdx: iconIdxList[currIdx]
                            iconEnableF: mainGridId.iconEnableList[currIdx]

                            onClicked:
                            {
                                iconClick(idx)
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted:
    {
        mainGridId.opacity = 1
        mainGridId.scale = 1
    }
}
