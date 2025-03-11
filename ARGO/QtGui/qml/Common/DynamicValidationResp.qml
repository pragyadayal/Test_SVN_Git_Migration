import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

import "../payLoadHandler.js" as PayloadLib


Rectangle
{
    id: respMsgRect
    height: 30

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

    color : "transparent"
    anchors.top : parent.top
    anchors.topMargin: 35 + 12 // (topMarginPageHeading + additionalMargin)
    anchors.horizontalCenter: parent.horizontalCenter
    MxText
    {
        id: respText
        visible: validationF
        text: responseMessage[responseIndex]
        font.pointSize: fontSizeDynamicResponse
        color : mxColorRed
        anchors.centerIn: parent
    }
}
