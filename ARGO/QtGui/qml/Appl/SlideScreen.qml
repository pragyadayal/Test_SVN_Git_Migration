import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Rectangle
{
    width: lcd_width
    height: lcd_height
    color:"transparent"
    property bool isExpanded:false
    property int isExpandedStatus: 0

    visible: statusBarExpandedF

    MouseArea
    {
        id: draggingMouse
        anchors.fill : parent
        drag.target: parent
        drag.axis: Drag.YAxis
        drag.minimumY: - parent.height * .95
        drag.maximumY: 0
        enabled: false /*true*/

        onReleased:
        {
            if ( false == slider.isExpanded )
            {
                if( slider.y < - parent.height * .80 )
                    slider.y = - parent.height * .95
                else
                {
                    slider.y = 0
                    slider.isExpanded = true
                }
            }
            else
            {
                if(draggingMouse.mouseY > slider.y + topRect.height)
                {
                    hideSlideScreen()
                    return;
                }

                if( slider.y < - parent.height * .20 )
                {
                    hideSlideScreen()
                }
                else
                {
                    slider.y = 0
                }
            }
        }
    }

    onIsExpandedChanged: {
        if(doorFirmwareType)
        {
            slider.isExpanded ? hideSliderScreen.start() : hideSliderScreen.stop()
        }

        isExpandedStatus=(slider.isExpanded)? 1 : 0
        appData.data = EnumClass.GUI_STATUSBAR_EXPAND_STATUS +
                String.fromCharCode(uiMsgFsp).toString() +
                isExpandedStatus.toString() +
                String.fromCharCode(uiMsgFsp).toString() +
                String.fromCharCode(eotFsp).toString()
    }

    Item{
        id: transperant
        width:parent.width
        height: headerImage.height
        anchors.top: parent.top
    }

    Rectangle{
        id: topRect
        width: parent.width
        height: parent.height * 0.75
        anchors.top: transperant.bottom
        color: "black"

        MxMenuScreen
        {
            width: parent.width
            height: parent.height * 0.60
            anchors.topMargin: 0
            iconGridHeight:parent.height * 0.60
            maxIconPerPage : (doorFirmwareType) ? 2 : 4
            maxIconPerRow : 2
            columnRepeatCnt : (doorFirmwareType) ? 1 : 2
            spaceBetIcons : 25

            anchors{ centerIn: parent }

            iconIdxList:(!doorFirmwareType) ?
                            [//direct door
                             EnumClass.ICON_LAN,       //17
                             EnumClass.ICON_WI_FI,     //18
                             EnumClass.ICON_SERVER,    //16
                             EnumClass.ICON_MODEM      //19
                            ]:
                            [// panel door
                             EnumClass.ICON_LAN,       //17
                             EnumClass.ICON_WI_FI      //18
                            ]

            iconEnableList:(!doorFirmwareType) ?
                               [
                                   true,
                                   true,
                                   true,
                                   true
                               ]:
                               [
                                   true,
                                   true,
                               ]

            onIconPress:
            {
                hideSlideScreen()
                switch(idx)
                {
                default :
                    break
                case EnumClass.ICON_SERVER :
                    currentFeatureAccessed = EnumClass.GUI_BUTTON_SET_SERVER_CFG
                    break
                case EnumClass.ICON_LAN :
                    currentFeatureAccessed = EnumClass.GUI_BUTTON_SET_ETHERNET_CFG
                    break
                case EnumClass.ICON_WI_FI :
                    currentFeatureAccessed = EnumClass.GUI_BUTTON_SET_WIFI_CFG
                    break
                case EnumClass.ICON_MODEM :
                    currentFeatureAccessed = EnumClass.GUI_BUTTON_BROADBAND_CFG
                    break

                }

                isAdminHrdF = true
                appData.data = currentFeatureAccessed +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
            }
        }
    }

    Behavior on y { NumberAnimation { duration: 300 } }

    Rectangle{
        id: bottomBar
        color: mxColorOffWhite
        anchors.top: topRect.bottom
        width: parent.width * 0.45
        height: parent.height * 0.016
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: - bottomBar.height * 0.5
        radius: 4
    }

    Timer
    {
        id:hideSliderScreen
        interval:30000
        onTriggered:
        {
            hideSlideScreen()
        }
    }

    Component.onCompleted: {
        hideSlideScreen()
    }

    function hideSlideScreen()
    {
        slider.y = - slider.height * 0.95
        slider.isExpanded = false
    }
}
