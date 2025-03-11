import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : deviceipscreen
    height  : lcd_height
    width   : lcd_width

    property bool    validationF: false
    property int     responseIndex
    property variant responseMessage:
       ["",                                 //0
        dispStr.readDispStr(DispStr.MSG_INVALID_PASSWORD_288) 						/*1 "Invalid Password"*/,
        dispStr.readDispStr(DispStr.MSG__ENTER_VALID_IP_ADDRESS_11) 					/*2 "Enter Valid IP Address"*/,
        dispStr.readDispStr(DispStr.MSG__ENTER_VALID_SUBNET_15) 						/*3 "Enter Valid Subnet"*/,
        dispStr.readDispStr(DispStr.MSG__ENTER_VALID_GATEWAY_10) 						/*4 "Enter Valid GateWay"*/,
        dispStr.readDispStr(DispStr.MSG__ENTER_VALID_PREFERRED_DNS_14) 					/*5 "Enter Valid Preferred DNS"*/,
        dispStr.readDispStr(DispStr.MSG__ENTER_VALID_ALTERNATE_DNS_9) 					/*6 "Enter Valid Alternate DNS"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_PORT_831) 								/*7 "Enter Valid Port"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_DNS_832) 								/*8 "Enter Valid DNS"*/,
        dispStr.readDispStr(DispStr.MSG_INVALID_CURRENT_PASSWORD_833) 						/*9 "Invalid Current Password"*/,
        dispStr.readDispStr(DispStr.MSG_PASSWORD_MISMATCH_429) 						/*10 "Password Mismatch!"*/,
        dispStr.readDispStr(DispStr.MSG_PASSWORD_CANNOT_BE_BLANK_815) 							/*11 "Password cannot be blank!"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_IP_ADDRESS_FOR_ETHERNET_834) 			/*12 "Enter Valid IP Address for Ethernet"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_IP_ADDRESS_FOR_WI_FI_835) 				/*13 "Enter Valid IP Address for Wi-Fi"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_IP_ADDRESS_FOR_BROADBAND_836)           /*14 "Enter Valid IP Address for Broadband"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_PORT_FOR_ETHERNET_837)                  /*15 "Enter Valid Port for Ethernet"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_PORT_FOR_WI_FI_838)                      /*16 "Enter Valid Port for Wi-Fi"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_PORT_FOR_BROADBAND_839)                 /*17 "Enter Valid Port for Broadband"*/,
        "",                                                                         /*18 */
        dispStr.readDispStr(DispStr.MSG_ENTER_URL_883)                               /*19 "Enter URL"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_PORT_879)                              /*20 "Enter Port"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_PORT_831)                               /*21 "Enter Valid Port"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_TENANT_ID_880)                         /*22 "Enter Tenant ID"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_TENANT_ID_881)                   /*23 "Enter Valid Tenant ID"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_SLEEP_TIMER_VALUE_966)                        /*24 "Enter Sleep Timer Value"*/,
        dispStr.readDispStr(DispStr.MSG_ENTER_VALID_SLEEP_TIMER_VALUE_967)                  /*25 "Enter Valid Sleep Timer Value"*/,
    ]

    OpacityRectangle
    {

    }

    Connections
    {
        target: alphaNumKeypadId
        onVkpressed :
        {
            validationF = false
        }
    }

    ButtonElement
    {
        id                  : buttons
        buttonRowVisibleF   : true
        anchors.bottomMargin: 170
        isInImageFormat     :true
        leftBtnImagePath    : imageLocation + "/ButtonIcons/cancel.png"
        rightBtnImagePath   : imageLocation + "/ButtonIcons/ok and set.png"

        //buttonLeftText: "Cancel"
        onButtonLeftPress:
        {
            appData.data = EnumClass.GUI_BUTTON_CANCEL_OR_SKIP_TO_DYNAMIC +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }

        //buttonRightText: "Ok"
        onButtonRightPress:
        {
            appData.data= EnumClass.GUI_BUTTON_OK_ASSIGN_TENANT_ID +
                    String.fromCharCode(uiMsgFsp).toString() +
                    tenantId.value +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }

    Item
    {
        id                          : submitRectMouseArea
        anchors.top                 : parent.top
        anchors.topMargin           : 150
        anchors.horizontalCenter	: parent.horizontalCenter
        visible                     : true

        Rectangle
        {
            id 							: submitRect
            height 						: 40
            width 						: lcd_width - 10
            anchors.top                 : parent.top
            anchors.horizontalCenter	: parent.horizontalCenter
            color				       	: "transparent"

            Rectangle
            {
                color			: "#000000"
                opacity			: 0.7
                anchors.fill	: parent
            }

            Rectangle
            {
                id 					: topBorderLine
                height				: 2
                width				: parent.width
                anchors.left		: parent.left
                anchors.top         : parent.top
                color				: "#444444"
            }

            Rectangle
            {
                id 					: leftBorderLine
                height				: parent.height
                width				: 2
                anchors.left		: parent.left
                anchors.top         : parent.top
                color				: "#444444"
            }

            Text
            {
                id 					: submitBtn
                text 				: (dispStr.readDispStr(DispStr.MSG_SWITCH_TO_CENTRA_933)) //"Switch to CENTRA"
                color				: "#d8d8d8"
                font.pointSize  	: 14
                anchors.centerIn	: parent
            }

            MouseArea
            {
                id                      : submitBtnTouchArea
                height                  : parent.height
                width                   : parent.width
                enabled                 : true
                anchors.bottom          : parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                onPressed:
                {
                    appData.data = EnumClass.GUI_BUTTON_SWITCH_TO_CENTRA +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
                }
            }
        }
    }

    MxRectangleFrame
    {
        id                      : tenantIdRect
        anchors.top             : parent.top
        anchors.topMargin       : topMarginPswPin - 25
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        height: 80

        MxText
        {
            id                  : tenantIdStr
            anchors.top         : parent.top
            anchors.topMargin   : 8
            anchors.horizontalCenter: parent.horizontalCenter
            text                : dispStr.readDispStr(DispStr.MSG_ENTER_TENANT_ID_880) /*"Enter Tenant ID"*/
            color               : "#d8d8d8"
            font.pointSize      : 12
        }
        MxText
        {
            id              : respText
            visible         : validationF
            text            : responseMessage[responseIndex]
            font.pointSize  : fontSizeDynamicResponse
            color           : mxColorRed
            anchors.top     : tenantIdStr.bottom
            anchors.topMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
        }
        TextEditBox
        {
            id          : tenantId
			maxChar     : 10            
			validation  : RegExpValidator{regExp: /[0-9]+/}
           	isEnable    : true
            anchors.top     : respText.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        switch(msgId)
        {
            default:
                break;

            case EnumClass.GUI_MSG_VALIDATION_RESPONSE:
            {
                sendOnlyKeepAlive()

                var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
                //console.log(tmpPayload)
                responseIndex = parseInt(tmpPayload[0],10)
                validationF = true

                switch(responseIndex)
                {
                    case EnumClass.ENTER_SERVER_TENANT_ID:
                    {
                        tenantId.value = ""
                        break
                    }
                    case EnumClass.ENTER_VALID_SERVER_TENANT_ID:
                    {
                        tenantId.value = ""
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
        tenantId.setSelected()
    }
}
