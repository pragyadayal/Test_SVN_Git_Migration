import QtQuick 1.1
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    width: lcd_width - 20
    height: 43

    property string label : ""
    property string value : ""
    property bool   isExpandable : false

    signal elementClicked()

    Column
    {
        spacing : 3
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        MxText
        {
            id                  : elmentLabel
            text                : label
            height              : 15
            font.pointSize      : (elementValue.text.length > 0 ) ? fontSizeAboutDevice : (fontSizeAboutDevice * 1.1 )
            smooth              : true
            color               : "white"
        }
        MxText
        {
            id                  : elementValue
            text                : value
            height              : 15
            font.pointSize      : fontSizeAboutDevice
            smooth              : true
            color               : "#999999"
        }
    }

    Seperator
    {

    }

    MouseArea
    {
        anchors.fill : parent
        onClicked:
        {
            elementClicked()
        }
    }

    Image
    {
        id:selectionAero
        visible : isExpandable
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source:imageLocation + "/Controls/MoreList.png"
    }
}
