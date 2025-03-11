import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Item
{
    id: icon
    height : iconImg.height + iconText.height
    width: iconImg.width
    opacity : iconEnableF ? 1 : 0.50
    enabled : (iconIdx === 0) ? false : true

    property int iconIdx
    property bool iconEnableF : true

    property string localImgLoc : imageLocation + "/MenuIcon/"
    property int textWidth : 90
    property int textTopMargin : -5
    signal clicked(int idx)

    Image
    {
        id: iconImg
        source : (localImgLoc + iconImgSource[iconIdx])
        anchors.horizontalCenter: parent.horizontalCenter
        visible: (iconIdx === 0) ? false : true

        MxText
        {
            id: iconText
            width : textWidth
            anchors.top: parent.bottom
            anchors.topMargin: textTopMargin
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: fontSizeIconLabel
            //font.bold: true
            color: "white"
            smooth: true
            text : iconLabelSource[iconIdx]
            //wrapMode: Text.Wrap
            horizontalAlignment: TextInput.AlignHCenter
        }

        Rectangle
        {
            id : pressedRect
            width : iconImg.sourceSize.width - 6
            height: iconImg.sourceSize.height - 8
            anchors.horizontalCenter: parent.horizontalCenter
            color : " transparent"
            border.color: mxColorBlue
            border.width: 3
            smooth : true
            radius: 13
            visible : mouseArea.pressed ? true : false
        }
    }
    MouseArea
    {
        id: mouseArea
        anchors.fill: icon
        enabled : iconEnableF
        onClicked:
        {
            icon.clicked(iconIdx)
        }
    }

    property variant iconLabelSource :
    [
        "",                             //0
        dispStr.readDispStr(DispStr.MSG_ADMIN_31)              /*"Admin*/,
        dispStr.readDispStr(DispStr.MSG_HRD_268)                /*"HRD*/,
        dispStr.readDispStr(DispStr.MSG_ABOUT_DEVICE_20)        /*"About Device*/,
        dispStr.readDispStr(DispStr.MSG_CAFETERIA_24)                  /*"Cafeteria*/,
        dispStr.readDispStr(DispStr.MSG_SETTINGS_548)                /*"Settings*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_CONTROL_159)        /*"Door Control"*/,
        dispStr.readDispStr(DispStr.MSG_ENROLLMENT_196)            /*"Enrollment"*/,
        dispStr.readDispStr(DispStr.MSG_LATE_IN_START_316)           /*"Late In Start"*/,
        dispStr.readDispStr(DispStr.MSG_EARLY_OUT_START_182)         /*"Early Out Start"*/,
        dispStr.readDispStr(DispStr.MSG_LATE_IN_STOP_317)            /*"Late In Stop"*/,
        dispStr.readDispStr(DispStr.MSG_EARLY_OUT_STOP_183)          /*"Early Out Stop"*/,
        dispStr.readDispStr(DispStr.MSG_BACKUP_AND_UPDATE_60)   /*"Backup & Update"*/,
        dispStr.readDispStr(DispStr.MSG_CLEAR_ALARM_84)         /*"Clear Alarm"*/,
        dispStr.readDispStr(DispStr.MSG_DATE_AND_TIME_109)          /*"Date and Time"*/,
        dispStr.readDispStr(DispStr.MSG_DISPLAY_142)                /*"Display"*/,
        dispStr.readDispStr(DispStr.MSG_SERVER_525)                     /*"Server"*/,
        dispStr.readDispStr(DispStr.MSG_LAN_309)                        /*"LAN"*/,
        dispStr.readDispStr(DispStr.MSG_WI_FI_658)                /*status "Wi-Fi"*/,
        dispStr.readDispStr(DispStr.MSG_MODEM_356)                      /*"Modem"*/,
        dispStr.readDispStr(DispStr.MSG_PASSWORD_426)     /*Admin "Password"*/,
        dispStr.readDispStr(DispStr.MSG_SET_WALLPAPER_545)           /*"Set Wallpaper"*/,
        dispStr.readDispStr(DispStr.MSG_SELECT_CLOCK_510)               /*"Select Clock"              CAPITAL*/,
        dispStr.readDispStr(DispStr.MSG_SELECT_KEYPAD_519)              /*"Select Keypad"             CAPITAL*/,
        dispStr.readDispStr(DispStr.MSG_LOCK_DOOR_325)              /*"Lock Door"*/,
        dispStr.readDispStr(DispStr.MSG_UNLOCK_DOOR_618)            /*"Unlock Door"*/,
        dispStr.readDispStr(DispStr.MSG_NORMALIZE_DOOR_399)             /*"Normalize Door"*/,
        dispStr.readDispStr(DispStr.MSG_ENROLL_USER_195)               /*"Enroll User"*/,
        dispStr.readDispStr(DispStr.MSG_ENROLL_SPECIAL_CARDS_192)        /*"Enroll Special Cards"*/,
        dispStr.readDispStr(DispStr.MSG_DELETE_CREDENTIALS_129)         /*"Delete Credentials"*/,
        dispStr.readDispStr(DispStr.MSG_RESTORE_CONFIGURATION_497)      /*"Restore Configuration"*/,         //30
        dispStr.readDispStr(DispStr.MSG_WRITE_EVENTS_667)               /*"Write Events"*/,
        dispStr.readDispStr(DispStr.MSG_FIRMWARE_UPGRADE_245)   /*"Firmware Upgrade"*/,
        dispStr.readDispStr(DispStr.MSG_MANAGE_CAFETERIA_332)           /*"Manage Cafeteria"*/,
        dispStr.readDispStr(DispStr.MSG_VIEW_BALANCE_641)               /*"View Balance"*/,
        dispStr.readDispStr(DispStr.MSG_PRINT_LAST_BILL_466)            /*"Print Last Bill"*/,
        dispStr.readDispStr(DispStr.MSG_ITEM_SOLD_OUT_305)             /*"Item Sold Out"*/,
        dispStr.readDispStr(DispStr.MSG_ITEM_AVAILABLE_304)            /*"Item Available"*/,
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_WORK_IN_411)       /*"Official Work In"*/,
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_WORK_OUT_412)      /*"Official Work Out"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_IN_550)         /*"Short Leave In"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_OUT_551)        /*"Short Leave Out"*/,
        dispStr.readDispStr(DispStr.MSG_REGULAR_IN_492)             /*"Regular In"*/,
        dispStr.readDispStr(DispStr.MSG_REGULAR_OUT_493)                /*"Regular Out"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_END_739)               /*"Break End"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_START_392)             /*"Break Start"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_IN_416)             /*"Overtime In"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_OUT_417)            /*"Overtime Out"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_LOCK_166)               /*"Door Lock"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_UNLOCK_176)             /*"Door Unlock"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_NORMAL_169)             /*"Door Normal"*/,                 //50
        dispStr.readDispStr(DispStr.MSG_REGULAR_771)                    /*"Regular"*/,
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_407)                   /*"Official"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_END_739)               /*"Break End"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_418)                  /*"Over Time"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_549)                /*"Short Leave"*/,
        dispStr.readDispStr(DispStr.MSG_REGULAR_771)                    /*"Regular"*/,
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_407)                   /*"Official"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_START_392)             /*"Break Start"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_418)                  /*"Over Time"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_549)                /*"Short Leave"*/,           //60
        dispStr.readDispStr(DispStr.MSG_READ_ONLY_CARD_483)         /*"Read Only Card"*/,
        dispStr.readDispStr(DispStr.MSG_SMART_CARD_565)                 /*"Smart Card"*/,
        dispStr.readDispStr(DispStr.MSG_FINGER_ONLY_238)                /*"Finger Only"*/,
        dispStr.readDispStr(DispStr.MSG_FP_THEN_CARD_249)                /*"FP then Card"*/,
        dispStr.readDispStr(DispStr.MSG_USER_PROFILE_631)               /*"User Profile"*/,         //65
        dispStr.readDispStr(DispStr.MSG_ZONE_SETTINGS_682)           /*"Zone Settings"*/,
        dispStr.readDispStr(DispStr.MSG_ALARM_OPTIONS_39)          /*"Alarm Options"*/,
        dispStr.readDispStr(DispStr.MSG_ACTIVATE_DND_28)           /*"Activate DND"*/,
        dispStr.readDispStr(DispStr.MSG_DEACTIVATE_DND_120)         /*"Deactivate DND"*/,
        dispStr.readDispStr(DispStr.MSG_ACTIVATE_DEAD_MAN_ZONE_26)/*"Activate Dead Man Zone"*/,       //70
        dispStr.readDispStr(DispStr.MSG_DEACTIVATE_DEAD_MAN_ZONE_79)/*"Deactivate Dead Man Zone"*/,
        dispStr.readDispStr(DispStr.MSG_ZONE_CONTROL_677) /*"Zone Control"*/,
        dispStr.readDispStr(DispStr.MSG_LOCK_ZONE_326)/*"Lock Zone"*/,
        dispStr.readDispStr(DispStr.MSG_UNLOCK_ZONE_619)/*"Unlock Zone"*/ ,
        dispStr.readDispStr(DispStr.MSG_NORMALIZE_ZONE_401)/*"Normalize Zone"*/,               //75
        dispStr.readDispStr(DispStr.MSG_SET_PANIC_ALARM_541)/*"Set Panic Alarm"*/,
        dispStr.readDispStr(DispStr.MSG_MUTE_DOOR_BUZZER_367)/*"Mute Door Buzzer"*/,
        dispStr.readDispStr(DispStr.MSG_CLEAR_DOOR_AUX_OUTPUT_86)/*"Clear Door Aux Output"*/,
        dispStr.readDispStr(DispStr.MSG_MUTE_MASTER_BUZZER_368)/*"Mute Master Buzzer"*/,
        dispStr.readDispStr(DispStr.MSG_CLEAR_MASTER_OUTPUT_87)/*"Clear Master Output"*/,          //80
        dispStr.readDispStr(DispStr.MSG_ZONE_LOCK_680)/*"Zone Lock"*/,
        dispStr.readDispStr(DispStr.MSG_ZONE_UNLOCK_684)/*"Zone Unlock"*/,
        dispStr.readDispStr(DispStr.MSG_ZONE_NORMAL_681)/*"Zone Normal"*/,
        dispStr.readDispStr(DispStr.MSG_CHANGE_PASSWORD_1)        /*"Change Password"*/,
        dispStr.readDispStr(DispStr.MSG_DEFAULT_HRD_PASSWORD_125)       /*"Default HRD Pasword"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_MODE_CHANGE_168)      /*"Door Mode Change"*/,              //86
        dispStr.readDispStr(DispStr.MSG_ZONE_ARM_675)/*"Zone Arm"*/,
        dispStr.readDispStr(DispStr.MSG_ZONE_DISARM_678)/*"Zone Disarm"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_ARM_157)/*"Door Arm"*/,
        dispStr.readDispStr(DispStr.MSG_DOOR_DISARM_160)/*Door Disarm"*/,
        dispStr.readDispStr(DispStr.MSG_IN_811)/*IN*/,
        dispStr.readDispStr(DispStr.MSG_OUT_812)/*OUT*/,                                            //92
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_WORK_IN_411)       /*"Official Work In"*/,  //special finctions
        dispStr.readDispStr(DispStr.MSG_OFFICIAL_WORK_OUT_412)      /*"Official Work Out"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_IN_550)         /*"Short Leave In"*/,
        dispStr.readDispStr(DispStr.MSG_SHORT_LEAVE_OUT_551)        /*"Short Leave Out"*/,
        dispStr.readDispStr(DispStr.MSG_REGULAR_IN_492)             /*"Regular In"*/,
        dispStr.readDispStr(DispStr.MSG_REGULAR_OUT_493)                /*"Regular Out"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_END_739)               /*"Break End"*/,
        dispStr.readDispStr(DispStr.MSG_BREAK_START_392)             /*"Break Start"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_IN_416)             /*"Overtime In"*/,
        dispStr.readDispStr(DispStr.MSG_OVER_TIME_OUT_417)            /*"Overtime Out"*/,
        dispStr.readDispStr(DispStr.MSG_SET_BRIGHTNESS_963)            /*"Set Brightness"*/,        //103
        dispStr.readDispStr(DispStr.MSG_TEMPERATURE_986)             		 /*"Temperature"*/,           //104
        dispStr.readDispStr(DispStr.MSG_FACE_ONLY_1046)	/*Face */, 		//105
        dispStr.readDispStr(DispStr.MSG_DURESS_FINGER_1078)	/*Duress Fp */,  //106
      ]

    property variant iconImgSource :
    [
        "Admin.png",                     //0
        "Admin.png",
        "HRD.png",
        "AboutDevice.png",
        "E-Canteen.png",
        "Settings.png",
        "DoorControl.png",
        "Enrollment.png",
        "LateInStart.png",
        "EarlyOutStart.png",
        "LateInStop.png",       //10
        "EarlyOutStop.png",
        "BackupUpdate.png",
        "ClearAlarm.png",
        "DateTime.png",
        "Display.png",
        "Server.png",
        "LAN.png",
        "Wi-Fi.png",
        "Modem.png",
        "Password.png",         //20
        "WallPaper.png",
        "Clock.png",
        "Keypad.png",
        "LockDoor.png",
        "UnlockDoor.png",
        "NormalizeDoor.png",
        "EnrollUsers.png",
        "EnrollSpecialCards.png",
        "DeleteCredentials.png",
        "RestoreConfigurations.png",    //30
        "WriteEvents.png",
        "FirmwareUpgrade.png",
        "ManageE-Canteen.png",
        "ViewBalance.png",
        "PrintLastBill.png",
        "ItemSoldOut.png",
        "ItemAvailable.png",
        "OfficialIn.png",
        "OfficialOut.png",
        "ShortLeaveIn.png",             //40
        "ShortLeaveOut.png",
        "RegularIn.png",
        "RegularOut.png",
        "BreakIn.png",
        "BreakOut.png",
        "OverTimeIn.png",
        "OverTimeOut.png",
        "LockDoor.png",
        "UnlockDoor.png",
        "NormalizeDoor.png",              //50
        "RegularIn.png",
        "OfficialIn.png",
        "BreakIn.png",
        "OverTimeIn.png",
        "ShortLeaveIn.png",
        "RegularOut.png",
        "OfficialOut.png",
        "BreakOut.png",
        "OverTimeOut.png",
        "ShortLeaveOut.png",                //60
        "EnrollReadonlyCard.png",
        "EnrollSmartCard.png",
        "EnrollFingureOnly.png",
        "EnrollFPthenCard.png",
        "UserProfile.png",                  //65
        "ZoneSetting.png",
        "AlarmOptions.png",
        "ActDND.png",
        "DeActDND.png",
        "ActDMZ.png",                       //70
        "DeActDMZ.png",
        "ZoneControl.png",
        "ZoneLock.png",
        "ZoneUnlock.png",
        "ZoneNormal.png",                   //75
        "PanicAlarm.png",
        "MuteDoorBuzzer.png",
        "ClearDoorAuxOP.png",
        "MuteMasterBuzzer.png",
        "ClearMasterOP.png",             //80
        "ZoneControl.png",
        "ZoneLock.png",
        "ZoneUnlock.png",
        "ChangePassword.png",
        "DefaultHrdPassword.png",		//85
        "DoorModeChange.png",               //86
        "ZoneArm.png",
        "ZoneDisarm.png",
        "DoorArm.png",
        "DoorDisarm.png",
        "DoorModeIn.png",
        "DoorModeOut.png",                  //92
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
        "SetBrightness.png",              //103
        "TemperatureDetection.png",       //104
        "FaceOnly.png",
        "EnrollFingureOnly.png", //106
    ]
}
