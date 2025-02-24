import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

MxRectangleFrame
{
    id : mainInfoRect
    width: 236
    height: 70
    anchors.horizontalCenter: parent.horizontalCenter

    property string userId          : ""
    property string userName        : ""
    property bool   userPhotoF      : false
    property string userIdForPhoto  : ""

    property int labelWidth : 50
    property int dataWidth : 164

    Item
    {
        height : 55
        width : 55
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
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
        anchors.right			: parent.right
        anchors.rightMargin		: 10
        anchors.verticalCenter	: parent.verticalCenter
        spacing 				: 7

        Rectangle
        {
            id 		: userIdValueRect
            color   : "black"
            height  : 25
            width   : 150
            visible : true

            MxText
            {
                id : userIdValue
                width : dataWidth - 12
                anchors.horizontalCenter: userIdValueRect.horizontalCenter
                anchors.left: userIdValueRect.left
                anchors.leftMargin: 8
                text : userId
                color : "white"
                font.pointSize: fontSizeUserInfo
                clip : true
            }
        }
        Rectangle
        {
            id 		: userNameValueRect
            color   : "black"
            height  : 25
            width   : 150
            visible : true

            MxText
            {
                id : userNameValue
                text : userName
                anchors.horizontalCenter: userNameValueRect.horizontalCenter
                anchors.left: userNameValueRect.left
                anchors.leftMargin: 8
                width : dataWidth - 12
                color : "white"
                font.pointSize: fontSizeUserInfo
                clip : true
                smooth : true
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
