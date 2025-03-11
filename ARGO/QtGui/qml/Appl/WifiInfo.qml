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
    width: lcd_width - 30
    height:200
    anchors.horizontalCenter: parent.horizontalCenter

    anchors.top         : parent.top
    anchors.topMargin   : topMarginSettingScreen
    property bool doneF : true
    property bool isInit : false
    property bool infoVisible : true
    opacity : (infoVisible ? 1 : 0)

    property int     noOfSSID : 0
    property variant ssidName      : [""]
    property variant connectionStatus : [""]
    property variant wifiConnected    : [0]
    property variant isSecured     : [0]
    property variant encryptionType: [0]
    property variant fieldStrength : [0]
    property variant secType       : [0]
    property variant wpsEnabledF   : [0]
    property variant bssid         : [""]
    property string  imgPath : imageLocation + "/Controls/CheckBox"
    property bool    wifiEnableF: true
    property bool    wifiFieldShowF: true
    property variant bandFreqType : [0]

    signal editClicked()
    signal addClicked()
    signal selectionDoneForSetting(int idx)

    Item
    {
        id                  : wifiEnableDisableId
        anchors.top         : parent.top
        anchors.topMargin   : 4
        anchors.left        : parent.left
        anchors.right       : parent.right
        visible             : wifiFieldShowF


        MxText
        {
            id                  : wifiText
            anchors.verticalCenter      : parent.verticalCenter
            anchors.verticalCenterOffset: 2
            anchors.left        : parent.left
            text                : dispStr.readDispStr(DispStr.MSG_WI_FI_658) /*"Wifi" status*/
            font.pointSize      : fontSizeProfileInfo
            font.bold           : true
            smooth              : true
            color               : "White"
        }

        Image
        {
            id                          : checkboxImg
            anchors.verticalCenter      : parent.verticalCenter
            anchors.verticalCenterOffset: 2
            anchors.right               : parent.right
            source                      : ( imgPath+ ( wifiEnableF ? "Selected" : "") + ".png")

            MouseArea
            {
                anchors.centerIn: parent
                height  		: checkboxImg.sourceSize.height+15
                width   		: checkboxImg.sourceSize.width+25
                enabled         : true
                onPressed:
                {
                    sendOnlyKeepAlive()

                    wifiEnableF = (wifiEnableF ? false : true)

                    appData.data = EnumClass.GUI_MSG_WIFI_ENABLE_DISABLE_FLAG +
                                                           String.fromCharCode(uiMsgFsp).toString() +
                                                           (wifiEnableF ? 1 : 0) +
                                                           String.fromCharCode(uiMsgFsp).toString() +
                                                           String.fromCharCode(eotFsp).toString()

                }
            }
        }
    }

    Component
    {
        id          : wifiDelegate

        WifiInfoElement
        {
            id : infoElementId
            elementSsidName     : SSIDName
            wifiConnectionStatus    : wifiEnableF ? ConnectionStatus :""
            elementIsSecured    : IsSecured
            elementFieldStrength: FieldStrength
            elementWifiConnected : WifiConnected

            onElementClicked:
            {
                sendOnlyKeepAlive()
                if(isInit && (index == 0))
                {
                    editClicked()
                }
                else
                {
                    selectionDoneForSetting(index)
                }
            }
        }
    }
    Component.onCompleted:
    {
        wifiList.contentWidth   = wifiList.width
        wifiList.contentHeight += wifiDelegate.height * noOfSSID
    }

    ListModel
    {
        id:wifiModel
    }

    ListView
    {
        id                      : wifiList
        height                  : 200           // 2 * 40
        width                   : lcd_width - 20
        anchors.top             : wifiEnableDisableId.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.topMargin       : wifiFieldShowF ? 20 :0

        model                   : wifiModel
        delegate                : wifiDelegate

        snapMode                : ListView.SnapToItem
        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        cacheBuffer             : flickableCacheBuffer
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration
        opacity                 : wifiEnableF ? 1: 0.50
        enabled                 : wifiEnableF

        onMovementStarted:
        {
            sendOnlyKeepAlive()
        }

        ScrollBar
        {
            noOfElements        : noOfSSID
            visibleElements     : 2
            position            : wifiList.visibleArea.yPosition
        }
    }

    Row
    {
        id : buttons
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 10
        spacing : 5

        ButtonWithLabel
        {
            id : buttonScan
            isButtonInImageFormat: true
            buttonImagePath: imageLocation + "/ButtonIcons/scan.png"
            //button_Text: "Scan"
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                appData.data =  EnumClass.GUI_BUTTON_SEARCH_WIFINW +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }
        }

        ButtonWithLabel
        {
            id : buttonEdit
            isButtonInImageFormat: true
            buttonImagePath: imageLocation + "/ButtonIcons/edit.png"
           //button_Text: "Edit"
            visible : false
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                editClicked()
            }
        }

        ButtonWithLabel
        {
            id : buttonForget
            button_Text:dispStr.readDispStr(DispStr.MSG_FORGET_247)//"Forget"
            visible : false
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                appData.data =  EnumClass.MSG_WIFI_DFLT_CFG_EVENT +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }
        }

        ButtonWithLabel
        {
            id : buttonAdd
            isButtonInImageFormat: true
            buttonImagePath: imageLocation + "/ButtonIcons/add.png"
            //button_Text:"Add"
            visible : false
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                addClicked()
            }
        }
    }

    Row
    {
        id : wpsButtonsRow1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: buttons.bottom
        anchors.topMargin: 10
        spacing : 5
        width: parent.width - 10

        ButtonWithLabel
        {
            id : buttonWpsPush
            button_Text: dispStr.readDispStr(DispStr.MSG_WPS_PUSH_BUTTON_666)/*"WPS Push Button"*/
            isLargeButton: true
            width: parent.width
            buttonImagePath: imageLocation + "/Controls/LargeButton_240px.png"
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                appData.data =  EnumClass.GUI_BUTTON_WPS_PUSH_BUTTON +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }
        }

    }

    Row
    {
        id : wpsButtonsRow2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: wpsButtonsRow1.bottom
        anchors.topMargin: 2
        spacing : 5
        width: parent.width - 10

        ButtonWithLabel
        {
            id : buttonWpsPin
            button_Text: dispStr.readDispStr(DispStr.MSG_WPS_PIN_ENTRY_665)/*"WPS Pin Entry"*/
            isLargeButton: true
            width: parent.width
            buttonImagePath: imageLocation + "/Controls/LargeButton_240px.png"
            opacity         : wifiEnableF ? 1: 0.50
            enabled         : wifiEnableF
            onButtonwithLabelPress:
            {
                appData.data =  EnumClass.GUI_BUTTON_WPS_PIN_ENTRY +
                                String.fromCharCode(uiMsgFsp).toString() +
                                String.fromCharCode(eotFsp).toString()
            }
        }
    }

    function setDisplayInit(currSsid,currConnectionStatus,currIsSecurity,currFieldStrength, currWifiEnableF,currWifiConnected,currbandFreqType)
    {
        isInit = true
        wifiEnableF = currWifiEnableF

        if(currSsid != "")
        {
            noOfSSID = 1
            ssidName = [currSsid]
            connectionStatus = [currConnectionStatus]
            isSecured = [currIsSecurity]
            bandFreqType = [currbandFreqType]
            fieldStrength = [currFieldStrength]
            wifiConnected = [currWifiConnected]

            fillWiFiInfoList()

            buttonAdd.visible = false
            buttonEdit.visible = true
            buttonForget.visible = true
        }
        else
        {
            noOfSSID = 0
            wifiModel.clear()

            buttonAdd.visible = true
            buttonEdit.visible = false
            buttonForget.visible = false
        }
        wifiFieldShowF = true
    }

    function fillWiFiInfoList()
    {
		var tempSSID            = new Array()
        wifiModel.clear()
        for(var wifiIndex = 0; (wifiIndex < noOfSSID); wifiIndex++)
        {
			if(bandFreqType[wifiIndex] == 1)
			{
				tempSSID[wifiIndex] = ssidName[wifiIndex] + " (5G)"
			}
			else
			{
				tempSSID[wifiIndex] = ssidName[wifiIndex]
			}
             wifiModel.insert (wifiIndex,
                            {
                                "SSIDName"     : tempSSID[wifiIndex],
                                "ConnectionStatus" : connectionStatus[wifiIndex],
                                "IsSecured"    : isSecured[wifiIndex] ,
                                "FieldStrength": fieldStrength[wifiIndex],
                                "WifiConnected": wifiConnected[wifiIndex]
                           })
        }
        //buttonAdd.visible = false
        buttonEdit.visible = false
        buttonForget.visible = false

        wifiFieldShowF = (noOfSSID == 0)?true:false
    }
}
