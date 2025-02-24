import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib
Item
{
    id : idlePage
    height : lcd_height
    width  : lcd_width

    property string doorModeImg         	:"/Others/"+((doorMode == 1)?"Exit":"Entry")+".png"
    property string tapForImg               : "/Others/"+"TapForFaceCapture.png"
    property int mouseYstartForFooterBar	: 00
    property string filePath                :"file:/"
    property string qtImage                 :"/tmp/qt.jpg"
    property bool   isOverLayUserInfoVisible : false

    Item
    {
        id          : deviceActive
        height      : lcd_height
        width       : lcd_width
        visible     : !deviceInactive || enrollPopUpActive

        Item
        {
            id          : idleScreenPage
            visible     : (!frServerEnableF) || (deviceInactive)
            height      : lcd_height
            width       : lcd_width
            anchors.fill: parent

            Rectangle
            {
                id : clockRect
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top : parent.top
                anchors.topMargin: (clockIndex != 0) ?  55 :  30
                width:240
                height: 150
                color: "transparent"
                AnalogClock
                {
                    id : analogClock
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible : (clockIndex == 0) ? true : false
                }
                DigitalClock
                {
                    id : digital
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible : (clockIndex != 0) ?  true :  false
                }
            }

            Image
            {
                id: doorModeId
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: clockRect.bottom
                anchors.topMargin: 30
                source: imageLocation + doorModeImg
                visible: (doorModeVisibleF && showWallPaperF)
                         ? (appData.checkImage( doorModeCustomWallpaper[doorMode])
                            ? false : (appData.checkImage( doorModeDfltWallpaper[doorMode]) ? false : true) )
                         : (doorModeVisibleF ? true : false)
            }

        }

        //Item for dispalying image
        Item
        {
            id                      : imgDisplayScreen
            visible                 : frServerEnableF && (!deviceInactive || enrollPopUpActive)
            anchors.top             : parent.top
            anchors.topMargin       : 25
            width                   : lcd_width
            height                  : lcd_height-70
            anchors.horizontalCenter: parent.horizontalCenter

            // Tap For Icon
            Item
            {
                id                      : tapForIcon
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top             : parent.top
                anchors.topMargin       : 25
                width                   : 120
                height                  : 120
                //z                       : 1
                visible                 : (frStreamDisplayF == 0) ? true : false  // false=TapForIcon, true=FRStream

                onVisibleChanged: {
                    if(visible == true){
                        var qmlSrc = popUpLoader.source
                        if(qmlSrc == "qrc:/qml/Appl/FaceEnrollDetecting.qml"){
                            popUpLoader.unloadPopUp()
                        }
                    }
                }

                Image
                {
                    id                      : tapForImage
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.centerIn        : parent
                    width                   : 120
                    height                  : 120
                    fillMode                : Image.PreserveAspectFit
                    source                  : imageLocation + tapForImg
                    smooth                  : true
                }

            }

            // Display Video Stream
            Image
            {
                id          : snapFrmCamera
                anchors.top : parent.top
                width       : lcd_width
                height      : lcd_height-25
                cache       : false
                //            source      : (frStreamDisplayF == 1) ? (filePath+qtImage) : ""
                source      : (filePath+qtImage)
                visible     : (frStreamDisplayF == 1) ? true : false  // false=TapForIcon, true=FRStream

                onStatusChanged:
                {
                    if(snapFrmCamera.status==Image.Ready)
                    {
                        appData.removeFile(qtImage);
                    }
                    if(snapFrmCamera.status==Image.Error)
                    {
                        if(snapFrmCamera.source==filePath+qtImage)
                        {
                            appData.removeFile(qtImage);
                        }
                    }
                }
            }

            // FR Stream Overlay
            Item
            {
                id                      : frStreamOverlay
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom          : snapFrmCamera.bottom
                anchors.bottomMargin    : footerBarHeight + 1
                width                   : parent.width
                height                  : (lcd_height/4)
                visible                 : false
                z: 2

                Rectangle {
                    id: overLayTextRect
                    width: parent.width - 20
                    height: (lcd_height/12)
                    radius: 4
                    anchors.bottom: (isOverLayUserInfoVisible == true) ? idleUserInfo.top : frStreamOverlay.bottom
                    anchors.bottomMargin: 3
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "black"
                    opacity: 0.7
                    visible: false

                    MxText {
                        id: overLayText
                        anchors.centerIn: parent
                        font.pointSize: fontSizeTextInputs + 2
                        font.bold: true
                        color: "white"
                        text: ""
                        elide : Text.ElideRight
                        width: parent.width * 0.9
                        horizontalAlignment     : TextInput.AlignHCenter
                    }
                }

                Rectangle
                {
                    id:idleUserInfo
                    width: parent.width
                    height: (lcd_height/6)
                    anchors.bottom: frStreamOverlay.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "black"
                    opacity: 0.7
                    visible: isOverLayUserInfoVisible

                    MxText
                    {
                        id: userId
                        anchors.top: parent.top
                        anchors.topMargin: 6
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 22
                        text: ""
                        color: "white"
                        elide : Text.ElideRight
                        width: parent.width * 0.9
                        horizontalAlignment     : TextInput.AlignHCenter

                        onTextChanged:
                        {
                            if(text === "")
                                userId.visible = false
                            else
                                userId.visible = true

                        }
                    }

                    MxText
                    {
                        id: userName
                        anchors.top: (userId.visible) ? (userId.bottom) : (parent.top)
                        anchors.topMargin:(userId.visible) ? 4 : 23
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 24
                        font.bold: true
                        text: ""
                        elide : Text.ElideRight
                        width: parent.width * 0.9
                        horizontalAlignment     : TextInput.AlignHCenter
                        color: "white"
                    }
                }
            }
        }
    }

    Item
    {
        id          : deviceInactiveItem
        height      : lcd_height
        width       : lcd_width
        visible     : deviceInactive && !enrollPopUpActive

        /* background image */
        OpacityRectangle
        {
            anchors.fill: parent
        }

        Image
        {
            id : unauthorisedUserImg
            source : imageLocation + "/PopUp/UnauthorisedUserLarge.png"
            visible: true
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -10
        }

        MxText {
            id: idleText
            width : parent.width
            opacity: 0.8
            anchors.top: unauthorisedUserImg.bottom
            anchors.topMargin: 10
            font.pixelSize      : 23
            font.bold           : true
            color               : "white"
            text                : deviceInactiveStr
            elide               : Text.ElideRight
            maximumLineCount    : 2
            wrapMode            : Text.WordWrap
            horizontalAlignment : TextInput.AlignHCenter
            verticalAlignment : TextInput.AlignVCenter
            visible : true
        }
    }

    // Auto Hide FooterBar
    Item
    {
        anchors.fill: parent
        visible: !(deviceInactive && hideMenuForDeviceInactive)

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                if( (autoHideFooterBarF == true) && (footerBarId.y !== lcd_height-footerBarId.height) )
	            {
                    mouseYstartForFooterBar = mouseY;
	            }
            }

            onMousePositionChanged:
            {
                if( (autoHideFooterBarF == true) && (footerBarId.y !== lcd_height-footerBarId.height) )
                {
                    if( (mouseYstartForFooterBar - mouseY) > 40)
                    {
                        mouseYstartForFooterBar = 0   //If continue press then make starting point value to zero
                        footerBarStart  = lcd_height
                        footerBarEnd    = lcd_height-footerBarId.height
                        footerBarId.y   = 0
                        hideMenuBar.start()
                     }
                }
            }
        }
    }

    // Tap For Touch screen area
    Item
    {
        id                      : tapForIconTouchScreenEvent
        anchors.top             : parent.top
        anchors.topMargin       : 45
        width                   : 120
        height                  : 120
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea
        {
            id                      : tapForMouseArea
            anchors.fill            : parent
            enabled                 : (frServerEnableF && !deviceInactive) ? true : false

            onClicked:
            {
                appData.data = EnumClass.GUI_BUTTON_TAP_FOR_FR +
                               String.fromCharCode(uiMsgFsp).toString() +
                               String.fromCharCode(eotFsp).toString()
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        switch(msgId)
        {
            case EnumClass.GUI_MSG_RELOAD_IMAGE:
            {
                PayloadLib.parsePayload(msgId, rcvData)
                var QTFilestatus = appData.isFilePresent(qtImage);
                if(QTFilestatus == true)
                {
                    //console.log("## DEBUG QT File present ")
                    snapFrmCamera.source = ""
                    snapFrmCamera.source = filePath+qtImage
                }
                else
                {
                    //console.log("## DEBUG QT File NOT present ")
                }
            }
            break;

            case EnumClass.GUI_MSG_FR_STREAM_STATUS:
            {
                PayloadLib.parsePayload(msgId, rcvData)
            }
            break;

            case EnumClass.GUI_MSG_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING:
            {
                var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

                /* Remove already existing image but do not unload source */
                appData.removeFile(qtImage);

                if (frStreamDisplayF == 1)
                {
                    frStreamOverlay.visible = parseInt(tmpPayload[0], 10)
                    overLayTextRect.visible = parseInt(tmpPayload[1], 10)
                    overLayText.text = tmpPayload[2]

                    isOverLayUserInfoVisible = parseInt(tmpPayload[3], 10)
                    userId.text = tmpPayload[4]
                    if(userId.text === "0")
                    {
                         userId.text = ""
                    }

                    userName.text = tmpPayload[5]
                    footerConfigIdx = parseInt(tmpPayload[6], 10)
                }
                doNotApplyAutoHideF = parseInt(tmpPayload[7], 10);
            }
            break;

            case EnumClass.GUI_MSG_SET_OVERLAY_TEXT_ON_STREAM:
            {
                var payload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

                if(frStreamDisplayF == 1)
                {
                    frStreamOverlay.visible = parseInt(payload[0], 10)
                    overLayTextRect.visible = parseInt(payload[1], 10)
                    if(overLayTextRect.visible)
                        overLayText.text = payload[2]
                }
            }
            break;

            default:
                break;
        }
    }

    Component.onCompleted: {
        statusBarId.unloadSlideScreen()
        statusBarId.startStopSlideScreen(true)
        if(enrollPopUpActive !== false)
            enrollPopUpActive = false
    }
}


