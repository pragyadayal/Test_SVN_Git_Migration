import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../Appl"

Image
{
    signal footerBarVisibleMode()
    property int footerBarHeight: footerImg.height
    property bool   footerVisible       : footerConfig[footerTypeIdx][0]
    property bool   idleFooterVisible   : footerConfig[footerTypeIdx][1]
    property bool   numKeyPadVisible    : footerConfig[footerTypeIdx][2]
    property bool   backVisible         : footerConfig[footerTypeIdx][3]
    property bool   homeVisible         : footerConfig[footerTypeIdx][4]
    property bool   homeRightSideVisible: footerConfig[footerTypeIdx][5]
    property bool   faceVisible         : footerConfig[footerTypeIdx][6]
    property bool   pinCenterVisible    : footerConfig[footerTypeIdx][7]
    property bool   abortVisible        : footerConfig[footerTypeIdx][8]
    property bool stopdisablerightbutton 	: footerConfig[footerTypeIdx][9]
    property bool stoprightbutton       : footerConfig[footerTypeIdx][10]
    
    
    property bool   idleFooterVisibleNormalMode  : ((idleFooterVisible) && (doorApplicationType === 2) && (frServerEnableF === true)) ? false :
                                                    ((idleFooterVisible) && (currentDoorStatus === EnumClass.DOOR_ONLINE) && !deviceInactive) ? ( (idleFooterVisibleOfflineMode = false),(idleFooterVisibleDegradeBasicMode = false),(idleFooterVisibleDegradeAdvanceMode = false),true )  : false

    property bool   idleFooterVisibleOfflineMode	 : ( (idleFooterVisible) && (doorApplicationType === 2) && (frServerEnableF === true) ) ? true : ( ((idleFooterVisible) && (currentDoorStatus === EnumClass.DOOR_NWFAULT)) ? true :false)
    property bool   idleFooterVisibleDegradeBasicMode	 : ( (idleFooterVisible) && (((degradeDoorModeType === 0) && (currentDoorStatus === EnumClass.DOOR_DEGRADEMODE)) || (deviceInactive)) ) ? true : false
    property bool   idleFooterVisibleDegradeAdvanceMode	 : ( (idleFooterVisible) && (degradeDoorModeType === 1) && (currentDoorStatus === EnumClass.DOOR_DEGRADEMODE) ) ? true : false

    onIdleFooterVisibleChanged:
    {
        footerBarVisibleMode()
    }

    onIdleFooterVisibleNormalModeChanged:
    {

        if(idleFooterVisible && deviceInactive && idleFooterVisibleNormalMode === false)
            idleFooterVisibleDegradeBasicMode=true
    }
                                                    
    id: footerImg
    source: imageLocation + "/Footer/Footerbar.png"
    visible: footerVisible && (enrollPopUpActive || !(deviceInactive && hideMenuForDeviceInactive))
    y:lcd_height-footerImg.height

    property int footerTypeIdx : footerConfigIdx
    property variant footerConfig :
    [
        [
            false, //footerBarVisibleF                      //footerConfigIdx -- > 0
            false, //idleFooterBarVisibleF                  //No Footer
            false, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 1
            true, //idleFooterBarVisibleF                  //Idle Footer
            false, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 2
            false, //idleFooterBarVisibleF                  //Only Home
            false, //pinButtonF
            false, //backButtonF
            true, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 3
            false, //idleFooterBarVisibleF                 //Only Pin
            true, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 4
            false, //idleFooterBarVisibleF                  //Back & Home
            false, //pinButtonF
            true, //backButtonF
            false, //homeButtonF
            true, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 5
            false, //idleFooterBarVisibleF                  //Back, PIN & Home
            true, //pinButtonF
            true, //backButtonF
            false, //homeButtonF
            true, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 6
            false, //idleFooterBarVisibleF                  //Back, PIN, Home & Face
            true, //pinButtonF
            true, //backButtonF
            false, //homeButtonF
            true, //homeRightButtonF
            true, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],

        [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 7
            false, //idleFooterBarVisibleF                  //PIN & Face
            true, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            true, //FaceButtonF
            false, //pinCenterVisible
            false, //abortVisible
			false, //stopdisablerightbutton
			false, //stoprightbutton
        ],
		
		[
            true, //footerBarVisibleF                      //footerConfigIdx -- > 8
            false, //idleFooterBarVisibleF                  //PIN Center
            false, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            true, //pinCenterVisible
            false, //abortVisible
            false, //stopdisablerightbutton
			false, //stoprightbutton
        ],
		
		 [
            true, //footerBarVisibleF                      //footerConfigIdx -- > 9
            false, //idleFooterBarVisibleF                  //Abort & Stop_Disable
            false, //pinButtonF
            false, //backButtonF
            false, //homeButtonF
            false, //homeRightButtonF
            false, //FaceButtonF
            false, //pinCenterVisible
            true, //abortVisible
            true, //stopdisablerightbutton
            false, //stoprightbutton
        ],

        [
           true, //footerBarVisibleF                      //footerConfigIdx -- > 10
           false, //idleFooterBarVisibleF                  //Abort & Stop_Enable
           false, //pinButtonF
           false, //backButtonF
           false, //homeButtonF
           false, //homeRightButtonF
           false, //FaceButtonF
           false, //pinCenterVisible
           true, //abortVisible
           false, //stopdisablerightbutton
           true, //stoprightbutton
       ],
    ]

    FooterIcon
    {
        id               : numericKeyPad
        footerSource     : imageLocation + "/Footer/Keypad.png"
        visible          : numKeyPadVisible
        anchors.left     : footerImg.left
        anchors.leftMargin : (frServerEnableF && faceVisible) ? 80:130
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_PIN_ENTER_SCREEN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : faceFooterImage
        footerSource     : imageLocation + "/Footer/FaceScanIcon.png"
        visible          : faceVisible
        anchors.left     : parent.left
        anchors.leftMargin: 170

        onFooterPress:
        {
            appData.data = EnumClass.GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : homeImage
        footerSource     : imageLocation + "/Footer/Home.png"
        visible          : homeVisible
        anchors.centerIn : parent
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_HOME +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : backImage
        footerSource     : imageLocation + "/Footer/Back.png"
        visible          : backVisible
        anchors.left     : parent.left
        onFooterPress    :
        {
            sendBackButtonPressed()
        }
    }

    FooterIcon
    {
        id               : homeImageRight
        footerSource     : imageLocation + "/Footer/Home.png"
        visible          : homeRightSideVisible
        anchors.right    : parent.right
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_HOME +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id            : goingInImage
        footerSource  : imageLocation + "/Footer/GoingIn.png"
        visible       : idleFooterVisibleNormalMode
        anchors.left  : parent.left
        opacity       : (goingInEnableF == true) ? 1 : 0.5
        btnEnable     : goingInEnableF
        onFooterPress :
        {
            appData.data = EnumClass.GUI_BUTTON_GOING_IN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : numericKeyPad1
        footerSource     : imageLocation + "/Footer/Keypad.png"
        visible          : idleFooterVisibleNormalMode
        anchors.left     : goingInImage.right
        anchors.leftMargin: 25
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_PIN_ENTER_SCREEN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : menuImage
        footerSource     : imageLocation + "/Footer/Menu.png"
        visible          : idleFooterVisibleNormalMode
        anchors.right    : goingOutImage.left
        anchors.rightMargin: 25
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id            : goingOutImage
        footerSource  : imageLocation + "/Footer/GoingOut.png"
        visible       : idleFooterVisibleNormalMode
        anchors.right : parent.right
        opacity       : (goingOutEnableF == true) ? 1 : 0.5
        btnEnable     : goingOutEnableF
        onFooterPress :
        {
            appData.data = EnumClass.GUI_BUTTON_GOING_OUT +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : homeImageCenter
        footerSource     : imageLocation + "/Footer/Menu.png"
        visible          : idleFooterVisibleOfflineMode
        anchors.centerIn : parent
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }
    
    
    FooterIcon
    {
        id               : homeImageCenter1
        footerSource     : imageLocation + "/Footer/Menu.png"
        visible          : idleFooterVisibleDegradeBasicMode
        anchors.centerIn : parent
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : numericKeyPad2
        footerSource     : imageLocation + "/Footer/Keypad.png"
        visible          : idleFooterVisibleDegradeAdvanceMode
        anchors.left     : goingInImage.right
        anchors.leftMargin: 25
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_PIN_ENTER_SCREEN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : menuImage1
        footerSource     : imageLocation + "/Footer/Menu.png"
        visible          : idleFooterVisibleDegradeAdvanceMode
        anchors.right    : goingOutImage.left
        anchors.rightMargin: 25
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    
    FooterIcon
    {
        id               : abortLeft
        footerSource     : imageLocation + "/Footer/Back.png"
        visible          : abortVisible
        anchors.left     : parent.left

        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_FACE_ENRL_TERMINATE +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }
    
    FooterIcon
    {
        id               : stopdisble
        footerSource     : imageLocation + "/Footer/STOP_DISABLED.png"
        visible          : stopdisablerightbutton
        anchors.right    : parent.right
        btnEnable        : false
    }
    
    
    FooterIcon
    {
        id               : stopnormal
        footerSource     : imageLocation + "/Footer/STOP_NORMAL.png"
        visible          : stoprightbutton
        anchors.right    : parent.right
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_FACE_ENRL_STOP +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : pinImageCenter
        footerSource     : imageLocation + "/Footer/Keypad.png"
        visible          : pinCenterVisible
        anchors.centerIn : parent
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_PIN_ENTER_SCREEN +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    onFooterBarVisibleMode:
    {
        if( (idleFooterVisible) && (((degradeDoorModeType === 0) && (currentDoorStatus === EnumClass.DOOR_DEGRADEMODE))
                                    || (deviceInactive)) )
        {
            idleFooterVisibleDegradeBasicMode = true
        }
        else
        {
            idleFooterVisibleDegradeBasicMode = false
        }

        if( (idleFooterVisible) && (degradeDoorModeType === 1) && (currentDoorStatus === EnumClass.DOOR_DEGRADEMODE) )
        {
            idleFooterVisibleDegradeAdvanceMode = true
        }
        else
        {
            idleFooterVisibleDegradeAdvanceMode = false
        }

        if((idleFooterVisible) && (doorApplicationType === 2) && (frServerEnableF === true))
        {
            idleFooterVisibleOfflineMode = true
        }
        else
        {
            if((idleFooterVisible) && (currentDoorStatus === EnumClass.DOOR_NWFAULT))
            {
                idleFooterVisibleOfflineMode = true
            }
            else
            {
                idleFooterVisibleOfflineMode = false
            }
        }
    }
}
