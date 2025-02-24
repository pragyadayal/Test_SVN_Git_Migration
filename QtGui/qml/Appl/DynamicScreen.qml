import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : mainRectId
    width: lcd_width
    height: lcd_height

    OpacityRectangle
    {

    }

    property string heading       : ""
    property string textInfoLine1 : ""
    property string textInfoLine2 : ""
    property int imgIdx           : 0
    property string imgPath       : ""

    property variant popUpImgSource:
    [
        "InvalidPassword.png",
        "InvalidPassword.png",
        "LateInActive.png",
        "EarlyOutActive.png",
        "LateInStart.png",
        "LateInStop.png",
        "EarlyOutStart.png",
        "EarlyOutStop.png",
        "DoorLockSuccess.png",
        "DoorUnlockSuccess.png",
        "DoorNormalSuccess.png",
        "ClearAlarmSuccess.png",
        "EnrollmentInactive.png",
        "InvalidUserID.png",
        "EnrollmentFailed.png",
        "EnrollmentSuccessful.png",
        "TimeOut.png",
        "WritingSmartCard.gif",
        "ReadingData.gif" ,
        "CardIdExists.png" ,
        "FingerExists.png",
        "WrongSelection.png",
        "ConfigureReader.png",
        "WifiScanning.gif",
        "NoAPFound.png",
        "CredentialDeleted.png",
        "InvalidCardType.png",
        "UsbConnected.png",
        "UsbDisconnected.png",
        "UsbNotRecognized.png",
        "Failed.png",
        "Success.png",
        "InsufficientMemory.png",
        "CredentialExists.png",
        "InsufficientMemory.png",
        "EventsNotFound.png",
        "FirmwareNotFound.png",
        "ConfigurationNotFound.png",
        "UsbNotRecognized.png",
        "Success.png",
        "Failed.png",
        "SystemDefaulting.gif",
        "SystemDefaulted.png",
        "ActDND.png",
        "DeActDND.png",
        "ActDMZ.png",
        "DeActDMZ.png",
        "ZoneLockSuccess.png",
        "ZoneUnlockSuccess.png",
        "ZoneNormalSuccess.png",
        "PanicAlarmSuccess.png",
        "MuteDoorBuzzerSuccess.png",
        "ClearDoorAuxOPSuccess.png",
        "MuteMasterBuzzerSuccess.png",
        "ClearMasterAuxOPSuccess.png",
        "Processing.gif",
        "UnauthorisedUser.png",
        "AlarmInactive.png",
        "DoorDisarm.png",
        "FPSyncProcessing.png",
        "NoExitReader.png",
        "NetworkEstablished.png",
        "NoDataAvailable.png",
		"DoorArmSuccess.png",
        "DoorDisarmSuccess.png",
		"ZoneArmSuccess.png",
        "ZoneDisarmSuceess.png",                 //Speling mistake image name. Don't correct
        "Hourglass.gif",
        "Failed.png",
    ]

    property variant popUpSplFuncImgSource :
    [
        "OfficialIn.png",
        "OfficialOut.png",
        "ShortLeaveIn.png",
        "ShortLeaveOut.png",
        "RegularIn.png",
        "RegularOut.png",
        "BreakIn.png",
        "BreakOut.png",
        "OverTimeIn.png",
        "OverTimeOut.png",
        "EnrollUsers.png",
        "EnrollSpecialCards.png",
        "DeleteCredentials.png",
        "LateInStart.png",
        "LateInStop.png",
        "EarlyOutStart.png" ,
        "EarlyOutStop.png" ,
        "DoorLock.png",
        "DoorUnlock.png",
        "DoorNormal.png",
        "ClearAlarm.png",
        "ItemsSoldOut.png",
        "ItemsAvailable.png"
    ]

    property variant popUpSplFuncPanelImgSource :
    [
        "OfficialIn.png",
        "OfficialOut.png",
        "ShortLeaveIn.png",
        "ShortLeaveOut.png",
        "RegularIn.png",
        "RegularOut.png",
        "BreakIn.png",
        "BreakOut.png",
        "OverTimeIn.png",
        "OverTimeOut.png",
        "Dummy.png",                        //guardTour
        "PanicAlarm.png",
        "EnrollUsers.png",
        "EnrollSpecialCards.png",
        "DeleteCredentials.png",
        "LateInStart.png",
        "LateInStop.png",
        "EarlyOutStart.png" ,
        "EarlyOutStop.png" ,
        "UserProfile.png",
        "ActDND.png",
        "DeActDND.png",
        "ActDMZ.png",
        "DeActDMZ.png",
        "DoorLock.png",
        "DoorUnlock.png",
        "DoorNormal.png",
        "ZoneLock.png",
        "ZoneUnlock.png",
        "ZoneNormal.png",
        "MuteDoorBuzzer.png",
        "MuteMasterBuzzer.png",
        "ClearDoorAuxOP.png",
        "ClearMasterAuxOP.png",
		"DoorArmSuccess.png",
        "DoorDisarmSuccess.png",
		"ZoneArmSuccess.png",
        "ZoneDisarmSuccess.png",
        "ClearAlarm.png",

        //NOTE: image for splFunc "SHOW_YOUR_CRED" will always be at last,
        //      add new splFunc & image above it
        //Blank image is put intentionaly to match with enum no.
        "",
        "ShowYourId.png",
	]

    property variant panelDegradedModeImgSource :
    [
        "/AllowDenyInput/AccessAllowed.png",
        "/AllowDenyInput/AccessDenied.png",
        "/PopUp/ReadingData.gif",
        "/AllowDenyInput/PlaceFinger.gif",
        "/AllowDenyInput/CardNotIdentified.png",
        "/PopUp/CardReadFailed.png",
        "/PopUp/InvalidPassword.png",
        "/PopUp/TimeOut.png",
        "/PopUp/SystemDefaulting.gif",
        "/PopUp/SystemDefaulted.png",
        "/PopUp/Success.png",
        "/PopUp/Failed.png",
        "/PopUp/CommunicationError.png",
        "/PopUp/Success.png",
        "/PopUp/Failed.png",
        "/PopUp/WifiScanning.gif",
        "/PopUp/Processing.gif",
        "/PopUp/NoAPFound.png",
        "/PopUp/NetworkEstablished.png",
        "/PopUp/Hourglass.gif",
    ]

    MxRectangleFrame
    {
        id : popUpRect
        height : msgBoxHeight
        width : msgBoxWidth
        anchors.centerIn: parent
        isBlankBox : true

        Column
        {
            spacing : 5
            anchors.centerIn: parent

            MxText
            {
                id : popUpHeading
                width : msgBoxTextWidth
                color : "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                text : heading
                visible : (text.length > 0) ? true : false
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            AnimatedImage
            {
                id : popUpImg
                anchors.horizontalCenter: parent.horizontalCenter
                source : imgPath
                smooth : true
                cache : false
            }

            MxText
            {
                id : popUpText1
                width : msgBoxTextWidth
                color : "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                text : textInfoLine1
                visible : (text.length > 0) ? true : false
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
            MxText
            {
                id : popUpText2
                width : msgBoxTextWidth
                color : "white"
                font.pointSize: fontSizePopUpLabel
                anchors.horizontalCenter: parent.horizontalCenter
                text : textInfoLine2
                visible : (text.length > 0) ? true : false
             //   wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
        heading = tmpPayload[0]
        imgIdx = tmpPayload[1]
        textInfoLine1 = tmpPayload[2]
        var isSplFunc

       if(textInfoLine1 == dispStr.readDispStr(DispStr.MSG_DEVICE_IS_BUSY_977))
       {
           popUpText1.font.bold = true
           popUpText1.font.pointSize =19
       }

        switch(msgId)
        {
            case EnumClass.GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN:
            {
                isSplFunc = true
                textInfoLine2 = ""
                var escF = parseInt(tmpPayload[3],10)
                var pinF = parseInt(tmpPayload[4],10)

                setFooterBar(pinF, escF)
                break
            }

            case EnumClass.GUI_MSG_DYNAMIC_POP_UP_SCREEN:
            {
                isSplFunc = false
                textInfoLine2 = tmpPayload[3]
                setFooterConfigIdx(EnumClass.FOOTER_HOME)
                var footerF = parseInt(tmpPayload[4],10)
                footerBarHideF = (footerF ? true : false)
                break
            }
        }

        if(isSplFunc)
        {// For Special functions in direct door and panel door
            //console.log("For Special functions in direct door and panel door....ImageIdx----->" +imgIdx)
            imgPath = imageLocation + "/SpecialFunction/" + (doorFirmwareType ? popUpSplFuncPanelImgSource[imgIdx]: popUpSplFuncImgSource[imgIdx])

            if(enrollPopUpActive !== true)
            {
                enrollPopUpActive = true
            }
        }
        else if(doorFirmwareType && (currentDoorStatus != EnumClass.DOOR_ONLINE))
        {// For Degraded mode or N/w Fault
            //console.log("For Degraded mode or N/w Fault....ImageIdx----->" +imgIdx)
            imgPath = imageLocation + mainRectId.panelDegradedModeImgSource[imgIdx]
        }
        else
        {// Pop up for Direct door and Onine panel door
           // console.log("Pop up for Direct door and Onine panel door....ImageIdx----->" +imgIdx)
            imgPath = imageLocation + "/PopUp/" + mainRectId.popUpImgSource[imgIdx]
        }
        popUpImg.playing = true
    }
}
