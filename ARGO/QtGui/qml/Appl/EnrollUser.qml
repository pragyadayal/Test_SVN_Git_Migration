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

    property int maxUserCard
    property int maxUserFP
    property int maxUserFace
    property int maxUserDuressFP
    property alias   typeSelectedNo : enrollmentType.selectedNo
    //spacingRelated
    property int topMarginEnrollUser : 60
    property int labelWidth : 105
    property int isDuressFpCnt

    OpacityRectangle
    {

    }

    PageHeading
    {
        id : heading
        title: dispStr.readDispStr(DispStr.MSG_ENROLL_USER_195) /*"Enroll User"*/
    }

    UserInfoEntryRect
    {
        id : userInfoId
        anchors.top : parent.top
        anchors.topMargin: topMarginEnrollUser
        userIdLen :15
    }

    MxRectangleFrame
    {
        id : enrollInfoRect
        width : 280             //popup height-width
        height : (enrollmentType.selectedNo == 3) ? 100 : 68
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: userInfoId.bottom
        anchors.topMargin: 5
        visible : alphaNumKeypadF ? false : true

        Column
        {
            anchors.centerIn: parent
            spacing: 7
            Row
            {
                id : row1
                TextLabel
                {
                    id: enrollType
                    label: dispStr.readDispStr(DispStr.MSG_ENROLLMENT_TYPE_200)  /*"Enrollment Type"*/
                    width : labelWidth
                }
                SelectionBox
                {
                    id : enrollmentType
                    options : internalFpSupport ?
                            [
                                 dispStr.readDispStr(DispStr.MSG_READ_ONLY_CARD_483) /*"Read Only Card"*/,
                                 dispStr.readDispStr(DispStr.MSG_SMART_CARD_565) /*"Smart card"*/ ,
                                 dispStr.readDispStr(DispStr.MSG_FINGER_ONLY_238) /*"Finger Only"*/ ,
                                 dispStr.readDispStr(DispStr.MSG_FP_THEN_CARD_249) /*"FP then Card"*/,
                                 dispStr.readDispStr(DispStr.MSG_FACE_ONLY_1046) /*Face Only*/,
		 dispStr.readDispStr(DispStr.MSG_DURESS_FINGER_1078) /*Duress Fp*/
                             ]:
                             [
                                 dispStr.readDispStr(DispStr.MSG_READ_ONLY_CARD_483) /*"Read Only Card"*/,
                                 dispStr.readDispStr(DispStr.MSG_SMART_CARD_565) /*"Smart card"*/,
                                 dispStr.readDispStr(DispStr.MSG_FACE_ONLY_1046) /*Face Only*/
                             ]
                    iconIdxList: internalFpSupport ?
                    [
                        EnumClass.ICON_ENROLL_READ_ONLY_CARD,
                        EnumClass.ICON_ENROLL_SMART_CARD,
                        EnumClass.ICON_ENROLL_FINGER_ONLY,
                        EnumClass.ICON_ENROLL_FP_THEN_CARD,
	        EnumClass.ICON_ENROLL_FACE,
	        EnumClass.ICON_ENROLL_DURESS_FP
                    ]:
                    [
                        EnumClass.ICON_ENROLL_READ_ONLY_CARD,
                        EnumClass.ICON_ENROLL_SMART_CARD,
                        EnumClass.ICON_ENROLL_FACE
                    ]
                    title : dispStr.readDispStr(DispStr.MSG_SELECT_ENROLLMENT_TYPE_515) /*SELECT_ENROLLMENT_TYPE*/
                    identity : "enrollmentType"
                    isIcon: true
                }
            }
            Row
            {
                id : row2
                visible: internalFpSupport ? (((enrollmentType.selectedNo > 1) && (enrollmentType.selectedNo != 4) && (enrollmentType.selectedNo != 5) ) ? true : false ) : false
                TextLabel
                {
                    id: finger
                    label: dispStr.readDispStr(DispStr.MSG_NO_OF_FINGERS_397) /*"No. of Fingers"*/
                    width : labelWidth
                }
                SelectionBox
                {
                    id : noOfFinger
                    title : dispStr.readDispStr(DispStr.MSG_SELECT_NO_OF_FINGERS_521) /*"Select No. Of Fingers"*/
                    identity : "noOfFinger"
                    isSpinBox: true
                    validation : maxUserFP
                }
            }
            Row
            {
                id : row3
                visible: ((((enrollmentType.selectedNo == 2) || (enrollmentType.selectedNo == 4)) || (enrollmentType.selectedNo == 5)) ? false : true )
                TextLabel
                {
                    id: card
                    label: dispStr.readDispStr(DispStr.MSG_NO_OF_CARDS_386) /*"No. Of Cards"*/
                    width : labelWidth
                }
                SelectionBox
                {
                    id : noOfCards
                    title :  dispStr.readDispStr(DispStr.MSG_SELECT_NO_OF_CARDS_520)  /*"Select No. Of Cards"*/
                    identity : "noOfCards"
                    isSpinBox: true
                    validation : maxUserCard
                }
            }
            Row
            {
                id : row4
                visible: internalFpSupport ? ((enrollmentType.selectedNo == 4)  ? true : false) : ((enrollmentType.selectedNo == 2) ? true : false)
                TextLabel
                {
                    id: face
                    label: dispStr.readDispStr(DispStr.MSG_NO_OF_FACES_1047) /*"No. Of Faces"*/
                    width : labelWidth
                }
                SelectionBox
                {
                    id : noOfFaces
                    title :  dispStr.readDispStr(DispStr.MSG_SELECT_NO_OF_FACES_1048)  /*"Select No. Of Faces"*/
                    identity : "noOfFaces"
                    isSpinBox: true
                    validation : maxUserFace
                }
            }
            Row
            {
                id : row5
                visible: ((enrollmentType.selectedNo == 5) ? true : false )
                TextLabel
                {
                    id: duressFp
                    label: dispStr.readDispStr(DispStr.MSG_NO_OF_FINGERS_397) /*"No. of Fingers"*/
                    width : labelWidth
                }
                SelectionBox
                {
                    id : noOfduressFingers
                    title :  dispStr.readDispStr(DispStr.MSG_SELECT_NO_OF_FINGERS_521)  /*"Select No. Of Fingers"*/
                    identity : "noOfduressFingers"
                    isSpinBox: true
                    validation : maxUserDuressFP
                }
            }
        }
    }

    ButtonElement
    {
        id : buttons
		isInImageFormat:true
        centerBtnImagePath: imageLocation + "/ButtonIcons/enroll.png"
        buttonCenterVisibleF: true

        //buttonCenterText: "Enroll"
        onButtonCenterPress:
        {
            generatePayloadForEnrollment()
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

    function selectionDone(identity,indx)
    {
        if(identity === "enrollmentType")
        {
            //Need to update this condition when new enrollment type will be added
            if(indx < 6)
                enrollmentType.selectedNo = indx
            if(enrollmentType.selectedNo == 5){
	   				isDuressFpCnt = 1 
            }
            else{
	    			isDuressFpCnt = 0
            }
        }
        else if(identity === "noOfFinger")
        {
            noOfFinger.selectedNo = indx
        }
        else if (identity === "noOfCards")
        {
            noOfCards.selectedNo = indx
        }
        else if (identity === "noOfFaces")
        {
            noOfFaces.selectedNo = indx
        }
        else if (identity === "noOfduressFingers")
        {
            noOfduressFingers.selectedNo = indx
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

    function askUserNameFromUserId()
    {
        appData.data =  EnumClass.MSG_ENROL_SEARCH_USER_NAME_FROM_ID +
                        String.fromCharCode(uiMsgFsp).toString() +
                        userInfoId.userId +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function generatePayloadForEnrollment()
    {
        var tempFingerData
        var tempCardData
        var tempEnrollData
        var tempFaceData

        if(isDuressFpCnt == 1)
        {
              tempFingerData = noOfduressFingers.selectedNo
        }
        else
        {
              tempFingerData = noOfFinger.selectedNo
        }
        tempCardData   = noOfCards.selectedNo
        tempFaceData = noOfFaces.selectedNo

        tempEnrollData =  EnumClass.MSG_ENROL_USER_VALIDATION_EVENT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        typeSelectedNo.toString() +
                        String.fromCharCode(uiMsgFsp).toString() +
                        tempFingerData.toString() +
                        String.fromCharCode(uiMsgFsp).toString() +
                        tempCardData.toString() +
                        String.fromCharCode(uiMsgFsp).toString() +
                        userInfoId.userId +
                        String.fromCharCode(uiMsgFsp).toString() +
                        userInfoId.userName +
                        String.fromCharCode(uiMsgFsp).toString() +
						tempFaceData.toString() +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()

        //console.log("Data Given For Enrollment----->" + tempEnrollData)
        appData.data = tempEnrollData

    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
