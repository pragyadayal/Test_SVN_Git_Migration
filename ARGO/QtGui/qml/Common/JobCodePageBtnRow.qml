import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Row
{
    id : pageBtnRow
    height: pageBtnLength
    width: (5 * pageBtnLength)

    property variant    btnLabel
    property int        rowLeftMargin
    property variant    labelFontSize
    property variant    imgVisibleF

    signal btnClick(int idx)

    anchors.left: parent.horizontalCenter
    anchors.leftMargin: rowLeftMargin

    Repeater
    {
        id : btnRepeater
        model: 5
        Row
        {
            Item
            {
                id: pageBtnItm
                height: pageBtnLength
                width: pageBtnLength
                visible: (pageBtnTxt.text.length > 0)? true : false

                Image
                {
                    id: jobCodeRowSelect
                    height: parent.height
                    width: parent.width
                    source: imageLocation + "/Controls/Page_No_BG.png"
                    anchors.centerIn: parent
                    visible : imgVisibleF[index]
                }
                MxText
                {
                    id : pageBtnTxt
                    text: btnLabel[index]
                    font.pointSize : labelFontSize[index]
                    anchors.centerIn: parent
                    color: "white"
                    visible: (text.length > 0)? true : false
                }
                MouseArea
                {
                    id: mousePageBtn
                    anchors.fill: parent
                    onClicked:
                    {
                        btnClick(index)
                    }
                }
            }
        }
    }
}
