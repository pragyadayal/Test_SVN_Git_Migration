import QtQuick 1.0
import ApplControl 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

import "../payLoadHandler.js" as PayloadLib

Item
{
    id     : container
    height : spinBoxImage.sourceSize.height
    property int    value               : 0
    property int    topValidation       : 9
    property alias  spinBoxVisibleF     : container.visible
    property alias  listIndx            : spinBoxText.text
    property int    dispQty             : 0
    signal quantityPlusCall()

    onQuantityPlusCall: {
        plusMouseArea.pressed()
    }

    Image
    {
        id: spinBoxImage
        source: imageLocation + "/Controls/CafeSpinBoxBg.png"
        smooth: true
        anchors.centerIn: parent

        Column
        {
            id : spinBoxColumn
            anchors.centerIn: parent
            MxText
            {
                id : plusImagetext
                text : "+"
                font.pointSize: fontSizeCafeListText
                color : "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            MxText
            {
                id: spinBoxText
                color: "white"
                font.bold : true
                font.pointSize: fontSizeCafeListText
                text : ""
                anchors.horizontalCenter: parent.horizontalCenter
            }
            MxText
            {
                id : minusImagetext
                text : "-"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: fontSizeCafeListText
                color : "white"
            }
        }

        MouseArea
        {
            id : plusMouseArea
            height: (parent.height)/2
            width: parent.width
            anchors.top : parent.top
            onPressed:
            {
//                console.log("in plusMouseArea value:"+value+",dispQty"+dispQty);
                if(dispQty < topValidation)
                {
                    value = dispQty + 1
                }
            }
        }

        MouseArea
        {
            id: minusMouseArea
            height: (parent.height)/2
            width: parent.width
            anchors.bottom : parent.bottom
            onPressed:
            {
//                console.log("in SpinBoxMouseArea value:"+value+",dispQty"+dispQty);
                if(dispQty > 0)
                {
                    value = dispQty - 1
                }
            }
        }
    }
}
