import QtQuick 1.1
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id : scrollBar

    height : parent.height - 4
    width : 6
    anchors.right: parent.right
    opacity : 1

    property int noOfElements : 0
    property int visibleElements : 1
    property real position

    property real pageSize : (visibleElements/noOfElements)
    visible : (noOfElements > visibleElements) ? true : false

    property int scrollReleaseDuration : 3       // Resolution 100 msec

    Behavior on opacity
    {
        NumberAnimation{ duration: scrollReleaseDuration * 100; from : 1}
    }

    Rectangle
    {
        id : barImg
        height: (scrollBar.pageSize * (parent.height))
        width : 4
        radius: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : parent.top
        anchors.topMargin: (scrollBar.position * (parent.height))
        color : mxColorBlue
    }

    Timer
    {
        id : keyTimer
        interval: 300
        running : false

        onTriggered:
        {
            scrollBar.opacity = 0
        }
    }

    onPositionChanged:
    {
        scrollBar.opacity = 1
        keyTimer.restart()
    }

    Component.onCompleted:
    {
        keyTimer.restart()
    }
}
