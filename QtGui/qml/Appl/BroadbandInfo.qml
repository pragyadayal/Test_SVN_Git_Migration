import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : broadbandInfoPage
    width   : lcd_width
    height  : lcd_height

    property int     noOfProfiles
    property variant activateProfile    : [""]
    property variant profileName        : [""]
    property string  imgPath            : imageLocation + "/Controls/CheckBox"
    property bool profileConnectedF     : false

    OpacityRectangle
    {

    }

    PageHeading
    {
        id          : heading
        title       : dispStr.readDispStr(DispStr.MSG_MOBILE_BROADBAND_SETTINGS_351)	/*"Mobile Broadband Settings"*/
    }

    Component
    {
        id          : broadbandDelegate

        Item
        {
            id      : broadbandItem
            width   : broadbandList.width
            height  : 40                                 // Height of Each Row of profile

            MouseArea
            {
                id              : itemMouseArea
                anchors.fill    : parent
                onPressed :
                {
                    sendOnlyKeepAlive()
                    appData.data = EnumClass.MSG_SEND_MODEM_PROFILE_DATA +
                            String.fromCharCode(uiMsgFsp).toString() +
                            index.toString() +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
                }
            }

            MxText
            {
                id                  : profileNameText
                anchors.top         : parent.top
                anchors.topMargin   : 4
                anchors.left        : parent.left
                text                : ProfileName
                font.pointSize      : fontSizeProfileInfo
                font.bold           : (ActivateProfile == true) ? true : false
                smooth              : true
                color               : "White"
                elide               : Text.ElideRight
            }

            Image
            {
                id                          : checkboxImg
                anchors.verticalCenter      : profileNameText.verticalCenter
                anchors.verticalCenterOffset: 2
                anchors.right               : parent.right
                source                      : ( imgPath+ ( (ActivateProfile == true) ? "Selected" : "") + ".png")
            }

            MxText
            {
               id                   : connectionStatus
               anchors.left         : parent.left
               anchors.bottom       : parent.bottom
               anchors.bottomMargin : 4
               visible              : ActivateProfile
               text                 : profileConnectedF ?  dispStr.readDispStr(DispStr.MSG_CONNECTED_99)/*"Connected"*/:dispStr.readDispStr(DispStr.MSG_NOT_CONNECTED_774) /*Not Connected*/
               font.pointSize       : fontSizeProfileStatus
               smooth               : true
               color                : "white"
            }

            Seperator
            {

            }

            Component.onCompleted:
            {
                broadbandList.contentWidth   = broadbandInfoPage.width
                broadbandList.contentHeight += broadbandItem.height * noOfProfiles
            }
        }
    }

    ListModel
    {
        id  : broadbandModel
    }

    ListView
    {
        id                      : broadbandList
        height                  : ( noOfProfiles * 40 )
        width                   : lcd_width - 30
        anchors.top             : parent.top
        anchors.topMargin       : topMarginSettingScreen
        anchors.horizontalCenter: parent.horizontalCenter

        model                   : broadbandModel
        delegate                : broadbandDelegate

        snapMode                : ListView.SnapToItem
        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        cacheBuffer             : flickableCacheBuffer
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
            sendOnlyKeepAlive()
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        fillList()
    }

    function fillList ()
    {
        broadbandModel.clear()
        for(var index = 0; (index < noOfProfiles); index++)
        {
            broadbandModel.insert (index,
                                   {
                                       "ProfileName"    :profileName[index] ,
                                       "ActivateProfile":activateProfile[index]
                                   })
        }
    }
}
