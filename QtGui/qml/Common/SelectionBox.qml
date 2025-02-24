import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Image
{
    id : selectionBox
//    source: imageLocation + "/Controls/TextEditBox.png"
    height:24
    width: 153

    property variant    options
    property string     title
    property int        selectedNo
    property string     identity
    property variant    iconIdxList
    property bool       isSpinBox : false
    property bool       isIcon: false
    property int        validation
    signal selectionClicked

    MouseArea
    {
        anchors.fill:parent
        onClicked:
        {
            sendOnlyKeepAlive()
            selectionClicked()
            unloadKeypad()

            if(isIcon == true)
            {
                popUpLoader.loadSelectionBoxIconPopUp()
                popUpLoader.item.processDeviceResponse(title,identity,options,iconIdxList)
            }
            else if(isSpinBox == true)
            {
                popUpLoader.loadselectionBoxSpinBoxPopUp()
                popUpLoader.item.processDeviceResponse(title,identity,selectedNo.toString(),validation)
            }
            else
            {
                popUpLoader.loadSelectionBoxTextListPopUp()
                popUpLoader.item.processDeviceResponse(title,identity,options)
            }
        }
    }

    MxText
    {
        id: shownText
        color : "white"
        width : 100
        elide : Text.ElideRight
        font.pointSize: fontSizeTextInputs
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 10
        text : ( isSpinBox ? selectionBox.selectedNo : selectionBox.options[selectedNo])
    }
    Rectangle
    {
        color   : "black"
        height  : selectionBox.height
        width   : selectionBox.width
        visible : true
        opacity : 0.3
    }
    Image
    {
        id:selectionAero
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source:imageLocation + "/Controls/PickList.png"
    }
}
