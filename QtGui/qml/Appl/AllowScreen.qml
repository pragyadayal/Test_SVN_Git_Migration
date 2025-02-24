import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : allowRect
    height  : lcd_height
    width   : lcd_width

    property bool   userPhotoF          : false
    property string userIdForPhoto      : ""
    property int    dur                 : 3
    property int    msgTextWidth        : 200
    property bool   inOutVisibleF       : false

    Image
    {
       id: allowScreen
       source: imageLocation + "/Others/AllowDenyBackground.png"
       anchors.fill: parent
       cache: false

       Item
       {
           width : lcd_width
           height : 94
           anchors.top : parent.top
           anchors.topMargin: 20
           anchors.horizontalCenter: parent.horizontalCenter

           Column
           {
               spacing : 5
               anchors.centerIn: parent

               MxText
               {
                   id: allowText2
                   width : msgTextWidth
                   wrapMode: Text.Wrap
                   horizontalAlignment: Text.AlignHCenter
                   color: "#87e210"
                   font.pointSize: 15
                   text:""
               }

               MxText
               {
                   id: allowText3
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
               visible : (text.length > 0) ? true : false
           }

           MxText
           {
               id: userName
               anchors.top: userId.bottom
               anchors.topMargin: 15
               anchors.left: photoRect.right
               anchors.leftMargin: 42
               font.pointSize: 14
               font.bold: true
               text: ""
               color: "white"
               visible : (text.length > 0) ? true : false
           }

           MxText
           {
               id: designation
               anchors.top: userName.bottom
               anchors.topMargin: 15
               anchors.left: photoRect.right
               anchors.leftMargin: 42
               font.pointSize: 14
               font.bold: true
               text: ""
               color: "white"
               visible : (text.length > 0) ? true : false
           }
       }

       Item
       {
           width : lcd_width
           height : 95
           anchors.bottom : parent.bottom
           anchors.horizontalCenter: parent.horizontalCenter

           MxText
           {
              id: allowTime
              width : msgTextWidth
              wrapMode: Text.Wrap
              horizontalAlignment: Text.AlignHCenter
              anchors.top: parent.top
              anchors.topMargin: 2
              anchors.horizontalCenter: parent.horizontalCenter
              color:"white"
              font.pointSize: 20
              text:""
              visible : (text.length > 0) ? true : false
           }

           MxText
           {
              id: greetingtext
              width : msgTextWidth
              wrapMode: Text.Wrap
              horizontalAlignment: Text.AlignHCenter
              anchors.centerIn: parent
              color:"#f5ca04"
              font.pointSize: 18
              text:""
              visible : (text.length > 0) ? true : false
           }

           Column
           {
               anchors.centerIn: parent

               MxText
               {
                  id: jobCodeId
                  width : lcd_width
                  wrapMode: Text.Wrap
                  horizontalAlignment: Text.AlignHCenter
                  color:"white"
                  font.pointSize: 15
                  text:""
                  visible : (text.length > 0) ? true : false
               }

               MxText
               {
                  id: jobCodeName
                  width : lcd_width
                  wrapMode: Text.Wrap
                  horizontalAlignment: Text.AlignHCenter
                  color:"white"
                  font.pointSize: 15
                  text:""
                  visible : (text.length > 0) ? true : false
               }

               MxText
               {
                  id: jobAction
                  width : lcd_width
                  wrapMode: Text.Wrap
                  horizontalAlignment: Text.AlignHCenter
                  color:"white"
                  font.pointSize: 15
                  text:""
                  visible : (text.length > 0) ? true : false
               }
           }
       }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

        //console.log(tmpPayload)
        userId.text = tmpPayload[0]
        userName.text = tmpPayload[1]
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

        userPhotoF = parseInt(tmpPayload[2],10)
        userIdForPhoto = tmpPayload[7]
        loadUserPhoto()

        greetingtext.text = tmpPayload[3]
        allowText2.text = tmpPayload[4]
        allowText3.text = tmpPayload[5]

        jobCodeId.text = tmpPayload[8]
        jobCodeName.text = tmpPayload[9]
        jobAction.text = tmpPayload[10]

        allowTime.text = tmpPayload[11]
        designation.text = tmpPayload[12]
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
