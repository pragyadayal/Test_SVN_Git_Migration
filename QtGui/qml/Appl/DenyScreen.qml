import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id : denyRect
    height : lcd_height
    width  : lcd_width

    property bool   userInfoAvailableF   : false
    property int    denyReasonImageIndex : 0
    property string imgPath              : denyReasonImageIndex ? (imageLocation + "/AllowDenyInput/" + denyReasonImagePath[denyReasonImageIndex]) : ""

    property bool   userPhotoF          : false
    property string userIdForPhoto      : ""
    property int    dur                 : 3
    property int    msgTextWidth        : 200

    property variant denyReasonImagePath:
    [
        "",
        "AccessDenied.png",
        "FingerNotIdentified.png",
        "CardNotIdentified.png",
        "PinNotIdentified.png",
        "FaceNotFound.png",
        "Failed.png"
    ]

    OpacityRectangle
    {
        visible: userInfoAvailableF ? false : true
    }

    MxRectangleFrame
    {
        id: unidentifiedUser
        height  : msgBoxHeight
        width   : msgBoxWidth
        anchors.top: denyRect.top
        anchors.topMargin: 160
        anchors.horizontalCenter: parent.horizontalCenter
        visible: userInfoAvailableF ? false : true
        isBlankBox : true

        Column
        {
            id:denyInformation
            anchors.centerIn: parent
            spacing: 5

            Image
            {
                id: denyReasonImage
                anchors.horizontalCenter: parent.horizontalCenter
                visible: (denyReasonImageIndex == 0) ? false : true
                source: imgPath
            }

            MxText
            {
                id: denyText1
                width : msgBoxTextWidth
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: mxColorRed
                font.pointSize: fontSizePopUpLabel
                font.bold: true
                text:""
                visible : (text.length > 0) ? true : false
            }

            MxText
            {
                id: denyText2
                width : msgBoxTextWidth
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.pointSize: fontSizePopUpLabel
                text:""
                visible : (text.length > 0) ? true : false
            }

            MxText
            {
                id: denyText3
                width : msgBoxTextWidth
            //    wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.pointSize: fontSizePopUpLabel
                text:""
                visible : (text.length > 0) ? true : false
            }
        }
    }

    Item
    {
        id: identifiedUser
        visible : userInfoAvailableF ? true : false
        height  : parent.height
        width   : parent.width
        anchors.centerIn: parent

        Image
        {
            id: denyScreen
            source: imageLocation + "/Others/AllowDenyBackground.png"
			anchors.fill: parent
            cache: false

            Item
            {
                width : lcd_width
                height : 94
                anchors.top : parent.top
                anchors.topMargin: 15
                anchors.horizontalCenter: parent.horizontalCenter

                Column
                {
                    spacing : 5
                    anchors.centerIn: parent

                    MxText
                    {
                        id: denyText
                        width : msgTextWidth
                        wrapMode: Text.Wrap
                        horizontalAlignment: Text.AlignHCenter
                        color: "#fc6041"
                        font.pointSize: 15
                        text:""
                        visible : (text.length > 0) ? true : false
                    }

                    MxText
                    {
                        id: denyTextLine1
                        width : msgTextWidth
                        wrapMode: Text.Wrap
                        horizontalAlignment: Text.AlignHCenter
                        color: "white"
                        font.pointSize: 15
                        text:""
                        visible : (text.length > 0) ? true : false
                    }
                }
            }

            Rectangle
            {
                id:userInfoRect
                width: lcd_width
                height: 131
                anchors.top: parent.top
                anchors.topMargin: 104
                color: "transparent"

                Item
                {
                    id : photoRect
                    width: 98
                    height: 118
                    anchors.left: parent.left
                     anchors.leftMargin: 15
                    anchors.top : parent.top
                    anchors.topMargin: 70

                    Item
                    {
                        id: imageRect
                        width: 98
                        height: 118
                        anchors.centerIn: parent

                        Behavior on height
                        {
                            NumberAnimation{ duration: dur * 100; from : 0}
                        }
                        Behavior on width
                        {
                            NumberAnimation{ duration: dur * 100; from : 0}
                        }

                        AnimatedImage
                        {
                            id : userPhoto
                            anchors.fill : parent
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            source : ""
                            anchors.centerIn: parent

                            onSourceChanged:
                            {
                                imageRect.height = 118
                                imageRect.width = 98
                            }
                        }
                    }
                }

                MxText
                {
                    id: userId
                    anchors.top: parent.top
                    anchors.topMargin: 90
                    anchors.left: photoRect.right
                    anchors.leftMargin: 42
                    font.pointSize: 15
                    text: ""
                    color: "white"
                }

                MxText
                {
                    id: userName
                    anchors.top: userId.bottom
                    anchors.topMargin: 15
                    anchors.left: photoRect.right
                    anchors.leftMargin: 42
                    font.pointSize: 13
                    text: ""
                    color: "white"
                }

                MxText
                {
                    id: designation
                    anchors.top: userName.bottom
                    anchors.topMargin: 15
                    anchors.left: photoRect.right
                    anchors.leftMargin: 42
                    font.pointSize: 13
                    text: ""
                    color: "white"
                }
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        userInfoAvailableF  = parseInt(tmpPayload[0],10)

        userId.text         = tmpPayload[1]
        userName.text       = tmpPayload[2]
        if(userName.text.length == 0)
        {
            userId.anchors.topMargin = 55
            designation.anchors.topMargin = -25
        }
        else
        {
            userId.anchors.topMargin = 90
            designation.anchors.topMargin = 15
        }
        userPhotoF          = tmpPayload[3]
        userIdForPhoto      = tmpPayload[8]
        loadUserPhoto()

        if(userInfoAvailableF == false)
        {
            denyText1.text = tmpPayload[4]
            denyText2.text = tmpPayload[5]
            denyText3.text = tmpPayload[6]
        }
        else
        {
            denyText.text       = tmpPayload[4]
            denyTextLine1.text  = tmpPayload[5]
        }
        denyReasonImageIndex = parseInt(tmpPayload[7],10)
        designation.text = tmpPayload[9]
     }

    function loadUserPhoto()
    {
        if(userPhotoF)
        {
            if(userImagePath != "")
            {
               userPhoto.source =  appData.checkImage( userImagePath + userIdForPhoto )
                                   ? appData.createImagePath(userImagePath + userIdForPhoto)
                                   : defaultUserLargePhotoPath
            }
            else
            {
               userPhoto.source = defaultUserLargePhotoPath
            }
        }
        else
        {
            userPhoto.source = defaultUserLargePhotoPath
        }
        userPhoto.playing = true
    }
}
