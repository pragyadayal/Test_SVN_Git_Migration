import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id: container
    height: spinBoxColumn.height
    width: spinBoxColumn.width
    property alias value             : spinBoxText.text
    property int topValidation
    property int bottomValidation
    property alias spinBoxVisibleF   : container.visible
    property int noOfDigit : 0
    property bool currKey : false    // true : Up
                                     // false: Down

    signal spinBoxClicked

    Column
    {
        id : spinBoxColumn
        spacing: 2
        Item
        {
            id : plusRect
            height : plusImage.sourceSize.height
            width  : plusImage.sourceSize.width
            anchors.horizontalCenter: parent.horizontalCenter

            Image
            {
                id : plusImage
                anchors.centerIn: parent
                height : (plusPressArea.pressed) ? (sourceSize.height * 0.7) : sourceSize.height
                width  : (plusPressArea.pressed) ? (sourceSize.width  * 0.7) : sourceSize.width
                source: imageLocation + "/Controls/SpinBoxIncrease.png"
                smooth: true
            }

            MouseArea
            {
                id : plusPressArea
                anchors.fill: parent
                onPressed:
                {
                    increment()
                    sendOnlyKeepAlive()
                    spinBoxClicked()
                }
                onPressAndHold:
                {
                    longPressTimer.start()
                    currKey = true
                }
                onReleased:
                {
                    longPressTimer.stop()
                }
            }
        }

        Image
        {
            id: name
            anchors.horizontalCenter: parent.horizontalCenter
            source: imageLocation + "/Controls/SpinBoxDataBg.png"
            MxText
            {
                id: spinBoxText
                color: mxColorGrey
                font.bold : true
                font.pointSize: fontSizeSpinBoxIndex
                anchors.centerIn: parent
            }
        }

        Item
        {
            id : minusRect
            height : minusImage.sourceSize.height
            width  : minusImage.sourceSize.width
            anchors.horizontalCenter: parent.horizontalCenter
            Image
            {
                id : minusImage
                anchors.centerIn: parent
                height : (minusPressArea.pressed) ? (sourceSize.height * 0.7) : sourceSize.height
                width  : (minusPressArea.pressed) ? (sourceSize.width  * 0.7) : sourceSize.width
                source: imageLocation + "/Controls/SpinBoxDecrease.png"
                smooth: true
            }

            MouseArea
            {
                id : minusPressArea
                anchors.fill: parent
                onPressed:
                {
                    decrement()
                    sendOnlyKeepAlive()
                    spinBoxClicked()
                }

                onPressAndHold:
                {
                    longPressTimer.start()
                    currKey = false
                }
                onReleased:
                {
                    longPressTimer.stop()
                }
            }
        }
    }

    onValueChanged:
    {
        if(noOfDigit > 0)
        {
            if(noOfDigit > value.length )
            {
                value = "0" + value
            }
        }
    }

    function increment()
    {
        if(spinBoxText.text == topValidation)
        {
            spinBoxText.text = bottomValidation
        }
        else
        {
            spinBoxText.text++
        }
    }

    function decrement()
    {
        if(spinBoxText.text == bottomValidation)
        {
            spinBoxText.text = topValidation
        }
        else
        {
            spinBoxText.text--
        }
    }

    Timer
    {
        id : longPressTimer
        interval: 200
        running : false
        repeat : true

        onTriggered:
        {
            if(currKey == true)
            {
                increment()
            }
            else
            {
                decrement()
            }
        }
    }
}

