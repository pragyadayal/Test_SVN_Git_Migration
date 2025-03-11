import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id: textbox
    height: textBoxImg.height
    width : textBoxImg.width
    property string bgImgPath : imageLocation + "/Controls/TextEditBox.png"
    property alias value        : textboxData.text

    Image
    {
        id : textBoxImg
        source: bgImgPath
        anchors.centerIn: parent
    }

    MxText
    {
        id :textboxData
        width :parent.width - 12
        font.pointSize :fontSizeTextInputs
        anchors.centerIn: parent
        text : ""
        color : "white"
        smooth : true
        clip : true
    }

}
