import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : wifiMainRectId
    width: lcd_width
    height: lcd_height

    property string dataSsid                : ""
    property int    dataSecurityType        : 0
    property string dataPassword            : ""
    property int    dataConnectionType      : 0
    property string dataIpAddress           : ""
    property string dataSubnet              : ""
    property string dataGateway             : ""
    property string dataDns1                : ""
    property string dataDns2                : ""
    property int    dataWifiStatus          : 0

    property int    dataEncryptionType      : 0
    property string dataAnonymousId         : ""
    property string dataUserName            : ""
    property int    dataAdvAuthType         : 0
    property int    dataPeapVerType         : 0
    property int    dataInnerAuthType       : 0

    property alias isWifiInfoVisible    : wifiInfo.infoVisible
    property alias isSettingVisible     : wifiSettingRectId.visible

    property string currSsid            : ""
    property string currConnectionStatus     : dispStr.readDispStr(DispStr.MSG_NOT_CONNECTED_774)// "Not Connected"
    property bool   currIsSecured       : true
    property int    currFieldStrength   : 0
    property bool   currWifiConnected   : false

    property bool   currWpsEnabledF     : true
    property string currBssid           : ""
    property bool   currWifiEnableF     : true

    property bool isWpsPinOptionSelected : ( (currWpsEnabledF && (connOption.selectedNo == 1) )? true : false)
    property bool isSecured : ( ( !isWpsPinOptionSelected && securityType.selectedNo >= 1) ? true : false )
    property bool isEncrypted : ( ( !isWpsPinOptionSelected && securityType.selectedNo >= 2) ? true : false )
    property bool isEnterprise : ( ( !isWpsPinOptionSelected && (securityType.selectedNo == 4 || securityType.selectedNo == 5) ) ? true : false )
    property bool isAddNetwork : false
    property bool isAdvSettings: false
    property int  currbandFreqType : 0

    OpacityRectangle
    {

    }

    PageHeading
    {
        id : heading
        title: dispStr.readDispStr(DispStr.MSG_WI_FI_SETTINGS_796) /*"Wifi Settings"*/
        visible : !isSettingVisible
    }

    Item
    {
        id : titleSsidId
        width : lcd_width - 20
        height : 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : heading.top
        visible : isSettingVisible

        MxText
        {
            width : (parent.width - 20)
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text : isAddNetwork ? dispStr.readDispStr(DispStr.MSG_ADD_NETWORK_30) : currSsid
            color : "white"
            font.pointSize: fontSizeProfileInfo
            elide : Text.ElideRight
        }

        Image
        {
            id                     : ssidStrength
            source                 : imageLocation + "/Wifi/" + currFieldStrength.toString() +(currIsSecured ? "_lock" : "")+".png"
            anchors.right          : parent.right
            anchors.verticalCenter : parent.verticalCenter
            visible                : isAddNetwork ? false : true
        }
    }

    MxRectangleFrame
    {
        id                       : wifiSettingRectId
        width                    : lcd_width - 6
        height                   : (((alphaNumKeypadF) ? 160 : 310) > (flickableArea.contentHeight + 20) ? (flickableArea.contentHeight + 20): ((alphaNumKeypadF) ? 160 : 310))
        anchors.top              : parent.top
        anchors.topMargin        : topMarginSettingScreen
        anchors.horizontalCenter : parent.horizontalCenter
        visible                  : false

        Flickable
        {
            id                      : flickableArea
            height                  : wifiSettingRectId.height -20
            width                   : wifiSettingRectId.width - 8
            anchors.top             : parent.top
            anchors.topMargin       : 10
            anchors.horizontalCenter: parent.horizontalCenter

            contentHeight           : wifiColumn.height
            contentWidth            : wifiColumn.width

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
                id                    : wifiColumn
                spacing               : spacingBtwSettingElement
                anchors.left          : parent.left
                anchors.leftMargin    : 20
                Row
                {
                    id      : ssidRowId
                    spacing : spacingBtwSettingLabel
                    visible : isAddNetwork
                    property int currIndex : setCurrIdx(0, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_SSID_578) /*"SSID"*/
                    }
                    TextEditBox
                    {
                        id      : ssid
                        maxChar : 32
                        onValueChanged:
                        {
                            currWpsEnabledF = false
                        }

                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }

                Row
                {
                    id      : connOptionsRowId
                    spacing : spacingBtwSettingLabel
                    visible : (currWpsEnabledF ? true : false)
                    property int currIndex : setCurrIdx(ssidRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_CONNECTIVITY_101)/*"Connectivity"*/
                    }
                    SelectionBox
                    {
                        id : connOption
                        options : [dispStr.readDispStr(DispStr.MSG_PASSWORD_426)    /*Admin "Password"*/,
                                   dispStr.readDispStr(DispStr.MSG_PIN_FROM_ACCESS_POINT_437)      /*"PIN from access point"*/]
                        title    : dispStr.readDispStr(DispStr.MSG_CONNECTIVITY_OPTIONS_102)   /*"Connectivity Options"*/
                        identity : "connOption"

                        onSelectedNoChanged:
                        {
                            checkForPasswordLength()
                        }
                    }
                }

                Row
                {
                    id      : securityTypeRowId
                    spacing : spacingBtwSettingLabel
                    visible : (!isWpsPinOptionSelected)
                    property int currIndex : setCurrIdx(connOptionsRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_SECURITY_507) /*"Security"*/
                    }
                    SelectionBox
                    {
                        id : securityType
                        options : [
                                   dispStr.readDispStr(DispStr.MSG_NO_AUTHENTICATION_388) /*"No Authentication"*/ ,
                                   dispStr.readDispStr(DispStr.MSG_WEP_657) /*"WEP"*/,
                                   dispStr.readDispStr(DispStr.MSG_WPA_843) /*WPA*/,
                                   dispStr.readDispStr(DispStr.MSG_WPA2_PERSONAL_844)/*"WPA2 Personal"*/,
                                   dispStr.readDispStr(DispStr.MSG_WPA_ENTERPRISE_845) /*"WPA Enterprise"*/,
                                   dispStr.readDispStr(DispStr.MSG_WPA2_ENTERPRISE_846) /*"WPA2 Enterprise"*/]
                        title    : dispStr.readDispStr(DispStr.MSG_SECURITY_507) /*"Security"*/
                        identity : "securityType"

                        onSelectedNoChanged:
                        {
                            checkForPasswordLength()
                        }
                    }
                }

                Row
                {
                    id      : encryptionTypeRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEncrypted)
                    property int currIndex : setCurrIdx(securityTypeRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_ENCRYPTION_188) /*"Encryption"*/
                    }
                    SelectionBox
                    {
                        id : encryptionType
                        options : [dispStr.readDispStr(DispStr.MSG_PSK_473) /*PSK*/ ,
                                   dispStr.readDispStr(DispStr.MSG_EAP_179)/*EAP*/]
                        title    : dispStr.readDispStr(DispStr.MSG_ENCRYPTION_188) /*"Encryption"*/
                        identity : "encryptionType"
                    }
                }

                Row
                {
                    id      : anonymousRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEnterprise)
                    property int currIndex : setCurrIdx(encryptionTypeRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_ANONYMOUS_ID_46) /*"Anonymous ID"*/
                    }
                    TextEditBox
                    {
                        id      : anonymousId
                        maxChar : 50
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }

                Row
                {
                    id      : userNameRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEnterprise)
                    property int currIndex : setCurrIdx(anonymousRowId.currIndex, visible)
                    TextLabel
                    {
                        label :  dispStr.readDispStr(DispStr.MSG_USERNAME_632)      /*"Username"*/
                    }
                    TextEditBox
                    {
                        id      : userNameId
                        maxChar : 50

                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }

                Row
                {
                    id      : passwordRowId
                    spacing : spacingBtwSettingLabel

                    //Not visible for No Authentication
                    visible :(isSecured)
                    property int currIndex : setCurrIdx(userNameRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_PASSWORD_426) /*Admin "Password"*/
                    }
                    TextEditBoxPassword
                    {
                        id      : keydata
                        maxChar : 64
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }

                        onValueChanged:
                        {
                            checkForPasswordLength()
                        }
                    }
                }

                Row
                {
                    id      : pinRowId
                    spacing : spacingBtwSettingLabel

                    //Not visible for No Authentication
                    visible : isWpsPinOptionSelected
                    property int currIndex : setCurrIdx(passwordRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_PIN_436) /*"PIN"*/
                    }
                    TextEditBoxPassword
                    {
                        id      : pinData
                        maxChar : 9
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }

                Image
                {
                    id : advSettingsOptRowId
                    visible : !isAdvSettings
                    property int currIndex : setCurrIdx(pinRowId.currIndex, visible)
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    source : imageLocation + "/Controls/More.png"
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            isAdvSettings = true
                        }
                    }
                }

                Row
                {
                    id      : advAuthTypeRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEnterprise && isAdvSettings)
                    property int currIndex : setCurrIdx(advSettingsOptRowId.currIndex, visible)
                    TextLabel
                    {
                        label :  dispStr.readDispStr(DispStr.MSG_ADVANCED_AUTHENTICATION_35)  /*"Advance Authentication"*/
                    }
                    SelectionBox
                    {
                        id : advAuthType
                        options :  [dispStr.readDispStr(DispStr.MSG_AUTO_50)       /*"Auto"*/,
                                    dispStr.readDispStr(DispStr.MSG_TLS_606)            /*"TLS"*/,
                                    dispStr.readDispStr(DispStr.MSG_LEAP_320)        /*LEAP*/,
                                    dispStr.readDispStr(DispStr.MSG_FAST_235)           /*FAST*/,
                                    dispStr.readDispStr(DispStr.MSG_TUNNELED_TLS_614)   /*"Tunneled TLS"*/,
                                    dispStr.readDispStr(DispStr.MSG_PEAP_432)        /*"PEAP"*/]
                        title    :  dispStr.readDispStr(DispStr.MSG_ADVANCED_AUTHENTICATION_35)  /*"Advance Authentication"*/
                        identity : "advAuthType"
                    }
                }

                Row
                {
                    id      : peapVerTypeRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEnterprise && isAdvSettings)
                    property int currIndex : setCurrIdx(advAuthTypeRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_PEAP_VERSION_435) /*PEAP Version*/
                    }
                    SelectionBox
                    {
                        id : peapVerType

                        options  : [
                                    dispStr.readDispStr(DispStr.MSG_AUTO_50)       /*"Auto"*/,
                                    dispStr.readDispStr(DispStr.MSG_VERSION_0_859)       /*"Version 0"*/,
                                    dispStr.readDispStr(DispStr.MSG_VERSION_1_860)       /*"Version 1"*/,
                                    dispStr.readDispStr(DispStr.MSG_PEAP_LABEL_1_434)/*"PEAP Label 1"*/,
									]
                        title    : dispStr.readDispStr(DispStr.MSG_PEAP_VERSION_435) /*PEAP Version*/
                        identity : "peapVerType"
                    }
                }

                Row
                {
                    id      : innerAuthTypeRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isEnterprise && isAdvSettings)
                    property int currIndex : setCurrIdx(peapVerTypeRowId.currIndex, visible)
                    TextLabel
                    {
                        label :dispStr.readDispStr(DispStr.MSG_INNER_AUTHENTICATION_274)/*"Inner Authentication"*/
                    }
                    SelectionBox
                    {
                        id : innerAuthType
                        options :[
                                   dispStr.readDispStr(DispStr.MSG_AUTO_50)        /*"Auto"*/,
                                   dispStr.readDispStr(DispStr.MSG_MSCHAPV2_366)        /*"MSCHAPV2"*/,
                                   dispStr.readDispStr(DispStr.MSG_MD5_340)             /*"MD5"*/,
                                   dispStr.readDispStr(DispStr.MSG_GTC_260)             /*"GTC"*/,
                                  ]
                        title    : dispStr.readDispStr(DispStr.MSG_INNER_AUTHENTICATION_274)/*"Inner Authentication"*/
                        identity : "innerAuthType"
                    }
                }

                Row
                {
                    id : ipSettingsRowId
                    visible : isAdvSettings
                    property int currIndex : setCurrIdx(innerAuthTypeRowId.currIndex, visible)
                    spacing : spacingBtwSettingLabel
                    TextLabel
                    {
                        label :dispStr.readDispStr(DispStr.MSG_IP_SETTINGS_299) /*"IP Settings"*/
                    }

                    SelectionBox
                    {
                        id : connectionType
                        options :(!doorFirmwareType) ?
                                  [
                                    dispStr.readDispStr(DispStr.MSG_STATIC_583) /*"Static"*/,
                                    dispStr.readDispStr(DispStr.MSG_DYNAMIC_178) /*"DHCP"*/]:
                                    [dispStr.readDispStr(DispStr.MSG_STATIC_583) /*"Static"*/]
                        title :dispStr.readDispStr(DispStr.MSG_IP_SETTINGS_299) /*"IP Settings"*/
                        identity : "connectiontype"
                    }
                }
                Row
                {
                    id : ipAddrRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isAdvSettings && (connectionType.selectedNo == 0))
                    property int currIndex : setCurrIdx(ipSettingsRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_IP_ADDRESS_296) /*"IP Address"*/
                    }
                    IPField
                    {
                        id : wifiIpAddress
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }
                Row
                {
                    id: subnetRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isAdvSettings && (connectionType.selectedNo == 0))
                    property int currIndex : setCurrIdx(ipAddrRowId.currIndex, visible)
                    TextLabel
                    {
                        label :  dispStr.readDispStr(DispStr.MSG_SUBNET_MASK_592) /*"Subnet Mask"*/
                    }
                    IPField
                    {
                        id: wifiSubnet
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }
                Row
                {
                    id : gatewayRowId
                    spacing : spacingBtwSettingLabel
                    visible : (isAdvSettings && (connectionType.selectedNo == 0))
                    property int currIndex : setCurrIdx(subnetRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_GATEWAY_251)/*GateWay*/
                    }
                    IPField
                    {
                        id : wifiGateway
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }
                Row
                {
                    id : dns1RowId
                    spacing : spacingBtwSettingLabel
                    visible : (isAdvSettings && (connectionType.selectedNo == 0))
                    property int currIndex : setCurrIdx(gatewayRowId.currIndex, visible)
                    TextLabel
                    {
                        label : dispStr.readDispStr(DispStr.MSG_PREFERRED_DNS_456) /*"Preferred DNS"*/
                    }
                    IPField
                    {
                        id        : wifiDns1
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }
                Row
                {
                    id : dns2RowId
                    spacing : spacingBtwSettingLabel
                    visible : (isAdvSettings && (connectionType.selectedNo == 0))
                    property int currIndex : setCurrIdx(dns1RowId.currIndex, visible)
                    TextLabel
                    {
                        label :dispStr.readDispStr(DispStr.MSG_ALTERNATE_DNS_42) /*"Alternate DNS"*/
                    }
                    IPField
                    {
                        id        : wifiDns2
                        onActivated:
                        {
                            flickableArea.contentY = setVisibleArea(parent.currIndex)
                        }
                    }
                }
            }
        }

        ScrollBar
        {
            noOfElements        : dns2RowId.currIndex
           visibleElements     :  (alphaNumKeypadF ? 5 : 8)
            position            : flickableArea.visibleArea.yPosition
        }
    }

    WifiInfo
    {
        id : wifiInfo

        onEditClicked:
        {
            isAddNetwork = false
            isAdvSettings = false
            isWifiInfoVisible = false
            isSettingVisible = true
            fillSettingData()
            currWpsEnabledF = false
            checkForPasswordLength()
        }

        onAddClicked:
        {
            isAddNetwork = true
            isAdvSettings = false
            isWifiInfoVisible = false
            isSettingVisible = true
            clearSettingData()
            currWpsEnabledF = false
            checkForPasswordLength()
        }

        onSelectionDoneForSetting :
        {
            isAddNetwork = false
            isAdvSettings = false
            isWifiInfoVisible = false
            isSettingVisible = true

            clearSettingData()

            currSsid = (wifiInfo.bandFreqType[idx] == 1) ? wifiInfo.ssidName[idx] + "( 5G)" : wifiInfo.ssidName[idx]
            ssid.value = wifiInfo.ssidName[idx]
            currConnectionStatus = wifiInfo.connectionStatus[idx]
            currIsSecured = wifiInfo.isSecured[idx]
            currFieldStrength = wifiInfo.fieldStrength[idx]
            currWifiConnected = wifiinfo.wifiConnected[idx] ? true : false 
            currWpsEnabledF = wifiInfo.wpsEnabledF[idx]
            currBssid = wifiInfo.bssid[idx]
            currbandFreqType = wifiInfo.bandFreqType[idx]
            securityType.selectedNo = wifiInfo.secType[idx]
            encryptionType.selectedNo = wifiInfo.encryptionType[idx]
            checkForPasswordLength()
        }
    }

    ButtonElement
    {
        id : settingButtons
        buttonRowVisibleF: isSettingVisible

        //buttonLeftText: "Connect"
		 isInImageFormat:true
		leftBtnImagePath: imageLocation + "/ButtonIcons/connect.png"
        rightBtnImagePath:imageLocation + "/ButtonIcons/cancel.png"

        onButtonLeftPress:
        {
            generateWiFiPayload()
        }

       // buttonRightText: "Cancel"
        onButtonRightPress:
        {
            isSettingVisible = false
            isWifiInfoVisible = true
            unloadKeypad()
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

    function checkForPasswordLength()
    {
        var minPasswordLen = 0

        if(currWpsEnabledF == true && connOption.selectedNo == 1)
        {
            minPasswordLen = 0
        }
        else
        {

            var secType = securityType.selectedNo;
            if (secType == 0)
            {
                minPasswordLen = 0
            }
            else if (secType == 1)
            {
                minPasswordLen = 1
            }
            else if (secType == 2)
            {
                minPasswordLen = 8
            }
            else if (secType == 3)
            {
                minPasswordLen = 8
            }
            else if (secType == 4)
            {
                minPasswordLen = 1
            }
            else if (secType == 5)
            {
                minPasswordLen = 1
            }
        }

        if(keydata.value.length >= minPasswordLen)
        {
            settingButtons.buttonLeftEnable = true
        }
        else
        {
            settingButtons.buttonLeftEnable = false
        }
    }

    function generateWiFiPayload()
    {
        var tempWiFiIP
        var tempWiFiSubnet
        var tempWiFiGateway
        var tempWiFiDns1
        var tempWiFiDns2
        var tempSecType
        var tempNwConnectionType
        var tempAdvAuthType
        var tempAnonymousId
        var tempUserName
        var tempPeapVerType
        var tempInnerAuthType
        var tempWiFiData
        var tempConnOption
        var validF = true

        tempSecType = securityType.selectedNo
        tempNwConnectionType = connectionType.selectedNo

        if(tempSecType == 4 || tempSecType == 5)
        {
            tempAnonymousId = anonymousId.value + String.fromCharCode(uiMsgFsp).toString()
            tempUserName = userNameId.value + String.fromCharCode(uiMsgFsp).toString()

            tempAdvAuthType = advAuthType.selectedNo
            tempPeapVerType = peapVerType.selectedNo
            tempInnerAuthType = innerAuthType.selectedNo
        }
        else
        {
            tempAnonymousId = String.fromCharCode(uiMsgFsp).toString()
            tempUserName = String.fromCharCode(uiMsgFsp).toString()

            tempAdvAuthType = 0
            tempPeapVerType = 0
            tempInnerAuthType = 0
        }

        if(tempNwConnectionType == 0)
        {
            if( wifiIpAddress.isIpFieldFilled())
            {
                tempWiFiIP = wifiIpAddress.getIpFieldValue() +
                             String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                validF = false
                responseData.responseIndex = EnumClass.VALIDATION_RESP_INVALID_IP
                responseData.validationF = true
                wifiIpAddress.clearIpField()
             }

            if( wifiSubnet.isIpFieldFilled())
            {
                tempWiFiSubnet = wifiSubnet.getIpFieldValue() +
                                String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                if(validF == true)
                {
                    responseData.responseIndex = EnumClass.VALIDATION_RESP_INVALID_SUBNET
                    responseData.validationF = true
                    validF = false
                }
                wifiSubnet.clearIpField()
            }

            if( wifiGateway.isIpFieldEmpty())
            {
                tempWiFiGateway = String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                tempWiFiGateway =    wifiGateway.getIpFieldValue() +
                                     String.fromCharCode(uiMsgFsp).toString()
            }

            if( wifiDns1.isIpFieldEmpty())
            {
                tempWiFiDns1 = String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                tempWiFiDns1 =    wifiDns1.getIpFieldValue() +
                                     String.fromCharCode(uiMsgFsp).toString()
            }

            if( wifiDns2.isIpFieldEmpty())
            {
                tempWiFiDns2 = String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                tempWiFiDns2 =    wifiDns2.getIpFieldValue() +
                                     String.fromCharCode(uiMsgFsp).toString()
            }
        }
        else
        {
            tempWiFiIP = String.fromCharCode(uiMsgFsp).toString()
            tempWiFiSubnet = String.fromCharCode(uiMsgFsp).toString()
            tempWiFiGateway = String.fromCharCode(uiMsgFsp).toString()
            tempWiFiDns1 = String.fromCharCode(uiMsgFsp).toString()
            tempWiFiDns2 = String.fromCharCode(uiMsgFsp).toString()
        }

        if(validF == true)
        {
            tempWiFiData = EnumClass.MSG_WIFI_CFG_VALIDATION_EVENT +
                           String.fromCharCode(uiMsgFsp).toString() +
                           ssid.value +
                           String.fromCharCode(uiMsgFsp).toString() +
						   currbandFreqType +
                           String.fromCharCode(uiMsgFsp).toString()
            if(currWpsEnabledF && (connOption.selectedNo == 1))
            {
                tempConnOption = 1
                tempWiFiData += tempConnOption.toString() +
                                String.fromCharCode(uiMsgFsp).toString() +
                                currBssid +
                                String.fromCharCode(uiMsgFsp).toString() +
                                pinData.value +
                                String.fromCharCode(uiMsgFsp).toString()
            }
            else
            {
                tempConnOption = 0
                tempWiFiData += tempConnOption.toString() +
                                String.fromCharCode(uiMsgFsp).toString() +
                                tempSecType.toString() +
                                String.fromCharCode(uiMsgFsp).toString() +
                                keydata.value +
                                String.fromCharCode(uiMsgFsp).toString() +
								encryptionType.selectedNo.toString() +
		                        String.fromCharCode(uiMsgFsp).toString() +
		                        tempAdvAuthType.toString() +
		                        String.fromCharCode(uiMsgFsp).toString() +
		                        tempAnonymousId +
		                        tempUserName +
		                        tempPeapVerType.toString() +
		                        String.fromCharCode(uiMsgFsp).toString() +
		                        tempInnerAuthType.toString() +
                                String.fromCharCode(uiMsgFsp).toString()
            }

            tempWiFiData += tempNwConnectionType.toString() +
                            String.fromCharCode(uiMsgFsp).toString() +
                            tempWiFiIP +
                            tempWiFiSubnet +
                            tempWiFiGateway +
                            tempWiFiDns1 +
                            tempWiFiDns2 +
                            String.fromCharCode(eotFsp).toString()

            //console.log("WiFi Data ------------------>" + tempWiFiData)
            appData.data = tempWiFiData
        }
    }

    function selectionDone(identity,indx)
    {
        if(identity === "securityType")
        {
            securityType.selectedNo = indx
        }
        else if (identity === "connectiontype")
        {
            connectionType.selectedNo = indx
        }
        else if (identity === "connOption")
        {
            connOption.selectedNo = indx
        }
        else if(identity === "encryptionType")
        {
            encryptionType.selectedNo = indx
        }
        else if(identity === "advAuthType")
        {
            advAuthType.selectedNo = indx
        }
        else if(identity === "peapVerType")
        {
            peapVerType.selectedNo = indx
        }
        else if(identity === "innerAuthType")
        {
            innerAuthType.selectedNo = indx
        }
    }

    function fillSettingData()
    {
        isAddNetwork = false
        if(currbandFreqType == 1)
		{
			currSsid = dataSsid + " (5G)"
		}
		else
		{
        currSsid = dataSsid
		}

        ssid.value = dataSsid

        securityType.selectedNo = dataSecurityType
        encryptionType.selectedNo = dataEncryptionType

        anonymousId.value = dataAnonymousId
        userNameId.value = dataUserName

        keydata.value = dataPassword
        pinData.value = ""

        advAuthType.selectedNo = dataAdvAuthType
        peapVerType.selectedNo = dataPeapVerType
        innerAuthType.selectedNo = dataInnerAuthType

        connectionType.selectedNo = dataConnectionType
        wifiIpAddress.setIpFieldValue(dataIpAddress)
        wifiSubnet.setIpFieldValue(dataSubnet)
        wifiGateway.setIpFieldValue(dataGateway)
        wifiDns1.setIpFieldValue(dataDns1)
        wifiDns2.setIpFieldValue(dataDns2)
    }

    function clearSettingData()
    {
        ssid.value = ""

        securityType.selectedNo = 0
        encryptionType.selectedNo = 0

        anonymousId.value = ""
        userNameId.value = ""

        keydata.value = ""
        pinData.value = ""

        advAuthType.selectedNo = 0
        peapVerType.selectedNo = 0
        innerAuthType.selectedNo = 0


        if(doorFirmwareType == false)
        {
            // DoorApplication Type DirectDoor
            connectionType.selectedNo = 1
        }
        else
        {   // DoorApplication Type PanelDoor
            connectionType.selectedNo = 0
        }

        wifiIpAddress.clearIpField()
        wifiSubnet.clearIpField()
        wifiGateway.clearIpField()
        wifiDns1.clearIpField()
        wifiDns2.clearIpField()
    }

    function setCurrIdx(idx, visibleF)
    {
        if(visibleF)
            return (idx + 1)
        else
            return idx
    }

    function processDeviceResponse (msgId,rcvData)
    {
        textBoxF = false
        switch(msgId)
        {
            case EnumClass.GUI_MSG_WIFI_CFG_SCREEN:
            {
                PayloadLib.parsePayload(msgId, rcvData)

                wifiInfo.setDisplayInit(currSsid,currConnectionStatus,currIsSecured,currFieldStrength,currWifiEnableF,currWifiConnected,currbandFreqType)

                isWifiInfoVisible = true
                isSettingVisible = false

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
                    case EnumClass.VALIDATION_RESP_INVALID_IP :
                    {
                        wifiIpAddress.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_SUBNET :
                    {
                        wifiSubnet.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_GATEWAY:
                    {
                        wifiGateway.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_PDNS:
                    {
                        wifiDns1.clearIpField()
                        break
                    }
                    case EnumClass.VALIDATION_RESP_INVALID_ADNS:
                    {
                        wifiDns2.clearIpField()
                        break
                    }
                }
                break
            }

            case EnumClass.GUI_MSG_SEARCH_WIFI_SCREEN:
            {
                PayloadLib.parsePayload(msgId, rcvData)
                isWifiInfoVisible = true
                isSettingVisible = false
                wifiInfo.isInit = false
                wifiInfo.fillWiFiInfoList()
                break
            }

            case EnumClass.GUI_MSG_WIFI_SIGNAL_STRENGTH:
            {
                PayloadLib.parsePayload(msgId, rcvData)

                wifiInfo.setDisplayInit(currSsid,currConnectionStatus,currIsSecured,currFieldStrength,currWifiEnableF,currWifiConnected,currbandFreqType)
		 if(currbandFreqType == 1)
                {
                    currSsid = currSsid + " (5G)"
                }                
		break
            }
        }
        textBoxF = true
    }
}
