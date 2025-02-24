import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

MxRectangleFrame
{
    id      : mainUserInfoRect
    width   : 280
    height  : 100
    anchors.horizontalCenter: parent.horizontalCenter

    property alias  userId          : userIdValue.value
    property alias  userIdWaterMark : userIdText.text
    property alias  userName        : userNameValue.text
    property bool   userPhotoF      : true
    property string userIdForPhoto  : ""
    property alias  userIdLen       : userIdValue.maxChar
    property int    enrollIdType    : 0

    property int labelWidth : 80
    property int dataWidth : 120
    property alias userIdActiveF: userIdValue.isEnable
    property alias userNameActiveF: textuserName.isEnable
    property alias userNameTouchActiveF: touchArea.enabled

    Item
    {
        height : 80
        width  : 80
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        AnimatedImage
        {
            id : userPhoto
            anchors.fill : parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            source : defaultUserPhotoPath
        }
    }

    Column
    {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right : parent.right
        anchors.rightMargin : 10
        spacing : 8

        TextEditBox
        {
            id : userIdValue
            width : 160
            MxText
            {
                id : userIdText
                text : userIdWaterMark
                anchors.verticalCenter: userIdValue.verticalCenter
                anchors.left: userIdValue.left
                anchors.leftMargin: 8
                color:"white"
                font.pointSize: fontSizeTextInputs
                visible : (userIdValue.value.length > 0) ? false : true
            }
        }

        TextEditBox
        {
           id : textuserName
            width : 160
            validation:
                RegExpValidator {
                    regExp: /[\x00-\x7F]+/
            }
            textBoxActivatedF: false

            MxText
            {
                id : userNameValue
                text : userName
                anchors.left	  : parent.left
                anchors.leftMargin: 8
                width : dataWidth
                color : "white"
                font.pixelSize: fontSizeTextInputs
                clip : true
                smooth : true
                maximumLineCount: 1
                elide: Text.ElideRight
                wrapMode: Text.WrapAnywhere
            }

            MouseArea
            {
                id : touchArea
                anchors.fill: parent
                onClicked:
                {
                    popUpLoader.loadUserSearchScreen()
                }
            }
        }
    }

    function loadUserPhoto()
    {
        if(userPhotoF)
        {
            if(userImagePath != "")
            {
               userPhoto.source =  appData.checkImage( userImagePath + userIdForPhoto )
                                   ? appData.createImagePath(userImagePath + userIdForPhoto)
                                   : defaultUserPhotoPath
            }
            else
            {
               userPhoto.source = defaultUserPhotoPath
            }
            userPhoto.playing = true
        }
    }
}
