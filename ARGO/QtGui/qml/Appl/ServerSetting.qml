import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : serverSettings
    width   : lcd_width
    height  : lcd_height

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_SERVER_SETTINGS_531) /*"Server Settings"*/
    }

    MxRectangleFrame
    {
        id                       : serverRectId
        width                    : lcd_width - 6
        height                   : (alphaNumKeypadF) ? 160 : 300
        anchors.top              : parent.top
        anchors.topMargin        : topMarginSettingScreen
        anchors.horizontalCenter : parent.horizontalCenter

        ScrollBar
        {
            noOfElements        : 4
            visibleElements     : (alphaNumKeypadF ? 5 : 10)
            position            : flickableArea.visibleArea.yPosition
        }
    }

    Flickable
    {
        id                      : flickableArea
        height                  : serverRectId.height -20
        width                   : serverRectId.width - 8
        anchors.centerIn        : serverRectId

        contentHeight           : serverColumn.height
        contentWidth            : serverColumn.width

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
            id                  : serverColumn
            spacing             : spacingBtwSettingElement
            anchors.left          	: parent.left
            anchors.leftMargin      : 20

            Row
            {
                id              : row1
                spacing         : spacingBtwSettingLabel
                TextLabel
                {
                    label       : dispStr.readDispStr(DispStr.MSG_SERVER_525) /*"Server"*/
                }
                SelectionBox
                {
                    id          : serverModeId
                    title       : dispStr.readDispStr(DispStr.MSG_SERVER_525) /*"Server"*/
                    identity    : "ServerMode"
                    options     : [dispStr.readDispStr(DispStr.MSG_CENTRA_643)/*"CENTRA"*/,
                                   dispStr.readDispStr(DispStr.MSG_VYOM_651)/*"VYOM"*/,
                                   dispStr.readDispStr(DispStr.MSG_THIRD_PARTY_971)/*THIRD PARTY*/]
                }
            }

            Row
            {
                id              : row2
                spacing         : spacingBtwSettingLabel
                TextLabel
                {
                    label       : dispStr.readDispStr(DispStr.MSG_URL_584) /*Server "URL"*/
                }
                TextEditBox
                {
                    id          : serverURL
                    maxChar     : 253
                    isEnable    : true
                }
            }

            Row
            {
                id              : row3
                spacing         : spacingBtwSettingLabel
                TextLabel
                {
                    label       : dispStr.readDispStr(DispStr.MSG_PORT_448) /* Server "Port"*/
                }
                TextEditBox
                {
                    id          : serverPortNumber
                    validation  : IntValidator{bottom: 1; top:65535}
                    isEnable    : true
                    onActivated :
                    {
                        flickableArea.contentY = setVisibleArea(0)
                    }
                }
            }

            Row
            {
                id              : row4
                spacing         : spacingBtwSettingLabel
                visible         : (serverModeId.selectedNo == 1) ? true : false
                TextLabel
                {
                    label       : dispStr.readDispStr(DispStr.MSG_TENANT_ID_663) /*"Tenant ID"*/
                }
                TextEditBox
                {
                    id          : serverTenantID
                    validation  : RegExpValidator{regExp: /[0-9]+/}
                    maxChar     : 10
                    isEnable    : true
                    onActivated :
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
        target      : alphaNumKeypadId
        onVkpressed :
        {
            responseData.validationF = false
        }
    }

    ButtonElement
    {
        id                      : buttons
        buttonCenterVisibleF    : true
        isInImageFormat         : true
        centerBtnImagePath      : imageLocation + "/ButtonIcons/save.png"
        //buttonCenterText: "Save"
        onButtonCenterPress     :
        {
            generateServerPayload()
        }
    }

    function selectionDone(identity,indx)
    {
        if(identity === "ServerMode")
        {
            if(serverModeId.selectedNo !== indx)
            {
                if(indx == 2) //THIRD PARTY
                {
                    indx = 3
                }
                popUpLoader.loadConfirmDefault(false, indx)
            }
            else
            {
                if(indx == 2) //THIRD PARTY
                {
                    indx = 3
                }
                serverModeId.selectedNo = indx
                serverSettings.focus = true
            }
        }
    }

    function generateServerPayload()
    {
        var tempServerMode
        var tempServerURL
        var tempServerPort
        var tempServerTenantID
        var tempServerData
        var validF = true

        // Validations (Mandatory Fields)
        if(serverURL.value.length <= 0)
        {
            validF = false
            responseData.responseIndex = EnumClass.VALIDATION_RESP_BLANK_SERVER_URL /* "Enter URL" */
            responseData.validationF = true
            serverURL.value = ""
        }
        else if(serverPortNumber.value.length <= 0)
        {
            validF = false
            responseData.responseIndex = EnumClass.VALIDATION_RESP_BLANK_SERVER_PORT /* "Enter Port" */
            responseData.validationF = true
            serverPortNumber.value = ""
        }
        else if( (serverModeId.selectedNo == 1) && (serverTenantID.value.length <= 0) )
        {
            validF = false
            responseData.responseIndex = EnumClass.VALIDATION_RESP_BLANK_SERVER_TENANT_ID  /* "Enter Tenant ID" */
            responseData.validationF = true
            serverTenantID.value = ""
        }

        // Server Mode
        if(serverModeId.selectedNo == 2)
        {
            tempServerMode = 3	//THIRD PARTY
        }
        else
        {
            tempServerMode = serverModeId.selectedNo
        }

        // Server URL
        if(serverURL.value.length > 0)
        {
            tempServerURL = serverURL.value  + String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempServerURL = String.fromCharCode(uiMsgFsp).toString()
        }

        //Server Port
        if(serverPortNumber.value.length > 0)
        {
            tempServerPort = serverPortNumber.value + String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempServerPort = String.fromCharCode(uiMsgFsp).toString()
        }

        //Server Tenant ID
        if(serverTenantID.value.length > 0)
        {
            tempServerTenantID = serverTenantID.value  + String.fromCharCode(uiMsgFsp).toString()
        }
        else
        {
            tempServerTenantID = String.fromCharCode(uiMsgFsp).toString()
        }

        if(validF == true)
        {
            tempServerData  = EnumClass.MSG_SERVER_CFG_VALIDATION_EVENT +
                                String.fromCharCode(uiMsgFsp).toString() +
                                tempServerMode +
                                String.fromCharCode(uiMsgFsp).toString() +
                                tempServerURL +
                                tempServerPort +
                                tempServerTenantID +
                                String.fromCharCode(eotFsp).toString()

   //         console.log("Server Settings Data---------->" + tempServerData)
            appData.data = tempServerData
        }
    }

    function processDeviceResponse(msgId,rcvData)
    {
        textBoxF = false

        switch(msgId)
        {
            case EnumClass.GUI_MSG_ACMS_SERVER_CFG_SCREEN:
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

//                switch(responseData.responseIndex)
//                {

////                    case EnumClass.VALIDATION_RESP_BLANK_SERVER_URL	:
////                    {
////                        serverURL.value = ""
////                        break
////                    }

////                    case EnumClass.VALIDATION_RESP_BLANK_SERVER_PORT :
////                    {
////                        serverPortNumber.value = ""
////                        break
////                    }

//                    case EnumClass. VALIDATION_RESP_INVALID_SERVER_PORT :
//                    {
////                        serverPortNumber.value = ""
//                        break
//                    }

////                    case EnumClass.VALIDATION_RESP_BLANK_SERVER_TENANT_ID :
////                    {
////                        serverTenantID.value = ""
////                        break
////                    }

//                    case EnumClass.VALIDATION_RESP_INVALID_SERVER_TENANT_ID :
//                    {
////                        serverTenantID.value = ""
//                        break
//                    }

//                }
                break
            }
        }
        textBoxF = true
    }
}
