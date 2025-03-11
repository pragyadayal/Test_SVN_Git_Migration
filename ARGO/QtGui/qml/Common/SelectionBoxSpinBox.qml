import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id: selectionPopUpRectId
    width: lcd_width
    height: lcd_height

    property alias boxHeading: heading.text
    property alias selectedValue : spin.value
    property string selectionBoxIdentity
    property int inputValidation

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : popUpRectId
        height : 200
        width : 200
        anchors.centerIn: parent

        MxText
        {
            id: heading
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 15
            font.pointSize: fontSizeTextLabel
            color: mxColorBlue
            font.bold: true
        }

        SpinBox
        {
            id : spin
            anchors.centerIn: parent
            bottomValidation : 1
            topValidation: inputValidation
        }

        ButtonElement
        {
            id : buttons
			isInImageFormat:true
            leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
            rightBtnImagePath: imageLocation + "/ButtonIcons/ok and set.png"
            anchors.bottomMargin:5
            buttonRowVisibleF: true

            //buttonLeftText: "Cancel"
            onButtonLeftPress:
            {
                popUpLoader.unloadPopUp()
            }

            //buttonRightText: "Ok"
            onButtonRightPress:
            {
                mainLoader.item.selectionDone(selectionBoxIdentity,selectedValue)
                popUpLoader.unloadPopUp()
            }
        }
    }

    function processDeviceResponse(title,identity,selectedNo,validation)
    {
        boxHeading = title
        selectionBoxIdentity = identity
        inputValidation = validation
        selectedValue = selectedNo
    }
}
