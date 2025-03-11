import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    width: lcd_width
    height: lcd_height

    OpacityRectangle
    {

    }

    PageHeading
    {
        title :  dispStr.readDispStr(DispStr.MSG_LAN_SETTINGS_312) /*"LAN Settings"*/
    }

    MxRectangleFrame
    {
        id                       : ethernetRectId
        width                    : lcd_width - 6
        height                   : (alphaNumKeypadF ? 192 : (doorFirmwareType ? 125 : 192))
        anchors.top              : parent.top
        anchors.topMargin        : topMarginSettingScreen
        anchors.horizontalCenter : parent.horizontalCenter

        ScrollBar
        {
            noOfElements        : (doorFirmwareType ? 3 : 5)
            visibleElements     : (alphaNumKeypadF ? 2 : (doorFirmwareType ? 3 : 5))
            position            : flickableArea.visibleArea.yPosition
        }
    }

    Flickable
    {
        id                      : flickableArea
        height                  : ethernetRectId.height -20
        width                   : ethernetRectId.width - 8
        anchors.centerIn        : ethernetRectId

        contentHeight           : ehternetColumn.height
        contentWidth            : ehternetColumn.width

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
            id                    : ehternetColumn
            spacing               : spacingBtwSettingElement
            anchors.left          : parent.left
            anchors.leftMargin    : 20

            Row
            {
                id      : modeRow
                spacing : spacingBtwSettingLabel
                visible : !doorFirmwareType
                TextLabel
                {
                    label :dispStr.readDispStr(DispStr.MSG_IP_ASSIGNMENT_297) /*"IP Assignment"*/
                }
                SelectionBox
                {
                    id          : mode
                    title       : dispStr.readDispStr(DispStr.MSG_MODE_797) /*"MODE"*/
                    identity    : "mode"
                    options     : [dispStr.readDispStr(DispStr.MSG_DYNAMIC_178)/*"Dynamic"*/, dispStr.readDispStr(DispStr.MSG_STATIC_583)/*"Static"*/]
                }
            }

            Row
            {
                id      : row1
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_IP_ADDRESS_296) /*"IP Address"*/
                }
                IPField
                {
                    id        : ipAddress
                    isEnableF : true
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(2)
                    }
                }
            }
            Row
            {
                id      :row2
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_SUBNET_591) /*"Subnet"*/
                }
                IPField
                {
                    id        : subNet
                    isEnableF : true
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(0)
                    }
                }
            }
            Row
            {
                id      : row3
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_GATEWAY_251) /*"Gateway"*/
                }
                IPField
                {
                    id        : gateWay
                    isEnableF : true
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(0)
                    }
                }
            }
            Row
            {
                id      : row4
                spacing : spacingBtwSettingLabel
                visible : !(doorFirmwareType)
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_PREFERRED_DNS_456) /*"Preferred DNS"*/
                }
                IPField
                {
                    id        :preferredDns
                    isEnableF : true
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(0)
                    }
                }
            }
            Row
            {
                id      : row5
                spacing : spacingBtwSettingLabel
                visible : !(doorFirmwareType)
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_ALTERNATE_DNS_42) /*"Alternate DNS"*/
                }
                IPField
                {
                    id        :alternateDns
                    isEnableF : true
                    onActivated:
                    {
                        flickableArea.contentY = setVisibleArea(0)
                    }
                }
            }
        }
    }

    DynamicValidationResp
    {
        id : responseData
    }

    Connections
    {
        target: alphaNumKeypadId
        onVkpressed :
        {
            responseData.validationF = false
        }
    }

    ButtonElement
    {
        id : buttons
		isInImageFormat:true
        centerBtnImagePath: imageLocation + "/ButtonIcons/save.png"
        buttonCenterVisibleF: true
        anchors.bottomMargin: 165
        //buttonCenterText: "Save"
        onButtonCenterPress:
        {
            generateEthernetPayload()
        }
    }


    function selectionDone(identity,indx)
    {
        if(identity === "mode")
        {
            mode.selectedNo = indx
        }
    }

    function generateEthernetPayload()
    {
        var tempMode
        var tempEthIp
        var tempEthSubnet
        var tempEthGW
        var tempEthPDns
        var tempEthADns
        var tempEthernetData
        var validF = true

        tempMode = mode.selectedNo + String.fromCharCode(uiMsgFsp).toString()
        if( ipAddress.isIpFieldFilled())
        {
            tempEthIp =  ipAddress.getIpFieldValue() +
                    String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            validF = false
            responseData.responseIndex = EnumClass.VALIDATION_RESP_INVALID_IP
            responseData.validationF = true
            ipAddress.clearIpField()
        }

        if( subNet.isIpFieldFilled())
        {
            tempEthSubnet =  subNet.getIpFieldValue() +
                    String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            if(validF == true)
            {
                validF = false
                responseData.responseIndex = EnumClass.VALIDATION_RESP_INVALID_SUBNET
                responseData.validationF = true
            }
            subNet.clearIpField()
        }

        if( gateWay.isIpFieldEmpty())
        {
            tempEthGW = String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempEthGW =  gateWay.getIpFieldValue() +
                    String.fromCharCode(uiMsgFsp).toString()
        }

        if( preferredDns.isIpFieldEmpty())
        {
            tempEthPDns = String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempEthPDns =  preferredDns.getIpFieldValue() +
                    String.fromCharCode(uiMsgFsp).toString()
        }

        if( alternateDns.isIpFieldEmpty())
        {
            tempEthADns = String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempEthADns =  alternateDns.getIpFieldValue() +
                    String.fromCharCode(uiMsgFsp).toString()
        }

        if(validF == true)
        {
            tempEthernetData =  EnumClass.MSG_ETHERNET_CFG_VALIDATION_EVENT +
                                String.fromCharCode(uiMsgFsp).toString() +
                                tempEthIp +
                                tempEthSubnet +
                                tempEthGW

                    if(doorFirmwareType == true)
                    {
                        // door type is Panel Door
                        tempEthernetData = tempEthernetData + String.fromCharCode(eotFsp).toString()

                    }
                    else
                    {
                        // door type is Direct Door
                        tempEthernetData = tempEthernetData +
                                           tempEthPDns +
                                           tempEthADns + tempMode +
                                           String.fromCharCode(eotFsp).toString()
                    }
                    //console.log("EthernetSettings Data--------->" + tempEthernetData)
                    appData.data = tempEthernetData
         }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        textBoxF = false
        switch(msgId)
        {
            case EnumClass.GUI_MSG_EHTERNET_CFG_SCREEN:
            {
                PayloadLib.parsePayload(msgId, rcvData)
                break
            }

            case EnumClass.GUI_MSG_VALIDATION_RESPONSE:
            {
                sendOnlyKeepAlive()

                var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
                //console.log(tmpPayload)
                responseData.responseIndex = parseInt(tmpPayload[0],10)
                responseData.validationF = true

                switch(responseData.responseIndex)
                {
                    case EnumClass.VALIDATION_RESP_INVALID_IP:
                    {
                        ipAddress.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_SUBNET:
                    {
                        subNet.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_GATEWAY:
                    {
                        gateWay.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_PDNS:
                    {
                        preferredDns.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_ADNS:
                    {
                        alternateDns.clearIpField()
                        break
                    }
                    default:
                    {
                        break
                    }
                }
                break
            }
        }
        textBoxF = true
    }
}
