import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    height : lcd_height
    width  : lcd_width

    property alias   delCredentialIndex : credential.selectedNo
    //spacingRelated
    property int topMarginUserInfoRect : 60
    property int labelWidth            : 105
    OpacityRectangle
    {

    }

    PageHeading
    {
        title: dispStr.readDispStr(DispStr.MSG_DELETE_CREDENTIALS_129) /*"Delete Credential"*/
    }

    UserInfoEntryRect
    {
        id : userInfoId
        anchors.top : parent.top
        anchors.topMargin: topMarginUserInfoRect
        userIdLen:15
    }

    MxRectangleFrame
    {
        id : enrollInfoRect
        width : 280             //popup height-width
        height : 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : userInfoId.bottom
        anchors.topMargin : 5
        visible : alphaNumKeypadF ? false : true

        Row
        {
            id : rowId
            anchors.centerIn : parent
            TextLabel
            {
                id: credentialTypeLabel
                label: dispStr.readDispStr(DispStr.MSG_CREDENTIAL_827) /*"Credential"*/
                width : labelWidth
            }

            SelectionBox
            {
                id : credential
                options : internalFpSupport ?
                          [ dispStr.readDispStr(DispStr.MSG_READ_ONLY_CARD_483) /*"Read Only Card"*/,
                            dispStr.readDispStr(DispStr.MSG_SMART_CARD_565) /*"Smart card"*/ ,
                            dispStr.readDispStr(DispStr.MSG_FINGER_ONLY_238) /*"Finger Only"*/ ,
                            dispStr.readDispStr(DispStr.MSG_FP_THEN_CARD_249) /*"FP then Card"*/,
                            dispStr.readDispStr(DispStr.MSG_FACE_ONLY_1046) /*Face Only*/
                          ]:
                          [
                            dispStr.readDispStr(DispStr.MSG_READ_ONLY_CARD_483) /*"Read Only Card"*/,
                            dispStr.readDispStr(DispStr.MSG_SMART_CARD_565) /*"Smart card"*/
                          ]

                title :  dispStr.readDispStr(DispStr.MSG_SELECT_CREDENTIAL_511) /*"Select Credential"*/
                identity : "credential"
                iconIdxList: internalFpSupport ?
                [
                    EnumClass.ICON_ENROLL_READ_ONLY_CARD,
                    EnumClass.ICON_ENROLL_SMART_CARD,
                    EnumClass.ICON_ENROLL_FINGER_ONLY,
                    EnumClass.ICON_ENROLL_FP_THEN_CARD,
					EnumClass.ICON_ENROLL_FACE
                ]:
                [
                    EnumClass.ICON_ENROLL_READ_ONLY_CARD,
                    EnumClass.ICON_ENROLL_SMART_CARD,
                ]
                isIcon : true
            }
        }
    }

    ButtonElement
    {
        id : buttons
        isInImageFormat:true
        centerBtnImagePath:imageLocation + "/ButtonIcons/delete.png"
        buttonCenterVisibleF: true
        //buttonCenterText: "Delete"
        anchors.bottomMargin: (enrollInfoRect.visible ? 60 : 165)
        onButtonCenterPress:
        {
            generatePayloadForDeleteUser()
        }
    }

    Connections
    {
        target: alphaNumKeypadId
        onHidePress:
        {
            if(popUpLoader.source == "")
            {
                askUserNameFromUserId()
            }
        }
    }

    function userSearchResult(tempName)
    {
        var tmpParaList = new Array()

        tmpParaList = tempName.split(String.fromCharCode(deshFsp).toString())

        userInfoId.userId   = tmpParaList[0]
        userInfoId.userName = tmpParaList[1]
        userInfoId.userIdForPhoto = tmpParaList[2]
        userInfoId.loadUserPhoto()
     }

    function askUserNameFromUserId()
    {
        appData.data =  EnumClass.MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID +
                        String.fromCharCode(uiMsgFsp).toString() +
                        userInfoId.userId +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function generatePayloadForDeleteUser()
    {
        var tempDeleteData
        var tempCredentialType

        var tmpDelCredentialIndex = delCredentialIndex;
        if(tmpDelCredentialIndex == 4) /*For face only type*/
        {
              tmpDelCredentialIndex = 7
        }
         /* Duress Finger not supported for delete credential so passed read only card as default value*/
        else if(tmpDelCredentialIndex == 8) /* 8 for Duress*/
        {
              tmpDelCredentialIndex = 0  /* 0 for Read Only Card*/
        }
        tempDeleteData =  EnumClass.MSG_DEL_CRED_VALIDATION_EVENT +
                           String.fromCharCode(uiMsgFsp).toString() +
                           tmpDelCredentialIndex.toString() +
                           String.fromCharCode(uiMsgFsp).toString() +
                           userInfoId.userId +
                           String.fromCharCode(uiMsgFsp).toString() +
                           userInfoId.userName +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()

        //console.log("Delete Credential Data ------>" + tempDeleteData )
        appData.data = tempDeleteData
    }

    function selectionDone(identity,indx)
    {
        if(identity === "credential")
        {
            credential.selectedNo = indx
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {

        PayloadLib.parsePayload(msgId, rcvData)
    }
}
