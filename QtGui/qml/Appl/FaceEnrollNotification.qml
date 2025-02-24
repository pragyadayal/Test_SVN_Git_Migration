import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : faceEnrollConfirm
    width   : lcd_width
    height  : lcd_height
    focus	: true
    
	property int sideWidthMargin    : 10
	property int marginBwImgAndText : 15
	property int marginBwText       : 10
    property int countDown          : 0
	property int fromTime
	property int dur
	property int popuptype
	property int isWarningIconShow
	
	property string timeoutPayload	: ""
	
    OpacityRectangle
    {
		id			: opacityRectangleId
		visible		: true
    }
    
    Rectangle
    {
        id : setRectColour
        width                   : (lcd_width - sideWidthMargin)
        anchors.centerIn        : parent
        color                   : "transparent"

        Image
        {
            id                      : warningImg
            anchors.top             : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode                : Image.PreserveAspectFit
            source                  : imageLocation + "/Others/WarningIcon.png"
			visible: false
        }
        
        Rectangle
        {
            id                      : cnfrmFaceEnrollPopUp
            height                  : 100
            width                   : 300
            anchors.top             : (warningImg.visible == true) ? warningImg.bottom : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
			color                   : "transparent"
			
            MxText
            {
                id                      : userpopupstr
				width                   : parent.width
				anchors.top             : cnfrmFaceEnrollPopUp.top
				anchors.topMargin		: 5
				horizontalAlignment     : TextInput.AlignHCenter
				font.pointSize          : 20
				wrapMode                : Text.Wrap
				color                   : "white"
            }

            MxText
            {
                id                      : userpopuptimeoutstr
				width                   : parent.width
				anchors.top             : userpopupstr.bottom
				anchors.topMargin	: 5
				horizontalAlignment     : TextInput.AlignHCenter
				font.pointSize          : 16
				wrapMode                : Text.Wrap
				color                   : "#a4a4a4"
            }
            
			ButtonElement
			{
				id : buttons
				buttonRowVisibleF   : (popuptype == 1) ? false : true
				isInImageFormat     : true
				buttonCenterVisibleF : (popuptype == 1) ? true : false
				centerBtnImagePath : imageLocation + "/ButtonIcons/ok and set.png"
				leftBtnImagePath    : imageLocation + "/ButtonIcons/ok and set.png"
				rightBtnImagePath   : imageLocation + "/ButtonIcons/cancel.png"
				visible: true
				anchors.top         : userpopuptimeoutstr.bottom
                anchors.topMargin	: 110
				
                //YES
				onButtonLeftPress:
				{
					appData.data = EnumClass.GUI_BUTTON_FACE_ENRL_CONTINUE +
							String.fromCharCode(uiMsgFsp).toString() +
							String.fromCharCode(eotFsp).toString()
							countDown = 0
				}

                //NO
				onButtonRightPress:
				{
					appData.data = EnumClass.GUI_BUTTON_FACE_ENRL_CANCEL +
							String.fromCharCode(uiMsgFsp).toString() +
							String.fromCharCode(eotFsp).toString()
							countDown = 0
				}
                //ABORT
				onButtonCenterPress:
				{
                    appData.data = EnumClass.GUI_BUTTON_FACE_ENRL_CONTINUE_ABORT +
							String.fromCharCode(uiMsgFsp).toString() +
							String.fromCharCode(eotFsp).toString()
							countDown = 0
				}
			}
        }
    }

    Behavior on countDown
    {
        NumberAnimation
        {
            id: countDownAnim
			duration: dur
			from : fromTime
			to:0
		}
    }

    Image {
        id: processingImage
        anchors.top             : parent.top
        anchors.topMargin	: 90
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode                : Image.PreserveAspectFit
        source: imageLocation + "/PopUp/Processing.gif"
        visible: false
    }

    onCountDownChanged:
    {
			if(popuptype != 0)
                userpopuptimeoutstr.text = timeoutPayload + (" "+countDown +" "+dispStr.readDispStr(DispStr.MSG_SEC_863)/*"seconds..."*/)
			console.log(userpopuptimeoutstr.text)
			if(countDown === 0)
			{
				if(popuptype != 0)
				{
                    appData.data = EnumClass.GUI_BUTTON_POPUP_SCREEN_TIMEOUT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()
				}
            }
    }
    function processDeviceResponse (msgId,rcvData)
    {
		var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())
		
		fromTime = tmpPayload[0]
        dur = fromTime * 1000
        countDown = fromTime
		
		popuptype = parseInt(tmpPayload[1],10)
		if(popuptype == 0) // warning for face not detected
		{
			buttons.visible = false
			footerBarHideF = false
			opacityRectangleId.visible = false
		}
        else if(popuptype == 3)
        {
            buttons.visible = false
            footerBarHideF = true
            processingImage.visible = true
        }
		else
		{
			footerBarHideF = true
            userpopuptimeoutstr.text = tmpPayload[4] + (" "+countDown +" "+dispStr.readDispStr(DispStr.MSG_SEC_863)/*"seconds..."*/)
			timeoutPayload = tmpPayload[4]
		}
			
        isWarningIconShow = parseInt(tmpPayload[2],10)
        if(isWarningIconShow === 1)
			warningImg.visible = true
        userpopupstr.text = tmpPayload[3]
    }
}


