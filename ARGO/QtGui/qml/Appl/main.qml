import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import QmlLogger 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id:mainWidget

    property int lcd_width : 320
    property int lcd_height: 480
    property int deviceType : EnumClass.DEVICE_TYPE_ARGO

	// Font family related
    property string textFontFamily   		: "Segoe UI"
    property string clockFontFamily  		: "Book Antiqua"

    // Face Recognition Related
    property bool   frServerEnableF         : false // false=RegularIdleScreen, true=FrScreen(TapForIcon/FRStream)
    property bool   frStreamDisplayF        : false // false=TapForIcon, true=FRStream
	property bool   doNotApplyAutoHideF     : false
	
    //Font Size Related Defines
    property int fontSizeFooterIcon         : 10   // modem info "selected"
    property int fontSizeAboutDevice        : 14   // text read only
    property int fontSizeTextLabel          : 15   //Settings label
    property int fontSizeTextInputs         : 16   // selectionbox
    property int fontSizeButton             : 14   // Activate
    property int fontSizeDynamicResponse    : 12
    property int fontSizePopUpLabel         : 16   // Popup msgs and titles, AllowScreen
    property int fontSizePageHeading        : 17
    property int fontSizeIconLabel          : 15
    property int fontSizeUserInfo           : 15
    property int fontSizeTextBoxHeading     : 16
    property int fontSizeProfileInfo        : 17
    property int fontSizeProfileStatus      : 12
    property int fontSizeNormal             : 12   // selection box heading data
    property int fontSizeSpinBoxIndex       : 20
    property int fontSizeCafeListTitle      : 19
    property int fontSizeCafeListText       : 16
    property int fontSizeCafeButton         : 18
    property int fontSizePinEntry           : 14
    property int fontSizePinEntryKey        : 20

    //Spacing Related
    property int spacingBtwSettingLabel     : 5
    property int spacingBtwSettingElement   : 5
    property int topMarginCafe              : 35
    property int topMarginPswPin            : 50
    property int topMarginDisplaySetting    : 70
    property int topMarginSettingScreen     : 65
    property int topMarginAllowDeny         : 40
    property int msgBoxWidth                : 200
    property int msgBoxTextWidth            : 190
    property int msgBoxHeight               : 160

    // FSP related Defines
    property int    uiMsgFsp : 0x05
    property int    eotFsp  : 0x02
    property int    deshFsp : 0x03
    property string timeFsp : ':'
    property string dateFsp : '-'

    //Color Related
    property color mxColorBlue          : "#0a70a2"     // Font
    property color mxColorOffWhite      : "#a19e9e"     // Textbox border
    property color mxColorOrange        : "#653104"     // Pop Up Rectangle border
    property color mxColorGrey          : "#9a9a9a"     // Text and rectangle border
    property color mxColorBrightBlue    : "#0b406a"     // Heading Border
    property color mxColorGreen         : "#60a008"     // AllowScreen
    property color mxColorDullGrey      : "#bbbaba"     //Sold out
    property color mxColorPopupBorder   : "#404040"     // Pop Up Rectangle border
    property color mxColorRed           : "#fc6041"

    //T & A related
    property bool   lateInEnable      :false
    property bool   earlyOutEnable    :false
    property bool   lateInStartStop   :false
    property bool   earlyOutStartStop :false

    property bool  goingInEnableF     :true
    property bool  goingOutEnableF    :true

    // Date Time related variables
    property int    hours             :  00
    property int    minutes           :  00
    property int    seconds           :  00
    property int    currentDate       :  00
    property int    currentMonth      :  00
    property int    currentYear       :  0000
    property int    currentDay        :  00
    property real   shift             :  00
    property int    timeFormat


    property int   currentMonthMultiL :  00
    property int   currentDateMultiL  :  00
    property int   currentYearMultiL  :  0000
    property int   currentDayMultiL   :  00

    // Flickable related variables
    property int flickablePressDelay        : 400  // in msec
    property int flickableCacheBuffer       : 1000
    property int flickableFlickDeceleration : 1000

    // Others
    property bool   startApplF          : false
    property int    clockIndex          : 0
    property bool   isAdminHrdF         : true          // Admin Menu --> true
                                                        // HRD Menu -- > false

    property bool   abtDeviceScreenF    : false          // Admin Menu --> true

    property bool   isEnrollUserF       : true          // Enrollment --> true
                                                        // Delete Credential --> false

    property int    doorApplicationType                 // Access Control --> 0
                                                        // T & A         --> 1
                                                        // Cafeteria   --> 2
    property int  internalFpSupport         : EnumClass.INTERNAL_FP_SUPPORTED

    property string imageLocation           : "file:///matrix/appl/image"
    property string imgPathFileSys          : "/matrix/customImage"
    property string imgPathFileSysDirect    : "file:///matrix/customImage"

    property string rcvData
    property string userImagePath : ""
    property string showPin       : ""
    property string defaultUserPhotoPath: imageLocation + "/Others/DefaultUser.png"
    property string defaultUserLargePhotoPath: imageLocation + "/Others/DefaultUserLarge.png"
    property string defaultUserLargeCommonPhotoPath: imageLocation + "/Others/DefaultUserLargeCommonPhoto.png"

    property bool    doorFirmwareType:false                 // Direct Door --> 0
                                                            // Panel Door --> 1
    //PanelDoor Related
    property int currentDoorStatus
    property int degradeDoorModeType
    //Walpaper Related
    property int    wallpaperIndex : 0
    property variant allWallpaper:
    [
        imgPathFileSys + "/Wallpapers/1",
        imgPathFileSys + "/Wallpapers/2",
        imgPathFileSys + "/Wallpapers/3",
        imgPathFileSys + "/Wallpapers/4",
        imgPathFileSys + "/Wallpapers/5",
        imgPathFileSys + "/Wallpapers/6",
        imgPathFileSys + "/Wallpapers/7",
        imgPathFileSys + "/Wallpapers/8",
        imgPathFileSys + "/Wallpapers/9",
        imgPathFileSys + "/Wallpapers/10",
        imgPathFileSys + "/Wallpapers/11",
        imgPathFileSys + "/Wallpapers/12",
    ]

    property variant allInOutSpecialFunction:[ ]

    property variant doorModeDfltWallpaper:
    [
        imgPathFileSys + "/Wallpapers/DoorModeWallpapers/IN_Wallpaper",
        imgPathFileSys + "/Wallpapers/DoorModeWallpapers/OUT_Wallpaper",
    ]
    property variant doorModeCustomWallpaper:
    [
        imgPathFileSys + "/Wallpapers//DoorModeWallpapers/Entry",
        imgPathFileSys + "/Wallpapers/DoorModeWallpapers/Exit",
    ]

    //Door mode related
    property bool   doorModeVisibleF    : false
    property int    doorMode            : 0
    property bool   showWallPaperF      : false
    property bool   samayModeF          : false

    //Multilanguage related
    property bool   enableMultiLang     : false

    // Status Bar related
    property int   statusBarHeight     : statusBarId.statusBarItemHeight

    // SlideScreen related
    property bool  statusBarExpandedF  : false
    property int currentFeatureAccessed: -1

    //Device InActive Related
    property  bool deviceInactive: false
    property  bool hideMenuForDeviceInactive: false
    property string deviceInactiveStr : ""
    property bool enrollPopUpActive : false

    width : lcd_width
    height: lcd_height

    Image
    {
        id:startUpImageId
        source: "file:///matrix/startup_logo.png"
    }

    Image
    {
        id: wallpaperId
        anchors.fill : parent
        source: (doorModeVisibleF && showWallPaperF)
                 ? (appData.checkImage( doorModeCustomWallpaper[doorMode])
                    ? appData.createImagePath(doorModeCustomWallpaper[doorMode])
                    : (appData.checkImage( doorModeDfltWallpaper[doorMode])
                    ? appData.createImagePath(doorModeDfltWallpaper[doorMode])
                    : (appData.checkImage( allWallpaper[wallpaperIndex])
                     ? appData.createImagePath(allWallpaper[wallpaperIndex])
                     : imageLocation + "/Others/DefaultWallpaper.jpg") ) )
                 : (appData.checkImage( allWallpaper[wallpaperIndex])
                    ? appData.createImagePath(allWallpaper[wallpaperIndex])
                    : imageLocation + "/Others/DefaultWallpaper.jpg")
        cache   : false
        visible : false
        z: -1
    }

    //Footer Related
    property int footerConfigIdx        : EnumClass.FOOTER_NONE
    property bool footerBarHideF        : false
    property int footerBarStart         : lcd_height
    property int footerBarEnd           : lcd_height-footerBarId.height
    property bool autoHideFooterBarF    : false
    property int footerBarHeight        : footerBarId.footerBarHeight

    FooterBar
    {
        id : footerBarId
        z : (footerBarHideF ? 1 : 2 )

        Behavior on y
        {
            NumberAnimation{ duration:250; from:footerBarStart; to:footerBarEnd}
        }
    }

    onDoorApplicationTypeChanged:
    {
        footerBarId.footerBarVisibleMode()
    }
    onFrServerEnableFChanged:
    {
        footerBarId.footerBarVisibleMode()
    }
    onCurrentDoorStatusChanged:
    {
        footerBarId.footerBarVisibleMode()
    }
    onDegradeDoorModeTypeChanged:
    {
        footerBarId.footerBarVisibleMode()
    }

     StatusBar
     {
         id:statusBarId
         z: 2
         visible : false
     }

    function setFontfamily()
    {
       textFontFamily = dispStr.loadFonts()
    }

    function setVisibleArea(idx)
    {
        var elementWidth = 24
        var spacing = spacingBtwSettingElement
        var yPosition

        if(idx < 3)
        {
            yPosition = 0
        }
        else
        {
            yPosition = ((idx - 2)* (elementWidth + spacingBtwSettingElement))
        }
        return yPosition
    }

    function setFooterBar(pinF, escF)
    {
        if(escF && pinF)
        {
            footerConfigIdx = EnumClass.FOOTER_BACK_PIN_HOME
        }
        else if(escF)
        {
            footerConfigIdx = EnumClass.FOOTER_BACK_HOME
        }
        else if(pinF)
        {
            footerConfigIdx = EnumClass.FOOTER_PIN
        }
        else
        {
            footerConfigIdx = EnumClass.FOOTER_NONE
        }

        // If FR Enable
        if(frServerEnableF && escF && pinF)
        {
            footerConfigIdx = EnumClass.FOOTER_BACK_PIN_HOME_FACE
        }
        else if(frServerEnableF && pinF)
        {
            footerConfigIdx = EnumClass.FOOTER_PIN_FACE
        }
    }

    function setFooterConfigIdx(footerIdx)
    {
        footerConfigIdx = footerIdx
    }

    // Keypad Related
    property bool keypadOption : true      // Alphanum Keypad --> false
                                           // Dialpad --> true
    property int currentKeypad : EnumClass.KEYPAD_NONE     // None -- > 0
                                                            // Alphanum Keypad --> 1
                                                            // Numeric Keypad --> 2
                                                            // Dialpad --> 3

    property int keypadLoadDuration         : 3    // Resolution 100 msec

    Item
    {
        id :alphaNumKeypadId
        z : 3

        height : alphaNumericKeypadId.height
        width  : alphaNumericKeypadId.width

        //Vertical Animation
        Behavior on y
        {
            NumberAnimation{ duration: keypadLoadDuration * 100; from : lcd_height}
        }

        // These signals are used to give connection to TextEditBox and
        // individual qml pages
        signal vkpressed(string key)
        signal hidePress()

        AlphaNumKeypad
        {
            id: alphaNumericKeypadId
            anchors.bottom : parent.bottom
            visible: ( (currentKeypad == EnumClass.KEYPAD_ALPHANUMERIC) ? true : false )
            capsFlag : false
            symbolFlag : false
            onVkpressed:
            {
                alphaNumKeypadId.vkpressed(key)
            }
            onHidePress:
            {
                unloadKeypad()
                alphaNumKeypadId.hidePress()
            }
            onVisibleChanged:
            {
                if(visible == false)
                {
                    capsFlag = false
                    symbolFlag = false
                }
            }
        }

        Dialpad
        {
            id: dialpadId
            anchors.bottom : parent.bottom
            visible : ( (currentKeypad == EnumClass.KEYPAD_DIALPAD) ? true : false )
            stateIdx : 0
            onVkpressed:
            {
                alphaNumKeypadId.vkpressed(key)
            }
            onHidePress:
            {
                unloadKeypad()
                alphaNumKeypadId.hidePress()
            }
            onVisibleChanged:
            {
                if(visible == false)
                {
                    stateIdx = 0
                }
            }
        }
    }

    Keypad
    {
       id: numKeyadId
       z : 3

       visible: ( (currentKeypad == EnumClass.KEYPAD_NUMERIC) ? true : false )

       //Vertical Animation
       Behavior on y
       {
           NumberAnimation{ duration: keypadLoadDuration * 100; from : lcd_height}
       }
    }

    onCurrentKeypadChanged:
    {
        loadKeypad(currentKeypad)
    }

    // For animation
    function loadKeypad(keypadId)
    {
        switch(keypadId)
        {
            case EnumClass.KEYPAD_NUMERIC:
                numKeyadId.y = (lcd_height - numKeyadId.height)
                break

            case EnumClass.KEYPAD_DIALPAD:
            case EnumClass.KEYPAD_ALPHANUMERIC:
                alphaNumKeypadId.y = (lcd_height - alphaNumKeypadId.height)
                break
        }
    }

    property bool textBoxF              : true      //To enable textbox
    property bool ipfieldCompleteF      : false     //To indicate complition of IP Field
    property bool alphaNumKeypadF       : false     //Shows availability of Alphanumeric keypad
    property bool numKeypadF            : false     //Shows availability of Numeric keypad
    property bool backVisibleF          : true      //Decides availability of back icon on keypad
    property bool homeVisibleF          : true      //Decides availability of home icon on keypad
    property bool hoverEnableF          : true      //hover status
    property bool sendBackMsgFromPopUp  : false      //Msg of back button pressed is supposed to be sent
    property bool interDigitTimerF      : false     //Inter Digit timer Running status of dialpad

    //Loads Alphanumeric Keypad or Dialpad  based on Configuration
    function loadAlphaNumKeypad()
    {
        statusBarId.unloadSlideScreen()
        statusBarId.startStopSlideScreen(false)
        currentKeypad = (keypadOption ? EnumClass.KEYPAD_DIALPAD : EnumClass.KEYPAD_ALPHANUMERIC )
        alphaNumKeypadF = true
    }

    //Loads Numeric Keypad for PIN Entry
    function loadNumericKeypad()
    {
        statusBarId.unloadSlideScreen()
        statusBarId.startStopSlideScreen(false)
        currentKeypad = EnumClass.KEYPAD_NUMERIC
        numKeypadF = true
    }

    //Unloads any Keypad
    function unloadKeypad()
    {
        if( mainLoader.source != "qrc:/qml/Appl/Upgrade.qml" )
        {
        /* Enable The Slide Screen, When There is No System Update is Going on */
            statusBarId.startStopSlideScreen(true)
        }
        currentKeypad = EnumClass.KEYPAD_NONE
        numKeypadF = false
        alphaNumKeypadF = false
    }

    //Unloads Pop Up and Keypad
    function unloadAboveLayer()
    {
        popUpLoader.unloadPopUp()
        unloadKeypad()
    }

    //
    function resetKeypadPara()
    {
        backVisibleF = true
        homeVisibleF = true
        hoverEnableF = true
    }

    //Main Screen Loader on z = 0
    Loader
    {
        id : mainLoader
        source : ""

        onSourceChanged:
        {
          	//console.log("mainLoader Source Changed:....")
            unloadAboveLayer()
            resetKeypadPara()
        }
    }

    //Pop Up Loader on z = 1
    Loader
    {
        id : popUpLoader
        source : ""
        z : 1

        onSourceChanged:
        {
            unloadKeypad()
        }

        function loadDynamicScreen()
        {
            source = "DynamicScreen.qml"
            sendBackMsgFromPopUp = true
        }
        
        function loadTempUserPopUpScreen()
        {
            source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/TempUserPopUpScreen.qml"
            sendBackMsgFromPopUp = true
        }

        //Selection box related with Icon
        function loadSelectionBoxIconPopUp()
        {
            source = "../Common/SelectionBoxIcon.qml"
            footerBarHideF = true
        }

        //Selection box related with SpinBox
        function loadselectionBoxSpinBoxPopUp()
        {
            source = "../Common/SelectionBoxSpinBox.qml"
            footerBarHideF = true
        }

        //Selection box related with Text List
        function loadSelectionBoxTextListPopUp()
        {
            source = "../Common/SelectionBoxTextList.qml"
            footerBarHideF = true
        }

        //Loads DateTime setting Pop Up
        function loadDateTimeSettingScreen()
        {
            source = "DateTimeSettings.qml"
            footerBarHideF = true
        }

        function loadUserSearchScreen()
        {
            source = "UserSearch.qml"
            popUpLoader.item.initUserSearch()
        }

        //Loads enrollsplfunction Pop Up
        function loadEnrollSplFunctionScreen()
        {
            source = "../Common/PopupScreen.qml"
            footerBarHideF = true
        }

        function loadSysTestPopUp()
        {
            source = "../Common/SystemTest.qml"
            footerBarHideF = true
        }

        function loadDynamicTemperatureScreen()
        {
            source = "DynamicTemperatureScreen.qml"
            sendBackMsgFromPopUp = true
        }

        function unloadPopUp()
        {
            source = ""
            footerBarHideF = false
            sendBackMsgFromPopUp = false
        }

        function loadConfirmDefault(changeConfig, sMode)
        {
           //console.log("changeConfig ---------->" + changeConfig + "   sMode ---------->" + sMode)

            source = "ConfirmDefault.qml"
            popUpLoader.item.changeDoorModeF = changeConfig
            popUpLoader.item.serverMode = sMode
            if(changeConfig === false)
            {
                footerBarHideF = false
            }
        }

        function loadRestartMsg()
        {
            source = "RestartMsg.qml"
            footerBarHideF = true
        }
    }

    function sendOnlyKeepAlive()
    {
        appData.data =  EnumClass.GUI_BUTTON_KEEP_ALIVE +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
    }

    function sendAlnumKeepAlive()
    {
        appData.data = EnumClass.GUI_ALNUM_KEEP_ALIVE +
                       String.fromCharCode(uiMsgFsp).toString() +
                       String.fromCharCode(eotFsp).toString()
    }

    function sendBackButtonPressed()
    {
        if( (popUpLoader.source != "") && (sendBackMsgFromPopUp == false) )
        {
            popUpLoader.unloadPopUp()
            sendOnlyKeepAlive()
        }
        else
        {
            appData.data = EnumClass.GUI_BUTTON_BACK +
                       String.fromCharCode(uiMsgFsp).toString() +
                       String.fromCharCode(eotFsp).toString()
        }
    }

    QmlLogger
    {
        id: qmlLog
    }

    ApplController
    {
        id: appData

        property bool mainLoaderResp: false
        property bool popUpLoaderResp: false

        Component.onCompleted:
        {
            createBinding()
        }

        onSigDataReceived:
        {
            rcvData=data

            //Set footerConfigIdx based on Screen
            //Set the below flags based on MSG, for which response is supposed to be sent
            mainLoaderResp = false
            popUpLoaderResp = false
            statusBarId.visible = true

            qmlLog.debug("RECEIVED MSGID - " + msgId)

            switch(msgId)
            {
                default:
                    break

                case EnumClass.GUI_MSG_FR_SERV_ENABLE:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    break;
                }

                case EnumClass.GUI_MSG_STARTUP_INFO:
                {
                     if(PayloadLib.parseInitData(msgId, rcvData) )
                     {
                         mainWidget.setFontfamily()
                         statusBarId.visible = true
                         wallpaperId.visible = true
                         unloadAboveLayer()
                         mainLoader.source = "Idle.qml"
                         startUpImageId.source = ""

                     }
                    break
                }

                case EnumClass.GUI_MSG_SEND_CONFIG_INFO:
                {
                    PayloadLib.parseConfigData(msgId, rcvData)
                    break
                }
                case EnumClass.GUI_MSG_START_GUI_APPLICATION:
                {
                    startApplF = true
                    statusBarExpandedF = true
                    if(doorApplicationType != 2)//if not cafeteria
                    {
                        footerConfigIdx = EnumClass.FOOTER_IDLE
                        if(autoHideFooterBarF == true)
                        {
                            footerBarStart  = lcd_height
                            footerBarEnd    = lcd_height
                            footerBarId.y   = 0
                        }
                    }
                    break
                }

                case EnumClass.GUI_MSG_STATUSBAR_SCREEN:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    break
                }

                case EnumClass.GUI_MSG_GOING_IN_SCREEN:
                {

                    mainLoader.source = "GoingIn.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    break
                }
                case EnumClass.GUI_MSG_GOING_OUT_SCREEN:
                {
                    mainLoader.source = "GoingOut.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    break
                }

                case EnumClass.GUI_MSG_MENU_SCREEN:
                {
                    mainLoader.source = "Menu.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    footerBarStart  = lcd_height - footerBarId.height
                    footerBarEnd    = lcd_height - footerBarId.height
                    footerBarId.y   = 0
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    break
                }

                case EnumClass.GUI_MSG_DISPLAY_IDLE_SCREEN:
                {
                    if(statusBarId.visible == false)
                    {
                        statusBarId.visible = true
                    }
                    unloadAboveLayer()
                    mainLoader.source = ""
                    mainLoader.source = "Idle.qml"
                    footerConfigIdx = EnumClass.FOOTER_IDLE

					if(autoHideFooterBarF == true)
					{
		                footerBarStart  = lcd_height
		                footerBarEnd    = lcd_height
		                footerBarId.y   = 0
					}
                    break
                }

                case EnumClass.GUI_MSG_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING:
                {
                    if(statusBarId.visible == false)
                    {
                        statusBarId.visible = true
                    }
                    unloadAboveLayer()

                    mainLoader.source = "Idle.qml"
					mainLoader.item.processDeviceResponse (msgId,rcvData)
                    mainLoaderResp = true

                    if(autoHideFooterBarF == true)
                    {
                        if(doNotApplyAutoHideF == false)
                        {
                            footerBarStart  = lcd_height
                            footerBarEnd    = lcd_height
                        }
                        else
                        {
                            footerBarStart  = lcd_height - footerBarId.height
                            footerBarEnd    = lcd_height - footerBarId.height
                        }
                        footerBarId.y   = 0
                    }
                    break
                }

                case EnumClass.GUI_MSG_MENU_LOGIN_SCREEN:
                {
                    mainLoader.source = "PasswordEntry.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    hoverEnableF = false
                    break
                }

                case EnumClass.GUI_MSG_ENTER_USER_NAME:
                {
                    mainLoader.source = "EnterUserName.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    hoverEnableF = false
                    break
                }

                case EnumClass.GUI_MSG_ADMIN_MENU_SCREEN:
                {
                    mainLoader.source = "AdminMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN:
                {
                    // Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0

                    popUpLoader.loadDynamicScreen()
                    //footerConfigIdx will be updated based on flags provided by application
                    popUpLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_DYNAMIC_POP_UP_SCREEN:
                {
                    // Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0

                    popUpLoader.loadDynamicScreen()
                    //footerConfigIdx will be updated based on flags provided by application
                    popUpLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_HRD_MENU_SCREEN:
                {
                    mainLoader.source = "HRDMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ADMIN_SETTING_SCREEN:
                {
                    /* Added currentFeatureAccessed validation to Cover the NO Password Case of Slise Screen */
                    if(currentFeatureAccessed == -1)
                    {
                    mainLoader.source = "Settings.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    }
                    break
                }
                case EnumClass.GUI_MSG_DISPLAY_MENU_SCREEN:
                {
                    mainLoader.source = "DisplaySettings.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_BACKGROUND_SELECTION_SCREEN :
                {
                    mainLoader.source = "WallpaperSettings.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    break
                }
                case EnumClass.GUI_MSG_BRIGHTNESS_SELECTION_SCREEN:
                {
                    mainLoader.source = "BrightnessSettings.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_CLOCK_SELECTION_SCREEN :
                {
                    mainLoader.source = "ClockSelection.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    break
                }

                case EnumClass.GUI_MSG_PIN_ENTER_SCREEN:
                {
                    mainLoader.source = "PinEntry.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    hoverEnableF = false
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    break
                }

                case EnumClass.GUI_MSG_SPL_FUNC_STATUS:
                {
                    // Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0

                    statusBarId.unloadSlideScreen()
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/UserInput.qml"
                    //footerConfigIdx will be updated based on flags provided by application
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_NOTICE_TEXT:
                {
					// Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0
                    
                    mainLoader.source = "NoticeScreen.qml"
                    //footerConfigIdx will be updated based on flags provided by application
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_EHTERNET_CFG_SCREEN:
                {
                    mainLoader.source = "EthernetSetting.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    if( currentFeatureAccessed != -1 )
                    {
                        currentFeatureAccessed = -1
                    }
                    break
                }

                case EnumClass.GUI_MSG_ACMS_SERVER_CFG_SCREEN:
                {
                    mainLoader.source = "ServerSetting.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    if( currentFeatureAccessed != -1 )
                    {
                        currentFeatureAccessed = -1
                    }
                    break
                }

                case EnumClass.GUI_MSG_MODEM_PROFILE_DATA:
                {
                    mainLoader.source = "BroadbandSetting.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_WIFI_CFG_SCREEN:
                {
                    mainLoader.source = "WiFi.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    if( currentFeatureAccessed != -1 )
                    {
                        currentFeatureAccessed = -1
                    }
                    break
                }

                case EnumClass.GUI_MSG_VALIDATION_RESPONSE:
                {
                    mainLoaderResp = true
                    break
                }

				case EnumClass.GUI_MSG_KEYPAD_SELECTION_SCREEN :
                {
                    mainLoader.source = "KeypadSelection.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    break
                }

				case EnumClass.GUI_MSG_E_CANTEEN_MENU_SCREEN :
                {
                    mainLoader.source = "eCanteenMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ENROLLMENT_SCREEN :
                {
                    mainLoader.source = "Enrollment.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_MANAGE_CANTEEN_SCREEN:
                {
                    mainLoader.source = "ManageECanteen.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN :
                {
                    mainLoader.source = "DoorControlMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ADMIN_BACKUP_AND_UPDATE_SCREEN :
                {
                    mainLoader.source = "BackupUpdateMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    break
                }

                case EnumClass.GUI_MSG_DATE_TIME_SCREEN :
                {
                    mainLoader.source = "DateTime.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_USER_ALLOWED :
                {
                    mainLoader.source = "AllowScreen.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    statusBarId.unloadSlideScreen()
                    break
                }

                case EnumClass.GUI_MSG_USER_DENIED :
                {
                    mainLoader.source = "DenyScreen.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    statusBarId.unloadSlideScreen()
                    break
                }

                case EnumClass.GUI_MSG_USER_ALLOWED_COMMON_SCREEN :
                case EnumClass.GUI_MSG_USER_DENIED_COMMON_SCREEN :
                case EnumClass.GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN :
                case EnumClass.GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN :
                {
		    mainLoader.source = ""
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/CommonUserResponse.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    statusBarId.unloadSlideScreen()
                    break
                }

                case EnumClass.GUI_MSG_MENU_PASSWORD_SCREEN:
                {
                    mainLoader.source = "PasswordMenuScreen.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_CHANGE_PW_SCREEN:
                {
                    mainLoader.source = "PasswordSetting.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    hoverEnableF = false
                    break
                }

                case EnumClass.GUI_MSG_SEARCH_WIFI_SCREEN:
                {
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ENROLL_USER_SCREEN:
                {
                    showFooterBarIfHidden()
                    mainLoader.source = ""
                    mainLoader.source = "EnrollUser.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    isEnrollUserF = true
                    break
                }

                case EnumClass.GUI_MSG_SEARCHED_DEL_CRED_USER_LIST:
                case EnumClass.GUI_MSG_SEARCHED_ENROLL_USER_LIST:
                {
                    popUpLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_DEL_CRED_GET_NAME_FROM_ID:
                case EnumClass.GUI_MSG_ENROLL_GET_NAME_FROM_ID:
                {
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_MODEM_PROFILE_SCREEN:
                {
                    mainLoader.source = "BroadbandInfo.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    if( currentFeatureAccessed != -1 )
                    {
                        currentFeatureAccessed = -1
                    }
                    break
                }

                case EnumClass.GUI_MSG_SYS_STATUS_SCREEN :
                {
                    mainLoader.source = "AboutDevice.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN :
                {
                    mainLoader.source = "AboutDeviceConnectivity.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN :
                {
                    mainLoader.source = "AboutDeviceReaders.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN :
                {
                    mainLoader.source = "AboutDeviceAlarms.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ENROLL_SPL_FN_SCREEN :
                {
                    showFooterBarIfHidden()
                    mainLoader.source = (!doorFirmwareType) ? "EnrollSplFunction.qml" : "EnrollSplFunctionPanel.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_DEL_CRED_SCREEN :
                {
                    showFooterBarIfHidden()
                    mainLoader.source = ""
                    mainLoader.source = "DeleteCredential.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    isEnrollUserF = false
                    break
                }

                case EnumClass.GUI_MSG_CAFE_NOT_SERVING_SCREEN :
                {
                    statusBarId.unloadSlideScreen()
                    unloadAboveLayer()
                    mainLoader.source = "NotServing.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    break
                }

                case EnumClass.GUI_MSG_CAFE_ITEM_LIST_SCREEN:
                {
                    statusBarId.unloadSlideScreen()
                    mainLoader.source = "Cafe.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_CAFE_LIST_SCREEN_UPDATE:
                {
                    mainLoader.item.processDeviceResponse(msgId, rcvData)
                    break
                }

                case EnumClass.GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN:
                {
                    mainLoader.source = "CafeAvailableItems.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN :
                {
                    mainLoader.source = "CafeSoldOutItems.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_CAFE_USER_ALLOWED :
                {
                    mainLoader.source = "CafeAllow.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_CAFE_ITEM_SELECTION:
                {
                    mainLoader.item.processItemSelectionResponse(msgId, rcvData)
                    break
                }

                case EnumClass.GUI_MSG_CAFE_USER_DENIED :
                case EnumClass.GUI_MSG_CAFE_POLICY_USER_DENIED:
                {
                    mainLoader.source = "CafeDeny.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_BACKUP_UPDATE_PROG:
                {
                    mainLoader.source = "Upgrade.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_SET_MMC_DEV_PATH:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    break
                }

                case EnumClass.GUI_MSG_ZONE_CFG_SCREEN :
                {
                    mainLoader.source = "ZoneSettings.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_ZONE_CONTROL_SCREEN :
                {
                    mainLoader.source = "ZoneControlMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_ALARM_OPTIONS_SCREEN :
                {
                    mainLoader.source = "AlarmOptionMenu.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_VIEW_USER_PROFILE_SCREEN :
                {
                    mainLoader.source = "ViewUserProfile.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_WPS_PROGRESS_SCREEN:
                {
                    mainLoader.source = "WpsProgress.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_TOUCH_PANEL_TEST_SCREEN:
                {
                    mainLoader.source = "TouchTest.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    statusBarId.visible = false
                    break
                }

                case EnumClass.GUI_MSG_LCD_TEST_SCREEN:
                {
                    mainLoader.source = "LCDTest.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    statusBarId.visible = false
                    break
                }

                case EnumClass.GUI_MSG_SYSTEM_TEST_POPUP_SCREEN:
                {
                    popUpLoader.loadSysTestPopUp()
                    popUpLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_REFRESH_WALLPAPER:
                {
                    var temp1 = wallpaperIndex
                    wallpaperIndex = allWallpaper.length
                    wallpaperIndex = temp1

                    var temp2 = doorModeVisibleF
                    doorModeVisibleF = !doorModeVisibleF
                    doorModeVisibleF = temp2
                    break
                }

                case EnumClass.GUI_MSG_SELF_ENROLLMENT_SCREEN:
                {
                    mainLoader.source = "AskSelfEnrollment.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                   // mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_ASSIGN_USER_ON_DEVICE:
                {
                    mainLoader.source = "AskAssignUserOnDevice.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN:
                {
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/AskTempUserConfirmation.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }
                
                case EnumClass.GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY:
                {
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/TempUserRegistered.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }
                
                case EnumClass.GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN:
                {
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/EnterTemporaryUserName.qml"
                    footerConfigIdx =  EnumClass.FOOTER_HOME
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    hoverEnableF = false
                    break
                }
                
                case EnumClass.GUI_MSG_TEMP_USER_POPUP_SCREEN:
                {
                    // Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0

                    popUpLoader.loadTempUserPopUpScreen()
                    //footerConfigIdx will be updated based on flags provided by application
                    popUpLoaderResp = true
                    break
                }
                
                case EnumClass.GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE:
                {
                    mainLoader.source = "AskAddUserOnDevice.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT:
                {
                    mainLoader.source = "AskTransactionWithoutDiscount.qml"
                    footerConfigIdx =  EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_JOB_CODE_DISPLAY_SCREEN:
                {
                    //While punch using card, any screen on Z=1 will has to be removed
                    unloadAboveLayer()
                    mainLoader.source = "JobCodeDisplay.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    backVisibleF = false
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_JOB_CODE_LIST:
                {
                    mainLoader.item.processDeviceResponse (msgId,rcvData)
                    break
                }

                case EnumClass.GUI_MSG_NET_CONN_NOT_AVAILABLE_SCREEN:
                {
                    mainLoader.source = "NetConnNotAvailable.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    break
                }

                case EnumClass.GUI_MSG_DOOR_MODE_SELECTION_SCREEN:
                {
                    //While punch using card, any screen on Z=1 will has to be removed
                    unloadAboveLayer()
                    mainLoader.source = "DoorModeSelect.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    break
                }

                case EnumClass.GUI_MSG_MILTILANG_FONT_LOAD:
                {
                   textFontFamily = dispStr.loadFonts()
                   break
                }

                case EnumClass.GUI_MSG_MULTILANG_FLAG:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                   break
                }

                case EnumClass.GUI_MSG_INOUT_SPCL_FUNCTION_SELECTION:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    mainLoader.source = "InOutSpecialFunction.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
                    break
                }

                case EnumClass.GUI_MSG_INIT_FIRMWARE_UPGRADE :
                {
                    mainLoader.source = "FtpFirmwareUpgrade.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_SHOW_USER_ATTENDANCE_DETAIL:
                {
                    mainLoader.source = ""
                    mainLoader.source = "AttendanceDetail.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_APPLY_LEAVE_RESPONSE:
                {
                    mainLoader.item.processDeviceResponse (msgId,rcvData)
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_INSTALLATION_SCREEN_DEVICE_IP:
                {
                    mainLoader.source = "InstallationScreenDevIp.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    break
                }

                case EnumClass.GUI_MSG_INSTALLATION_SCREEN_SERVER_IP:
                {
                    mainLoader.source = "InstallationScreenServerIp.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    break
                }

                case EnumClass.GUI_MSG_INSTALLATION_SCREEN_VYOM_TENANT_ID:
                {
                    mainLoader.source = "InstallationScreenVyomTenantId.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    backVisibleF = false
                    homeVisibleF = false
                    break
                }

                case EnumClass.GUI_MSG_RELOAD_IMAGE:
                {
                    if(mainLoader.source == "qrc:/qml/Appl/Idle.qml")
                    {
                        mainLoader.item.processDeviceResponse (msgId,rcvData)
                    }
                    break;
                }
                case EnumClass.GUI_MSG_FR_STREAM_STATUS:
                {
                    PayloadLib.parsePayload(msgId, rcvData)

                    if( currentFeatureAccessed != -1 )
                    {
                        currentFeatureAccessed = -1
                    }
                    statusBarId.unloadSlideScreen()
                    break;
                }
                case EnumClass.GUI_MSG_START_AUTO_SYSTEM_TEST_SCREEN:
                {
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/TouchAutoSysTest.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    statusBarId.visible = false
                    break
                }

                case EnumClass.GUI_MSG_AUTO_SYSTEM_TEST_RUNNING_SCREEN:
                {
                    mainLoader.source = "AutoSystemTestRunning.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    break
                }

                case EnumClass.GUI_MSG_FR_CAMERA_CONNECTION_FAILED :
                {
                    mainLoader.source = "FailedToConnectCamera.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_ADAPTIVE_FR_SCREEN :
                {
                    mainLoader.source = "AdaptiveFR.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break
                }
                case EnumClass.GUI_MSG_APPL_MODE_INFO:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                   break
                }
                case EnumClass.GUI_MSG_FR_CAMERA_CONNECTION_FAILED_INFINITE :
                {
                    mainLoader.source = "FailedToConnectCameraInfinite.qml"
                    footerConfigIdx = EnumClass.FOOTER_IDLE
                    mainLoaderResp = true
                    footerBarStart  = lcd_height - footerBarId.height
                    footerBarEnd    = lcd_height - footerBarId.height
                    footerBarId.y   = 0
                    if(hideMenuBar.running)
                    {
                        hideMenuBar.stop()
                    }
                    break
                }

                case EnumClass.GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN:
                {
                    // Show footer bar
                    footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
                    footerBarStart  = lcd_height-footerBarId.height
                    footerBarEnd    = lcd_height-footerBarId.height
                    footerBarId.y   = 0

                    popUpLoader.loadDynamicTemperatureScreen()
                    //footerConfigIdx will be updated based on flags provided by application
                    popUpLoaderResp = true
                    break
                }

                case EnumClass.GUI_MSG_TEMPERATURE_MENU_SCREEN:
                {
                    mainLoader.source = "TemperatureReading.qml"
                    footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                    mainLoaderResp = true
                    break;
                }

                case EnumClass.GUI_MSG_AMBIENT_TEMP_DETECTING_SCREEN:
                {
                    popUpLoader.source = "AmbientTempDetecting.qml"
                    footerConfigIdx = EnumClass.FOOTER_HOME
					break;
                }
				case EnumClass.GUI_MSG_FACE_DETECTING_WAIT_SCREEN:
				{
                    			qmlLog.debug("Inside GUI_MSG_FACE_DETECTING_WAIT_SCREEN")
					sendBackMsgFromPopUp = true
                    popUpLoaderResp = true
					popUpLoader.source = "FaceEnrollDetecting.qml"
                    popUpLoader.item.useForCountDown = true
                    footerConfigIdx = EnumClass.FOOTER_ABORT_STOP_DISABLE
					break;
				}
				case EnumClass.GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN:
				{
                    popUpLoaderResp = true
                    popUpLoader.source = "FaceEnrollDetecting.qml"
                    popUpLoader.item.useForCountDown = false
					break;
				}
				case EnumClass.GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN:
                {
                    popUpLoader.source = ""
                    popUpLoaderResp = true
                    sendBackMsgFromPopUp = true
                    popUpLoader.source = "FaceEnrollNotification.qml"
                    break;
                }
                case EnumClass.GUI_MSG_DYNAMIC_USER_INFO_SCREEN:
                {
                    mainLoader.source = ":/../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Appl/DynamicUserInfoScreen.qml"
                    footerConfigIdx = EnumClass.FOOTER_NONE
                    mainLoaderResp = true
                    break;
                }

                case EnumClass.GUI_MSG_SET_OVERLAY_TEXT_ON_STREAM:
                {
                    if(mainLoader.source == "qrc:/qml/Appl/Idle.qml")
                    {
                        mainLoader.item.processDeviceResponse (msgId, rcvData)
                    }
                    break;
                }
                case EnumClass.GUI_MSG_LOG_INFO:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    break;
                }

                case EnumClass.GUI_MSG_WIFI_SIGNAL_STRENGTH:
                {
                    if(mainLoader.source == "qrc:/qml/Appl/WiFi.qml")
                    {
                        footerConfigIdx = EnumClass.FOOTER_BACK_HOME
                        mainLoader.item.processDeviceResponse (msgId,rcvData)
                    }
                    else
                    {
                        PayloadLib.parsePayload(msgId, rcvData)
                    }
                    break;
                }

                case EnumClass.GUI_MSG_DEVICE_INACTIVE:
                {
                    PayloadLib.parsePayload(msgId, rcvData)
                    break;
                }
            }

            if(mainLoaderResp == true)
            {
                unloadAboveLayer()
                mainLoader.item.processDeviceResponse (msgId,rcvData)
            }

            if(popUpLoaderResp == true)
            {
                statusBarId.unloadSlideScreen()
                popUpLoader.item.processDeviceResponse (msgId,rcvData)
            }
        }
    }

    Timer
    {
        interval: 1000; running: true; repeat: true;
        onTriggered:
        {
            mainWidget.timeChanged()
        }
    }

    DispStr
    {
        id:dispStr
    }


    function timeChanged()
    {

        var date     = new Date
        hours        = date.getUTCHours() + ((shift< 0)?(-Math.floor(Math.abs(shift))):(Math.floor(Math.abs(shift))))
        minutes      = date.getUTCMinutes() + ((shift % 1) * 60)
        seconds      = date.getUTCSeconds()
        currentDate  = date.getUTCDate()
        currentMonth = date.getUTCMonth()
        currentYear  = date.getUTCFullYear()
        currentDay   = date.getDay();

        var localDate = new Date(currentYear,currentMonth,currentDate,hours,minutes,seconds)
        currentDateMultiL  = localDate.getDate()              //Use MultiL variables in timeDispStr.
        currentMonthMultiL = localDate.getMonth()
        currentYearMultiL  = localDate.getFullYear()
        currentDayMultiL   = localDate.getDay();
    }

    function updateDoorStatus()
    {
        if(statusBarId.degradeModeF)
        {
            currentDoorStatus = EnumClass.DOOR_DEGRADEMODE
            degradeDoorModeType = statusBarId.degradeModeType
        }
        else if(statusBarId.networkFaultF)
        {
            currentDoorStatus = EnumClass.DOOR_NWFAULT
        }
        else
        {
            currentDoorStatus = EnumClass.DOOR_ONLINE
        }
    }

    function showFooterBarIfHidden()
    {
        footerBarHideF = false  //If card detect from time out screen (Not from Idle screen)
        if(autoHideFooterBarF === true || deviceInactive == true)
        {
            if(hideMenuBar.running)
            {
                hideMenuBar.stop()
            }

            footerBarStart  = lcd_height-footerBarId.height
            footerBarEnd    = lcd_height-footerBarId.height
            footerBarId.y   = 0
        }
    }

    Timer
    {
        id      : hideMenuBar
        interval: 3250
        repeat  : false

        onTriggered:
        {
            footerBarStart  = lcd_height-footerBarId.height
            footerBarEnd    = lcd_height
            footerBarId.y   = 0

            if(hideMenuBar.running)
            {
                hideMenuBar.stop()
            }
        }
    }
}
