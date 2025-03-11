///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Gate Controller
//   Owner        : Kuldip Gor
//   File         : payloadHandler.js
//   Description  :
//////////////////////////////////////////////////////////////////////////////

//constants
var UI_MSG_FSP = 0x05;
var SLASH_FSP = '/';
var TIME_FSP = ':';
var VALUE_FSP = 0x04;
var DATE_FSP = '-';
var DESH_FSP = 0x03;
var DOT_FSP  = '.';
var MAX_DURESS_FP_PER_USER = 2;
//////////////////////////////////////////////////////////////////////////////
//parsePayload
//Param:
//  IN : reqType, devPayload
//  OUT:NONE
//  Returns: NONE
//  Description:
//////////////////////////////////////////////////////////////////////////////

function parsePayload(msgId, payload)
{
    var tmpPayload = new Array();

    switch(msgId)
    {

        default:
        {
            qmlLog.fatal("parsePayload: Invalid MsgID: 0x" + msgId.toString(16))
            break;
        }
        case EnumClass.GUI_MSG_LOG_INFO:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            qmlLog.debug("GUI_MSG_LOG_INFO: logSeverity: [" + parseInt(tmpPayload[0], 10) + "]")
            qmlLog.debug("GUI_MSG_LOG_INFO: sysLogEnableF: [" + parseInt(tmpPayload[1], 10) + "]")
            qmlLog.debug("GUI_MSG_LOG_INFO: sysLogServerAddr: [" + tmpPayload[2] + "]")
            qmlLog.debug("GUI_MSG_LOG_INFO: sysLogServerPort: [" + parseInt(tmpPayload[3], 10) + "]")
            qmlLog.debug("GUI_MSG_LOG_INFO: deviceMacAddr: [" + tmpPayload[4] + "]")
            qmlLog.debug("GUI_MSG_LOG_INFO: timeDiff: [" + tmpPayload[5] + "]")
            shift = parseInt(tmpPayload[5],10)
            shift = (shift/(60*60))
            qmlLog.updateDebugCnfg(parseInt(tmpPayload[0], 10), parseInt(tmpPayload[1], 10), tmpPayload[2], parseInt(tmpPayload[3], 10), tmpPayload[4], shift)
            break;
        }

        case EnumClass.GUI_MSG_FR_SERV_ENABLE:
        {
           tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
           frServerEnableF = parseInt(tmpPayload[0], 10)
           frStreamDisplayF = false
           qmlLog.debug("GUI_MSG_FR_SERV_ENABLE: frServerEnableF: [" + frServerEnableF + "] frStreamDisplayF: [" + frStreamDisplayF + "]")
           break;
        }

        case EnumClass.GUI_MSG_NOTICE_TEXT:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            noticeText1 = tmpPayload[0]
            noticeText2 = tmpPayload[1]
            noticeImageIndex = parseInt(tmpPayload[2],10)
            pinButtonVisibleF = parseInt(tmpPayload[3],10)
            escButtonVisibleF = parseInt(tmpPayload[4],10)
            break
        }

        case EnumClass.GUI_MSG_MENU_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            doorApplicationType = parseInt(tmpPayload[0],10)
            break
        }
        case EnumClass.GUI_MSG_RELOAD_IMAGE:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            frStreamDisplayF = parseInt(tmpPayload[0],10)
            break
        }
        case EnumClass.GUI_MSG_FR_STREAM_STATUS:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            frStreamDisplayF = parseInt(tmpPayload[0],10)
            break
        }

        case EnumClass.GUI_MSG_GOING_IN_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            regularInEnable = tmpPayload[0]
            officialInEnable = tmpPayload[1]
            postBreakInEnable = tmpPayload[2]
            shortLeaveInEnable = tmpPayload[3]
            overtimeInEnable = tmpPayload[4]
            break
        }

        case EnumClass.GUI_MSG_GOING_OUT_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            regularOutEnable = tmpPayload[0]
            officialOutEnable = tmpPayload[1]
            preBreakOutEnable = tmpPayload[2]
            shortLeaveOutEnable = tmpPayload[3]
            overtimeOutEnable = tmpPayload[4]
            break
        }

        case EnumClass.GUI_HRD_MENU_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            lateInEnable = tmpPayload[0]
            earlyOutEnable = tmpPayload[1]
            enrollmentEnable = tmpPayload[2]
            break
        }

        case EnumClass.GUI_ADMIN_MENU_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            lateInEnable = tmpPayload[0]
            earlyOutEnable = tmpPayload[1]
            enrollmentEnable = tmpPayload[2]
            break
        }

        case EnumClass.GUI_MSG_ENROLLMENT_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            enrollUserF   = tmpPayload[0]
            enrollSplFuncF= tmpPayload[1]
            deleteCredF   = tmpPayload[2]
            break
        }

        case EnumClass.GUI_MSG_STATUSBAR_SCREEN:
        {
            var tempStatus

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            qmlLog.debug("GUI_MSG_STATUSBAR_SCREEN: " + tmpPayload)

            tempStatus = tmpPayload[0]
            if(tempStatus.length)
            {
                statusBarId.ethernetF = parseInt(tmpPayload[0],10)
            }

            tempStatus = tmpPayload[1]
            if(tempStatus.length)
            {
                statusBarId.wifiStatus = parseInt(tmpPayload[1],10)
            }


            if(!doorFirmwareType)
            {//direct door
                tempStatus = tmpPayload[2]
                if(tempStatus.length)
                {
                    statusBarId.modemStatus = parseInt(tmpPayload[2],10)
                }

                tempStatus = tmpPayload[3]
                if(tempStatus.length)
                {
                    statusBarId.alarmF = parseInt(tmpPayload[3],10)
                }

                tempStatus = tmpPayload[4]
                if(tempStatus.length)
                {
                    statusBarId.doorLockUnlock = parseInt(tmpPayload[4],10)
                }

                tempStatus = tmpPayload[5]
                if(tempStatus.length)
                {
                    statusBarId.serverF = parseInt(tmpPayload[5],10)

                }

                tempStatus = tmpPayload[6]
                if(tempStatus.length)
                {
                    statusBarId.usbF = parseInt(tmpPayload[6],10)
                }

                tempStatus = tmpPayload[7]
                if(tempStatus.length)
                {
                    statusBarId.splFuncStatus = parseInt(tmpPayload[7],10)
                }

                tempStatus = tmpPayload[8]
                if(tempStatus.length)
                {
                    goingInEnableF = parseInt(tmpPayload[8],10)
                }

                tempStatus = tmpPayload[9]
                if(tempStatus.length)
                {
                    goingOutEnableF = parseInt(tmpPayload[9],10)
                }

                tempStatus = tmpPayload[10]
                if(tempStatus.length)
                {
                    statusBarId.downloadingF = parseInt(tmpPayload[10],10)
                }

                tempStatus = tmpPayload[11]
                if(tempStatus.length)
                {
                    statusBarId.bluetoothStatus = parseInt(tmpPayload[11],10)
                }

                tempStatus = tmpPayload[12]
                if(tempStatus.length)
                {
                    statusBarId.temperatureSensorStatusF = parseInt(tmpPayload[12],10)
                }

				tempStatus = tmpPayload[13]
                if(tempStatus.length)
                {
                    statusBarId.temperatureString = tmpPayload[13]
                }
                else
                {
                    statusBarId.temperatureString = "";
                }

                tempStatus = tmpPayload[14]
                if(tempStatus.length)
                {
                    statusBarId.temperatureCnfgEnable = tmpPayload[14]
                }
                tempStatus = tmpPayload[15]
                if(tempStatus.length)
                {
                    statusBarId.fsmSyncStatus = parseInt(tmpPayload[15],10)
    		    console.log("#### DEBUG statusBarId.fsmSyncStatus " + statusBarId.fsmSyncStatus)
                }
            }
            else
            {//panel door
                tempStatus = tmpPayload[2]
                if(tempStatus.length)
                {
                    statusBarId.networkFaultF = parseInt(tmpPayload[2],10)
                }

                tempStatus = tmpPayload[3]
                if(tempStatus.length)
                {
                    statusBarId.degradeModeF = parseInt(tmpPayload[3],10)
                }

                updateDoorStatus()

                tempStatus = tmpPayload[4]
                if(tempStatus.length)
                {
                    statusBarId.doorLockUnlock = parseInt(tmpPayload[4],10)
                }

                tempStatus = tmpPayload[5]
                if(tempStatus.length)
                {
                    statusBarId.alarmF = parseInt(tmpPayload[5],10)
                }

                tempStatus = tmpPayload[6]
                if(tempStatus.length)
                {
                    statusBarId.splFuncStatus = parseInt(tmpPayload[6],10)

                }

                tempStatus = tmpPayload[7]
                if(tempStatus.length)
                {
                    statusBarId.dndF = parseInt(tmpPayload[7],10)
                }

                tempStatus = tmpPayload[8]
                if(tempStatus.length)
                {
                    statusBarId.deadManZoneF = parseInt(tmpPayload[8],10)
                }

                tempStatus = tmpPayload[9]
                if(tempStatus.length)
                {
                    statusBarId.downloadingF = parseInt(tmpPayload[9],10)
                }

                tempStatus = tmpPayload[10]
                if(tempStatus.length)
                {
                    statusBarId.bluetoothStatus = parseInt(tmpPayload[10],10)
                }

                tempStatus = tmpPayload[11]
                if(tempStatus.length)
                {
                    statusBarId.temperatureSensorStatusF = parseInt(tmpPayload[11],10)
                }

				tempStatus = tmpPayload[12]
                if(tempStatus.length)
                {
                    statusBarId.temperatureString = tmpPayload[12]
                }
                else
                {
                    statusBarId.temperatureString = "";
                }

                tempStatus = tmpPayload[13]
                if(tempStatus.length)
                {
                    statusBarId.temperatureCnfgEnable = tmpPayload[13]
                }
                tempStatus = tmpPayload[14]
                if(tempStatus.length)
                {
                    statusBarId.degradeModeType = parseInt(tmpPayload[14],10)
                }
                updateDoorStatus()

			goingInEnableF = true
			goingOutEnableF = true
            }
            break
        }

        case EnumClass.GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            doorLockF = tmpPayload[0]
            doorUnlockF = tmpPayload[1]
            doorNormalF = tmpPayload[2]
            doorArmF = tmpPayload[3]
            doorDisarmF = tmpPayload[4]
            break
        }

        case EnumClass.GUI_MSG_SYS_STATUS_SCREEN:
        {
            var tempLabelValueIdle = new Array()
            var index = 0

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            for(index = 0 ; index < 2 ; index++)
            {
                tempLabelValueIdle[index] = tmpPayload[index]
            }
            for(index = 2 ; index < labelNameIdle.length ; index++)
            {
                tempLabelValueIdle[index] = ""
            }

            labelValueIdle = tempLabelValueIdle

            break
        }

        case EnumClass.GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN :
        {
            var tempLabelValueConnectivity = new Array()
            var index = 0

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            for(index = 0 ; index < labelNameConnectivity.length ; index++)
            {
                tempLabelValueConnectivity[index] = tmpPayload[index]
            }
            labelValueConnectivity = tempLabelValueConnectivity
            break
        }

        case EnumClass.GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN :
        {
            var tempLabelValueDeviceReaders = new Array()
            var index = 0

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            for(index = 0 ; index < labelNameReaders.length ; index++)
            {
                tempLabelValueDeviceReaders[index] = tmpPayload[index]
            }
            labelValueReaders = tempLabelValueDeviceReaders
            break

        }

        case EnumClass.GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN :
        {
            var tempLabelValueDeviceAlarms = new Array()
            var index = 0

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            for(index = 0 ; index < labelNameAlarms.length ; index++)
            {
                tempLabelValueDeviceAlarms[index] = tmpPayload[index]
            }
            labelValueAlarms = tempLabelValueDeviceAlarms
            break

        }

        case EnumClass.GUI_MSG_ADMIN_MENU_SCREEN :
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            if(!doorFirmwareType)
            {//Direct Door
                usbAvailableF     = parseInt(tmpPayload[0],10)
                lateInEnable      = tmpPayload[1]
                lateInStartStop   = tmpPayload[2]
                earlyOutEnable    = tmpPayload[3]
                earlyOutStartStop = tmpPayload[4]
                clearAlarmF       = parseInt(tmpPayload[5],10)
                tempSensorStatusF = parseInt(tmpPayload[6])

            }
            else if(currentDoorStatus == 0)
            {// Panel Door Online       //EnumClass.DOOR_ONLINE
                lateInEnable      = tmpPayload[0]
                lateInStartStop   = tmpPayload[1]
                earlyOutEnable    = tmpPayload[2]
                earlyOutStartStop = tmpPayload[3]
                userProfileF      = tmpPayload[4]
            }
            break
        }

        case EnumClass.GUI_MSG_HRD_MENU_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            lateInEnable    =  tmpPayload[0]
            lateInStartStop = tmpPayload[1]
            earlyOutEnable  = tmpPayload[2]
            earlyOutStartStop = tmpPayload[3]
            if(doorFirmwareType) //Panel Door -->1 Direct door-->0
            {
                userProfileF      = tmpPayload[4]
            }
            break
        }


        case EnumClass.GUI_MSG_ADMIN_SETTING_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            wifiModulePresentF = parseInt(tmpPayload[0],10)
            samayModeF = parseInt(tmpPayload[1],10)

            break
        }

        case EnumClass.GUI_MSG_EHTERNET_CFG_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            ipAddress.setIpFieldValue(tmpPayload[0])
            subNet.setIpFieldValue(tmpPayload[1])
            gateWay.setIpFieldValue(tmpPayload[2])

            // DoorApplication Type DirectDoor
            if(doorFirmwareType == false)
            {
                preferredDns.setIpFieldValue(tmpPayload[3])
                alternateDns.setIpFieldValue(tmpPayload[4])
                mode.selectedNo = parseInt(tmpPayload[5])
            }
            break
        }

        case EnumClass.GUI_MSG_CAFE_USER_ALLOWED:
        {
            var tempText = new Array();

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            userInfoRectId.userId = tmpPayload[0]
            userInfoRectId.userName = tmpPayload[1]
            userInfoRectId.userPhotoF = tmpPayload[2]
            imgIdx = parseInt(tmpPayload[8], 10)
            userInfoRectId.userIdForPhoto = tmpPayload[9]
            userInfoRectId.loadUserPhoto()

            cafeTextSepF = parseInt(tmpPayload[10],10)
            if(cafeTextSepF == true)
            {
                if(tmpPayload[3].length > 0)
                {
                    tempText = tmpPayload[3].split(String.fromCharCode (VALUE_FSP).toString())
                    cafeRow1.text1  = tempText[0]
                    cafeRow1.text2  = " : "
                    cafeRow1.text3  = tempText[1]
                }
                if(tmpPayload[4].length > 0)
                {
                    tempText = tmpPayload[4].split(String.fromCharCode (VALUE_FSP).toString())
                    cafeRow2.text1  = tempText[0]
                    cafeRow2.text2  = " : "
                    cafeRow2.text3 = tempText[1]
                }
                if(tmpPayload[5].length > 0)
                {
                    tempText = tmpPayload[5].split(String.fromCharCode (VALUE_FSP).toString())
                    cafeRow3.text1  = tempText[0]
                    cafeRow3.text2  = " : "
                    cafeRow3.text3 = tempText[1]
                }
                if(tmpPayload[6].length > 0)
                {
                    tempText = tmpPayload[6].split(String.fromCharCode (VALUE_FSP).toString())
                    cafeRow4.text1  = tempText[0]
                    cafeRow4.text2  = " : "
                    cafeRow4.text3 = tempText[1]
                }
                if(tmpPayload[7].length > 0)
                {
                    tempText = tmpPayload[7].split(String.fromCharCode (VALUE_FSP).toString())
                    cafeRow5.text1  = tempText[0]
                    cafeRow5.text2  = " : "
                    cafeRow5.text3 = tempText[1]
                }
            }
            else
            {
                cafeAllowText1.text = tmpPayload[3]
                cafeAllowText2.text = tmpPayload[4]
                cafeAllowText3.text = tmpPayload[5]
                cafeAllowText4.text = tmpPayload[6]
                cafeAllowText5.text = tmpPayload[7]
            }

            break
        }

        case EnumClass.GUI_MSG_SET_MMC_DEV_PATH:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            userImagePath = tmpPayload[0]
            break
        }

        case EnumClass.GUI_MSG_PIN_ENTER_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            // console.log(tmpPayload)
            showPin = tmpPayload[0]
            break;
        }

        case EnumClass.GUI_MSG_DEL_CRED_GET_NAME_FROM_ID:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            if(tmpPayload)
            {
                userInfoId.userName = tmpPayload[0] ? tmpPayload[0] : dispStr.readDispStr(DispStr.MSG_USER_NAME_1013)
                userInfoId.userIdForPhoto = tmpPayload[1] ? tmpPayload[1] : ""
                if(tmpPayload[0].length == 0)
                {
                    userInfoId.userId = ""
                }
                userInfoId.loadUserPhoto()
            }
            break
        }

        case EnumClass.GUI_MSG_APPL_MODE_INFO:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            doorApplicationType = tmpPayload[0]
            break;
        }

        case EnumClass.GUI_MSG_ENROLL_USER_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            enrollmentType.selectedNo = parseInt(tmpPayload[0],10)
	    //Internal finger print reader Supported or not(Condition Add for ARGO CAX Door)
            if(internalFpSupport == 0)
            {                
                if(enrollmentType.selectedNo == 4)
                {
                    enrollmentType.selectedNo = 2;
                }
            }

            if(enrollmentType.selectedNo == 5){
				isDuressFpCnt = 1
            }
            else
            {
				isDuressFpCnt = 0
            }
            noOfFinger.selectedNo = parseInt(tmpPayload[1],10)
            noOfCards.selectedNo = parseInt(tmpPayload[2],10)
            maxUserCard = parseInt(tmpPayload[3],10)
            
            maxUserFP = parseInt(tmpPayload[4],10)
            if(maxUserFP > MAX_DURESS_FP_PER_USER)
            {
	maxUserDuressFP = MAX_DURESS_FP_PER_USER
            }
            else
            {
	maxUserDuressFP = parseInt(tmpPayload[4],10)
            }
            
            userInfoId.enrollIdType = parseInt(tmpPayload[5],10)
			maxUserFace = parseInt(tmpPayload[8],10)
            //noOfFaces.selectedNo = 1
            noOfFaces.selectedNo = parseInt(tmpPayload[9],10)
            if(noOfFinger.selectedNo > MAX_DURESS_FP_PER_USER)
            {
	noOfduressFingers.selectedNo = MAX_DURESS_FP_PER_USER
            }
            else
            {
	noOfduressFingers.selectedNo = parseInt(tmpPayload[1],10)
            }

            if(tmpPayload[6].length > 0)
            {
                userInfoId.userId = tmpPayload[6]
            }
            else
            {
                userInfoId.userId = ""
            }
            
            if(tmpPayload[7].length > 0)
            {
                userInfoId.userName = tmpPayload[7]
            }
            else
            {
                userInfoId.userName = dispStr.readDispStr(DispStr.MSG_USER_NAME_1013)
            }

            if(userInfoId.enrollIdType === 1)
            {
                userInfoId.userIdWaterMark = dispStr.readDispStr(DispStr.MSG_REFERENCE_ID_939)
            }
            else
            {
                userInfoId.userIdWaterMark = dispStr.readDispStr(DispStr.MSG_USER_ID_626)
            }

            break
        }

        case EnumClass.GUI_MSG_SEARCHED_DEL_CRED_USER_LIST:
        case EnumClass.GUI_MSG_SEARCHED_ENROLL_USER_LIST:
        {
            var index
            var temp = new Array()

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            noOfUsers = parseInt(tmpPayload[0],10)
            for(index =0 ; index < noOfUsers ; index++)
            {
                temp[index] = tmpPayload[index+1]
                //console.log(temp[index])
            }
            userNamelist = temp
            break
        }


        case EnumClass.GUI_MSG_MODEM_PROFILE_SCREEN:
        {
            var profileindex
            var tempProfileConnectedF
            var tempData = new Array()
            var tempProfileEnableF = new Array()
            var tempBroadbandData = new Array()


            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            noOfProfiles = parseInt(tmpPayload[0],10)
            for(profileindex =0 ; profileindex < noOfProfiles ; profileindex++)
            {
                tempData = tmpPayload[profileindex+1].split(String.fromCharCode (DESH_FSP).toString())
                tempProfileEnableF[profileindex] = parseInt(tempData[0],10)
                tempBroadbandData[profileindex] = tempData[1]
            }
            activateProfile = tempProfileEnableF
            profileName = tempBroadbandData
            tempProfileConnectedF = parseInt(tmpPayload[profileindex + 1],10)
            if(tempProfileConnectedF == 2)
            {
                profileConnectedF = true                      //temp = 0 -->No modem
            }                                               //     = 1 -->Not Connected
            else                                            //     = 2 -->Connected
            {
                profileConnectedF = false
            }


            break
        }

        case EnumClass.GUI_MSG_MODEM_PROFILE_DATA:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            currentProfileIdx = parseInt(tmpPayload[0],10)
            profileActivateF = parseInt(tmpPayload[1],10)
            prflSelCheck     = parseInt(tmpPayload[1],10)
            profileName.value = tmpPayload[2]
            dialNumber.value = tmpPayload[3]
            userName.value = tmpPayload[4]
            password.value = tmpPayload[5]
            serviceType.selectedNo = parseInt(tmpPayload[6],10)
            accessPointNetwork.value = tmpPayload[7]

            if((parseInt(tmpPayload[8],10) == 2) && (profileActivateF == true)) //Connected
            {
                modemIpAddress.setIpFieldValue(tmpPayload[9])
                modemGateway.setIpFieldValue(tmpPayload[10])
                modemDns1.setIpFieldValue(tmpPayload[11])
                modemDns2.setIpFieldValue(tmpPayload[12])
            }
            break
        }

        case EnumClass.GUI_MSG_ACMS_SERVER_CFG_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)

            serverModeId.selectedNo = parseInt(tmpPayload[0])
            if(parseInt(tmpPayload[0]) == 3)	//THIRD PARTY
            {
                 serverModeId.selectedNo = 2
            }
            else
            {
                serverModeId.selectedNo = parseInt(tmpPayload[0])
            }
            serverURL.value = tmpPayload[1]
            serverPortNumber.value = tmpPayload[2]
            serverTenantID.value = tmpPayload[3]

            break
        }

        case EnumClass.GUI_MSG_DATE_TIME_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            manualF= parseInt(tmpPayload[0],10)
            break
        }

        case EnumClass.GUI_MSG_WIFI_CFG_SCREEN:
        {
			var tempSsid
			var tempsigLevel
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            dataSsid = tmpPayload[0]
            currSsid = tmpPayload[0]
            dataSecurityType =  parseInt(tmpPayload[1],10)
            dataPassword = tmpPayload[2]
            dataConnectionType = parseInt(tmpPayload[3],10)
            dataIpAddress = tmpPayload[4]
            dataSubnet = tmpPayload[5]
            dataGateway = tmpPayload[6]
            dataDns1 = tmpPayload[7]
            dataDns2 = tmpPayload[8]
            dataWifiStatus = parseInt(tmpPayload[9],10)     //0 No wifi
                                                            //1 Wifi Not Connected
                                                            //2 Wifi Connected

            dataEncryptionType = parseInt(tmpPayload[10],10)
            dataAdvAuthType = parseInt(tmpPayload[11],10)
            dataAnonymousId = tmpPayload[12]
            dataUserName = tmpPayload[13]

            dataPeapVerType = parseInt(tmpPayload[14],10)
            dataInnerAuthType = parseInt(tmpPayload[15],10)

            currIsSecured = ((dataSecurityType > 0) ? true : false)
            currWifiEnableF = ( parseInt(tmpPayload[16],10) == 1) ? true : false
            if(dataWifiStatus == 2)
            {
                currConnectionStatus = dispStr.readDispStr(DispStr.MSG_CONNECTED_99)
                currFieldStrength = parseInt(tmpPayload[17],10)

                currWifiConnected = true
		currbandFreqType = parseInt(tmpPayload[18],10)
            }
            else
            {
                currConnectionStatus = dispStr.readDispStr(DispStr.MSG_NOT_CONNECTED_774)
                currFieldStrength = 0
                currWifiConnected = false
		currbandFreqType = parseInt(tmpPayload[17],10)
            }
			
            break
        }

        case EnumClass.GUI_MSG_SEARCH_WIFI_SCREEN:
        {
            var wifiIndex
            var tempWiFiData        = new Array()
            var tempSSID            = new Array()
            var tempNwSecType       = new Array()
            var tempSigQuality      = new Array()
            var tempConnStatF       = new Array()
            var tempSecType         = new Array()
            var tempWpsEnabledF     = new Array()
            var tempBssid           = new Array()
            var tempEncryption      = new Array()
            var tempWifiConn        = new Array()
	    var tempSigLevel		= new Array()

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            wifiInfo.noOfSSID = parseInt(tmpPayload[0],10)
            for(wifiIndex =0 ; wifiIndex < parseInt(tmpPayload[0],10) ; wifiIndex++)
            {
                tempWiFiData = tmpPayload[wifiIndex+1].split(String.fromCharCode (DESH_FSP).toString())

                
                tempSSID[wifiIndex]             = tempWiFiData[0]
                tempNwSecType[wifiIndex]        = parseInt(tempWiFiData[1],10)
                tempSigQuality[wifiIndex]       = parseInt(tempWiFiData[2],10)
                tempWifiConn[wifiIndex]		= parseInt(tempWiFiData[3],10)
                tempConnStatF[wifiIndex]        = (tempWifiConn[wifiIndex] ? dispStr.readDispStr(DispStr.MSG_CONNECTED_99) : "")

                tempSecType[wifiIndex]          = parseInt(tempWiFiData[4],10)
                tempEncryption[wifiIndex]       = parseInt(tempWiFiData[5],10)
                tempWpsEnabledF[wifiIndex]      = parseInt(tempWiFiData[6],10)
                tempBssid[wifiIndex]            = tempWiFiData[7]
                tempSigLevel[wifiIndex]			= parseInt(tempWiFiData[8],10)

            }

            wifiInfo.ssidName      = tempSSID
            wifiInfo.fieldStrength = tempSigQuality
            wifiInfo.isSecured     = tempNwSecType
            wifiInfo.connectionStatus   = tempConnStatF
            wifiInfo.wifiConnected   = tempWifiConn     

            wifiInfo.secType       = tempSecType
            wifiInfo.encryptionType= tempEncryption
            wifiInfo.wpsEnabledF   = tempWpsEnabledF
            wifiInfo.bssid         = tempBssid
            wifiInfo.bandFreqType  = tempSigLevel
            break
        }

        case EnumClass.GUI_MSG_ENROLL_GET_NAME_FROM_ID:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            if(tmpPayload)
            {
                userInfoId.userName = tmpPayload[0] ? tmpPayload[0] : dispStr.readDispStr(DispStr.MSG_USER_NAME_1013)
                userInfoId.userIdForPhoto = tmpPayload[1] ? tmpPayload[1] : ""
//                if(tmpPayload[0].length == 0)
//                {
//                    userInfoId.userId = ""
//                }
                userInfoId.loadUserPhoto()
            }
            break
        }

        case EnumClass.GUI_MSG_DEL_CRED_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            var EnrollIdx =parseInt(tmpPayload[0],10)
            if(EnrollIdx==7)
            {
                EnrollIdx=4;
            }
            delCredentialIndex = EnrollIdx //parseInt(tmpPayload[0],10)
            userInfoId.userId  = ""
            userInfoId.enrollIdType = parseInt(tmpPayload[1],10);
            if(userInfoId.enrollIdType === 1)
            {
                userInfoId.userIdWaterMark = dispStr.readDispStr(DispStr.MSG_REFERENCE_ID_939)
            }
            else
            {
                userInfoId.userIdWaterMark = dispStr.readDispStr(DispStr.MSG_USER_ID_626)
            }
            userInfoId.userName = dispStr.readDispStr(DispStr.MSG_USER_NAME_1013)
            break
        }
        case EnumClass.GUI_MSG_BACKUP_UPDATE_PROG:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            headerData = tmpPayload[0]
            progressData= tmpPayload[2]
            break
        }

        case EnumClass.GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN:
        case EnumClass.GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN:
        case EnumClass.GUI_MSG_CAFE_ITEM_LIST_SCREEN:
        case EnumClass.GUI_MSG_CAFE_LIST_SCREEN_UPDATE:
        {
            var tempitemName        = new Array()
            var tempitemCode        = new Array()
            var tempitemNumber      = new Array()
            var tempitemPrice       = new Array()
            var tempitemSoldOutFlag = new Array()
            var itemDetails         = new Array()

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            numberOfItems = parseInt(tmpPayload[0],10)
            defltItemEnbl = parseInt(tmpPayload[1], 10)

            cafeMenuPage = parseInt(tmpPayload[2], 10)
            currentPgIndex = parseInt(tmpPayload[3], 10)
            for (var idx=0; idx<numberOfItems; idx++)
            {
                itemDetails              = tmpPayload[idx+4].split(String.fromCharCode (DESH_FSP).toString())
                tempitemNumber[idx]      = parseInt(itemDetails[0],10)
                tempitemCode[idx]        = parseInt(itemDetails[1],10)
                tempitemName[idx]        = itemDetails[2]
                tempitemPrice[idx]       = itemDetails[3]
                tempitemSoldOutFlag[idx] = parseInt(itemDetails[4],10)
            }

            itemNumber      = tempitemNumber
            itemCode        = tempitemCode
            itemName        = tempitemName
            itemPrice       = tempitemPrice
            itemSoldOutFlag = tempitemSoldOutFlag
            break
        }

        case EnumClass.GUI_MSG_CAFE_ITEM_SELECTION:
        {
            var tempItemIdx         = new Array()
            var tempPrice
            var cnt
            var idx
            var tempItemQty         = new Array()
            var tempItemAmount      = new Array()

            tempItemQty = itemQty
            tempItemAmount = itemAmount

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            finalFracTotal = tmpPayload[0]

            for(cnt = 1, idx = 0; cnt < (tmpPayload.length - 1); cnt = (cnt +3),idx++ )
            {
                tempItemIdx[idx] = parseInt(tmpPayload[cnt],10)
                tempItemQty[tempItemIdx[idx]] = parseInt(tmpPayload[cnt+1],10)
                tempItemAmount[tempItemIdx[idx]] = tmpPayload[cnt+2]
            }

            itemQty         = tempItemQty
            itemAmount      = tempItemAmount

            for(cnt = 0; cnt < itemQty.length; cnt++)
            {
              	if(tempItemIdx[cnt] != undefined )
                {
                    updateMenuItem(tempItemIdx[cnt]);
                }
            }

            finalTotal = (finalFracTotal == "0.00")?0:1
            break
        }

        case EnumClass.GUI_MSG_MANAGE_CANTEEN_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            printLastBillF = parseInt(tmpPayload[0],10)
            itemSoldOutF = parseInt(tmpPayload[1],10)
            itemAvailableF = parseInt(tmpPayload[2],10)
            break
        }

        case EnumClass.GUI_MSG_CHANGE_PW_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            if(isAdminHrdF)
            {
                currentPassword = tmpPayload[0] //Admin Password
            }
            else
            {
                currentPassword = tmpPayload[1] //Hrd Password
            }
            break
        }

        case EnumClass.GUI_MSG_ZONE_CFG_SCREEN :
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            dndF = tmpPayload[0]
            dndActiveDeactive = tmpPayload[1]
            dmzF = tmpPayload[2]
            dmzActiveDeactive = tmpPayload[3]
            break
        }

        case EnumClass.GUI_MSG_ZONE_CONTROL_SCREEN :
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            zoneLockF = parseInt(tmpPayload[0],10)
            zoneUnlockF = parseInt(tmpPayload[1],10)
            zoneNormalF = parseInt(tmpPayload[2],10)
            zoneArmF = parseInt(tmpPayload[3],10)
            zoneDisarmF = parseInt(tmpPayload[4],10)
            break
        }

        case EnumClass.GUI_MSG_ALARM_OPTIONS_SCREEN :
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            panicAlarmF = tmpPayload[0]
            muteDoorBuzzerF = tmpPayload[1]
            clearDoorAuxOPF = tmpPayload[2]
            muteMasterBuzzerF = tmpPayload[3]
            clearMasterOPF = tmpPayload[4]
            clearDcAllAlarmF = tmpPayload[5]
            break
        }

        case EnumClass.GUI_MSG_VIEW_USER_PROFILE_SCREEN :
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            usrName.value = tmpPayload[0]
            designation.value = tmpPayload[1]
            branch.value = tmpPayload[2]
            department.value = tmpPayload[3]
            bloodGroup.value = tmpPayload[4]
            emergencyContact.value = tmpPayload[5]
            medicalHistory.value = tmpPayload[6]
            break
        }

        case EnumClass.GUI_MSG_WPS_PROGRESS_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            //console.log(tmpPayload)
            triggerType = parseInt(tmpPayload[0],10)
            pinData = tmpPayload[1]
            break
        }

        case EnumClass.GUI_MSG_JOB_CODE_DISPLAY_SCREEN:
        case EnumClass.GUI_MSG_JOB_CODE_LIST:
        {
            var tmpJobCodeList = new Array()

            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
//            console.log(tmpPayload)
            currStartPageNo = parseInt(tmpPayload[0],10)
            currSubPageCnt = parseInt(tmpPayload[1],10)
            prevStartPageNo = parseInt(tmpPayload[2],10)
            prevSubPageCnt = parseInt(tmpPayload[3],10)
            nextStartPageNo = parseInt(tmpPayload[4],10)
            nextSubPageCnt = parseInt(tmpPayload[5],10)
            noOfJobCodes = parseInt(tmpPayload[6],10)
            for(var jobx = 0; jobx < noOfJobCodes; jobx++)
            {
                tmpJobCodeList[jobx] = tmpPayload[jobx+7]
            }

            jobCodeList = tmpJobCodeList
            break
         }

        case EnumClass.GUI_MSG_MULTILANG_FLAG:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            enableMultiLang = parseInt(tmpPayload[0],10)
            textFontFamily = dispStr.loadFonts()
           break
        }

        case EnumClass.GUI_MSG_INOUT_SPCL_FUNCTION_SELECTION:
        {
            var tmpActiveInactiveSpclFuncnList = new Array()
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
			// console.log(tmpPayload)
  
            for(var spclFucn = 0; spclFucn < tmpPayload.length-1; spclFucn++)
            {
                tmpActiveInactiveSpclFuncnList[spclFucn] =  EnumClass.ICON_GOING_IN_OFFICIAL_SPLFUNC + parseInt(tmpPayload[spclFucn],10)
            }
            allInOutSpecialFunction = tmpActiveInactiveSpclFuncnList
            break
        }

        case EnumClass.GUI_MSG_ASSIGN_USER_ON_DEVICE:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            askAssignUsrInfo.userId   = tmpPayload[0]
            askAssignUsrInfo.userName = tmpPayload[1]
            break
        }

        case EnumClass.GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            askAddUsrInfoUserId   = tmpPayload[0]
            break
        }

        case EnumClass.GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            askcafeUsrInfo.userId   = tmpPayload[0]
            askcafeUsrInfo.userName = tmpPayload[1]
            askcafeUsrInfo.userIdActiveF = false
            askcafeUsrInfo.userNameActiveF = false
            askcafeUsrInfo.userNameTouchActiveF = false
            askcafeUsrInfo.userPhotoF = tmpPayload[2] ? tmpPayload[2] : ""
            askcafeUsrInfo.userIdForPhoto = tmpPayload[3]
            askcafeUsrInfo.loadUserPhoto()
            break
        }

        case EnumClass.GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN:
        {
            tmpPayload = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
            faceEnrollDetectingItem.maxFaces = parseInt(tmpPayload[0], 10)
            faceEnrollDetectingItem.noOfFacesProcessed = parseInt(tmpPayload[1], 10)
            faceEnrollDetectingItem.warningMsg = tmpPayload[2]
            if(faceEnrollDetectingItem.noOfFacesProcessed >= 1)
                footerConfigIdx = EnumClass.FOOTER_ABORT_STOP_ENABLE
            break;
        }

        case EnumClass.GUI_MSG_WIFI_SIGNAL_STRENGTH:
        {
			var tempSsid
			var tempsigLevel
            tmpPayload = payload.split(String.fromCharCode(UI_MSG_FSP).toString())
            statusBarId.wifiStrength = parseInt(tmpPayload[0])
            dataSsid = tmpPayload[1]
            currSsid = tmpPayload[1]
            dataSecurityType = parseInt(tmpPayload[2],10)
            currIsSecured = ((dataSecurityType > 0) ? true :false)
            currWifiEnableF = ( parseInt(tmpPayload[3],10) == 1) ?true :false
            currbandFreqType = parseInt(tmpPayload[4],10)

        currConnectionStatus = dispStr.readDispStr(DispStr.MSG_CONNECTED_99)
	    currFieldStrength = parseInt(tmpPayload[0])

            break;
        }

        case EnumClass.GUI_MSG_DEVICE_INACTIVE:
        {
           tmpPayload = payload.split(String.fromCharCode(UI_MSG_FSP).toString())

            deviceInactive = (parseInt(tmpPayload[0],10) == 1) ?true :false
            hideMenuForDeviceInactive = (parseInt(tmpPayload[1],10) == 1) ?true :false

            if(deviceInactive === true)
            {
                deviceInactiveStr = tmpPayload[2]
            }
            else
            {
                deviceInactiveStr = ""
            }
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
//parseInitData
//Param:
//  IN : reqType, devPayload
//  OUT:NONE
//  Returns: NONE
//  Description:
//////////////////////////////////////////////////////////////////////////////
function parseInitData(msgId, payload)
{
    var tmpInitData = new Array();

    if(msgId == EnumClass.GUI_MSG_STARTUP_INFO)
    {
        tmpInitData = payload.split(String.fromCharCode (UI_MSG_FSP).toString())
        //console.log(tmpInitData)
        timeFormat = parseInt(tmpInitData[0],10)
        wallpaperIndex = parseInt(tmpInitData[1],10)
        clockIndex = parseInt(tmpInitData[2],10)
        shift = parseInt(tmpInitData[3],10)
        shift = (shift/(60*60))
        keypadOption = parseInt(tmpInitData[4],10)
        doorApplicationType = parseInt(tmpInitData[5],10)
        doorFirmwareType = parseInt(tmpInitData[6],10)
        enableMultiLang = parseInt(tmpInitData[7],10)
        autoHideFooterBarF = parseInt(tmpInitData[8],10)
        internalFpSupport = parseInt(tmpInitData[9],10)

        return true
    }
    else
    {
        return false
    }
}
//////////////////////////////////////////////////////////////////////////////
//parseConfigData
//Param:
//  IN : reqType, devPayload
//  OUT:NONE
//  Returns: NONE
//  Description:
//////////////////////////////////////////////////////////////////////////////
function parseConfigData(msgId, payload)
{
    var tmpConfigData = new Array();
    var tmpEntryWallpaperF, tmpExitWallpaperF;

    tmpConfigData = payload.split(String.fromCharCode (UI_MSG_FSP).toString())

    timeFormat          = parseInt(tmpConfigData[0],10)
    shift               = parseInt(tmpConfigData[1],10)
    shift               = (shift/(60*60))
    doorModeVisibleF    = parseInt(tmpConfigData[2],10)
    doorMode            = parseInt(tmpConfigData[3],10)
    tmpEntryWallpaperF  = parseInt(tmpConfigData[4],10);
    tmpExitWallpaperF   = parseInt(tmpConfigData[5],10);
    autoHideFooterBarF  = parseInt(tmpConfigData[6],10);

    if( (autoHideFooterBarF == true) && ( mainLoader.source == "qrc:/qml/Appl/Idle.qml") && (popUpLoader.source == ""))
    {
        footerBarStart  = lcd_height
        footerBarEnd    = lcd_height
        footerBarId.y   = 0
    }
    else if((autoHideFooterBarF == false) && (deviceInactive == false || hideMenuForDeviceInactive == false) &&
	    (mainLoader.source == "qrc:/qml/Appl/Idle.qml") && (popUpLoader.source == ""))
    {
        footerBarStart  = lcd_height-footerBarId.height
        footerBarEnd    = lcd_height-footerBarId.height
        footerBarId.y   = 0
    }

    if(doorMode == 0)
        showWallPaperF = tmpEntryWallpaperF
    else if(doorMode == 1)
        showWallPaperF = tmpExitWallpaperF
    else
        showWallPaperF = false
}
