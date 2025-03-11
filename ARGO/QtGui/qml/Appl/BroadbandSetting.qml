import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : mobileBroadbandSettings
    height  : lcd_height
    width   : lcd_width

    property bool profileActivateF
    property bool prflSelCheck
    property int  currentProfileIdx
    property string  imgPath : imageLocation + "/Controls/CheckBox"

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :dispStr.readDispStr(DispStr.MSG_BROADBAND_SETTINGS_855) /*"Broadband Settings"*/
    }

    MxRectangleFrame
    {
        id                       : broadbandRectId
        width                    : lcd_width - 6
        height                   : (alphaNumKeypadF) ? 165 : 300
        anchors.top              : parent.top
        anchors.topMargin        : topMarginSettingScreen
        anchors.horizontalCenter : parent.horizontalCenter

        ScrollBar
        {
            noOfElements        : 11
            visibleElements     : (alphaNumKeypadF ? 4 : 10)
            position            : flickableArea.visibleArea.yPosition
        }
    }

    Flickable
    {
        id                      : flickableArea
        height                  : broadbandRectId.height -20
        width                   : broadbandRectId.width - 8
        anchors.centerIn        : broadbandRectId

        contentHeight           : broadbandColumn.height
        contentWidth            : broadbandColumn.width

        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
             sendOnlyKeepAlive()
        }

        Column
        {
            id      : broadbandColumn
            spacing : spacingBtwSettingElement
            anchors.left            : parent.left
            anchors.leftMargin      : 20
            Row
            {
                id      : row1
                spacing : spacingBtwSettingLabel

                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_PROFILE_468)	/*"Profile"*/
                }
                Image
                {
                    id                  : checkboxImg
                    source              : (imgPath + (profileActivateF ? "Selected" : "") + ".png")

                    MouseArea
                    {
                        anchors.centerIn: parent
                        height  		: checkboxImg.sourceSize.height + 15
                        width   		: checkboxImg.sourceSize.width + 15
                        enabled         : prflSelCheck ? false : true
                        onPressed:
                        {
                            sendOnlyKeepAlive()
                            if (profileActivateF == true)
                            {
                                profileActivateF = false
                            }
                            else
                            {
                                profileActivateF = true
                            }
                        }
                    }
                }
            }

            Row
            {
                id      :row2
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_PROFILE_NAME_446)/*"Profile Name"*/
                }
                TextEditBox
                {
                    id      : profileName
                    maxChar : 15
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(2)
                    }
                }
            }

            Row
            {
                id      : row3
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_DIAL_NUMBER_136) /*"Dial Number"*/
                }
                TextEditBox
                {
                    id       : dialNumber
                    maxChar  : 16
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(3)
                    }
                }
            }

            Row
            {
                id      : row4
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_USER_NAME_1013) /*"User Name"*/
                }
                TextEditBox
                {
                    id      : userName
                    maxChar : 25
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(4)
                    }
                }
            }

            Row
            {
                id      : row5
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_PASSWORD_426) /*Admin "Password"*/
                }
                TextEditBoxPassword
                {
                    id          : password
                    maxChar     : 25
                    width       : lcd_width * 0.6
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(5)
                    }
                }
            }

            Row
            {
                id      : row6
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_SERVICE_826) /*"Service"*/
                }
                SelectionBox
                {
                    id       : serviceType
                    title    : dispStr.readDispStr(DispStr.MSG_SELECT_SERVICE_524) /*"Select Service"*/
                    identity : "serviceType"
                    options  : [dispStr.readDispStr(DispStr.MSG_GSM_259)/*"GSM"*/,dispStr.readDispStr(DispStr.MSG_CDMA_81)/*"CDMA"*/]
                }
            }

            Row
            {
                id      : row7
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_APN_47)  /*"APN"*/
                }
                TextEditBox
                {
                    id      : accessPointNetwork
                    maxChar : 40
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(7)
                    }
                }
            }

            Row
            {
                id      : row8
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_IP_ADDRESS_296) /*"IP Address"*/
                }
                IPField
                {
                    id        : modemIpAddress
                    isEnableF : false
                }
            }

            Row
            {
                id      : row9
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_GATEWAY_251) /*"Gateway"*/
                }
                IPField
                {
                    id        : modemGateway
                    isEnableF : false
                }
            }

            Row
            {
                id      : row10
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_PREFERRED_DNS_456) /*"Preferred DNS"*/
                }
                IPField
                {
                    id        : modemDns1
                    isEnableF : false
                }
            }

            Row
            {
                id      : row11
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_ALTERNATE_DNS_42) /*"Alternate DNS"*/
                }
                IPField
                {
                    id        : modemDns2
                    isEnableF : false
                }
            }
        }
    }

    function selectionDone(identity,indx)
    {
        if(identity === "serviceType")
        {
            serviceType.selectedNo = indx
        }
        mobileBroadbandSettings.focus = true
    }

    ButtonElement
    {
        id : buttons
        buttonRowVisibleF: true
		isInImageFormat:true
        leftBtnImagePath: imageLocation + "/ButtonIcons/refresh.png"
        rightBtnImagePath: imageLocation + "/ButtonIcons/save.png"

        //buttonLeftText: "Refresh"
        onButtonLeftPress:
        {
            getProfile()
        }
        //buttonRightText: "Save"
        onButtonRightPress:
        {
            generateModemPayload()
        }
    }

    function generateModemPayload()
    {
        var tempProfileIndex
        var tempProfileActive
        var tempServiceType
        var tempModemData

        tempProfileIndex  = currentProfileIdx.toString()
        tempProfileActive = (profileActivateF) ? "1" : "0"
        tempServiceType   = serviceType.selectedNo.toString()

        tempModemData  = EnumClass.MSG_MOBILE_CFG_VALIDATION_EVENT +
                         String.fromCharCode(uiMsgFsp).toString() +
                         tempProfileIndex +
                         String.fromCharCode(uiMsgFsp).toString() +
                         tempProfileActive +
                         String.fromCharCode(uiMsgFsp).toString() +
                         profileName.value +
                         String.fromCharCode(uiMsgFsp).toString() +
                         dialNumber.value +
                         String.fromCharCode(uiMsgFsp).toString() +
                         userName.value +
                         String.fromCharCode(uiMsgFsp).toString() +
                         password.value +
                         String.fromCharCode(uiMsgFsp).toString() +
                         tempServiceType +
                         String.fromCharCode(uiMsgFsp).toString() +
                         accessPointNetwork.value +
                         String.fromCharCode(uiMsgFsp).toString() +
                         String.fromCharCode(eotFsp).toString()

        //console.log("Modem Settings Data---------->" + tempModemData)
        appData.data = tempModemData
    }

    function processDeviceResponse (msgId,rcvData)
    {
        textBoxF = false
        PayloadLib.parsePayload(msgId, rcvData)
        textBoxF = true
    }

    function getProfile()
    {
        appData.data = EnumClass.MSG_SEND_MODEM_PROFILE_DATA +
                       String.fromCharCode(uiMsgFsp).toString() +
                       currentProfileIdx.toString() +
                       String.fromCharCode(uiMsgFsp).toString() +
                       String.fromCharCode(eotFsp).toString()
    }
}
