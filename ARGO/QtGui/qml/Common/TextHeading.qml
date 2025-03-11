import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id:textLabel
    width: 220
    height: 24

    property alias label: labeltext.text

    Text
    {
        id: labeltext
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        color: "white"
        wrapMode             : Text.WordWrap
        horizontalAlignment  : Text.AlignLeft
        font.pointSize: fontSizeTextLabel
        smooth : true
        font.bold : true
    }
    Seperator
    {

    }
}
