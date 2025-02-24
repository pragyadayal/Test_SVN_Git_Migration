import QtQuick 1.1
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id: popUp
    property alias boxHeading : heading.text
    property int selectedValue
    property variant selectionList
    property string selectionBoxIdentity

    height : lcd_height
    width : lcd_width

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : popUpRectId
        height : list.height + 20
        width : 200
        anchors.centerIn: parent

        Column
        {
            id: list
            anchors.centerIn: parent

            Item
            {
                id : headingRect
                height: 30
                width : 145
                anchors.horizontalCenter: parent.horizontalCenter
                MxText
                {
                    id: heading
                    width : 145
                    anchors.verticalCenter: parent.verticalCenter
                    font.pointSize: fontSizeTextLabel
                    font.bold: true
                    color: mxColorBlue
                //   wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignLeft
                }
            }

            Repeater
            {
                model: selectionList
                Image
                {
                    id: select
                    height:38
                    width: 150
                    source: imageLocation + "/Controls/SmallButtonBackground.png"
                    MouseArea
                    {
                        id : touchArea
                        anchors.fill: parent
                        onPressed:
                        {
                            selectionDone(index)
                        }
                    }

                    MxText
                    {
                        id: data
                        width : 135
                        anchors.centerIn: select
                        text:selectionList[index]
                        color:"white"
                        font.pointSize: fontSizeTextLabel
                    //    wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignLeft
                    }
                }
            }
        }
    }

    function selectionDone(choiceIdx)
    {
        sendOnlyKeepAlive()
        selectedValue = choiceIdx
        //NOTE : unload Pop-up before selection done because
        //       In server setting We are loading Another POP up.
        popUpLoader.unloadPopUp()
        mainLoader.item.selectionDone(selectionBoxIdentity,selectedValue)
    }

    function processDeviceResponse(title,identity,options)
    {
        boxHeading = title
        selectionList = options
        selectionBoxIdentity = identity
    }
}
