import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: faceEnrollDetectingItem
    width: lcd_width
    height: lcd_height

    property int countDown : 3
    // Face Enrollment related
    property int maxFaces               : 0
    property int noOfFacesProcessed     : 0
    property alias warningMsg: userpopupstr.text
    property bool useForCountDown: true
    property alias countDownAnimAlias: countDownAnimation

    // Count Down Cirlce
    Item
    {
        id: countDownCircleItem
        width: parent.width
        height: parent.height
        visible: false

        Rectangle
        {
            id: countDownCircle
            height: 50
            width:  50
            radius: 50
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            color: "blue"
            border.color: "blue"
            border.width: 2
            opacity: 0.5
        }

        MxText
        {
            id: tempCountdown
            wrapMode: Text.Wrap
            anchors.centerIn: countDownCircle
            horizontalAlignment: Text.AlignHCenter
            text: (countDown != 0) ? countDown : 0
            color   : "white"
            font.pointSize: 30
        }

        onVisibleChanged: {
            if(visible)
            {
                countDownAnimation.start()
                countDown = 3
            }
        }
    }

//    Behavior on countDown
//    {
        NumberAnimation on countDown
        {
            id: countDownAnimation
            running: false
            duration : 3000
            from : countDown
            to : 0
        }
//    }

    onCountDownChanged:
    {
        if(countDown == 0)
        {
            appData.data = EnumClass.GUI_MSG_FACE_DETECTING_COUNTDOWN_COMPLETED +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
            countDownCircleItem.visible = false
            progressBarItem.visible = true
        }
    }

    // Progress Bar
    Item
    {
        id: progressBarItem
        anchors.fill: parent
        visible: false

        Rectangle
        {
            id : progressOuterRect
            height : 20
            width : (lcd_width - 6)

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 55
            anchors.horizontalCenter: parent.horizontalCenter
            color: "transparent"
            border.color: "#303030"
            border.width: 2
            radius: 6
            smooth : true

            Rectangle
            {
                id: progressBarRect
                height: parent.height - 4
                width: 0
                radius: 6
                anchors{left: parent.left; leftMargin: 6; verticalCenter: parent.verticalCenter}
                color : "green"
            }
        }
    }

    //[
    Item
    {
        id : warningItem
        width                   : (lcd_width - 10)
        anchors.centerIn        : parent
        visible: false
//        color                   : "transparent"

        Image
        {
            id                      : warningImg
            anchors.top             : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode                : Image.PreserveAspectFit
            source                  : imageLocation + "/Others/WarningIcon.png"
        }

        Item
        {
            id                      : cnfrmFaceEnrollPopUp
            height                  : 100
            width                   : 300
            anchors.top             : (warningImg.visible == true) ? warningImg.bottom : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
//            color                   : "transparent"

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

                onTextChanged: {
                    if(text !== ""){
                        warningItem.visible = true
                    }
                    else{
                        warningItem.visible = false
                    }
                }
            }
        }
    }
    //]
    onNoOfFacesProcessedChanged: {
        progressBarRect.width = (noOfFacesProcessed/maxFaces) * (progressOuterRect.width - 10)
        console.log("Total face:"+maxFaces+",noOfFacesProcessed:"+noOfFacesProcessed+",progressBarRect.width:"+progressBarRect.width)
    }

    function processDeviceResponse(msgId, rcvData)
    {
        qmlLog.debug("Inside processDeviceResponse")
        PayloadLib.parsePayload(msgId, rcvData)

        if(useForCountDown == true){
            countDownCircleItem.visible = true
            progressBarItem.visible = false
        }
        else{
            countDownCircleItem.visible = false
            progressBarItem.visible = true
        }
    }
    Component.onCompleted: {
        console.log("FaceEnrollDetecting initialized");
        if(enrollPopUpActive !== true)
        {
            enrollPopUpActive = true
            showFooterBarIfHidden()
        }
    }

    Component.onDestruction: {
       if(enrollPopUpActive !== false)
            enrollPopUpActive = false
    }
}
