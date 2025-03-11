// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id : calendrCell
    property alias cellText : cellTextBox.text
    property alias cellColor: cellTextBox.color
    property alias cellFontPointSize: cellTextBox.font.pointSize

    signal elementClicked()
    property alias cellHeight : calendrCell.height
    property alias cellwidth : calendrCell.width

    Text
    {
        anchors.centerIn: parent
        id : cellTextBox
    }
//    MouseArea
//    {
//        id: mouseArea
//        anchors.fill: calendrCell
//        enabled : true
//        onClicked:
//        {
//            elementClicked()
//        }
//    }
}
