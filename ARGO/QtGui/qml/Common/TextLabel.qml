import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id:textLabel
    width: 100
    height: 24

    property bool isEnable : true
    property alias label: labeltext.text

    MxText
    {
        id: labeltext
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        color: mxColorGrey
    //    wrapMode             : Text.WordWrap
        horizontalAlignment  : Text.AlignLeft
        font.pointSize: fontSizeTextLabel
        smooth : true
        opacity: (isEnable ? 1 : 0.5 )
    }
}
