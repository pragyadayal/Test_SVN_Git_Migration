import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: userSearchRect
    width  : lcd_width
    height: lcd_height

    property int        noOfUsers    :0
    property variant    userNamelist : [""]
    property variant    usrIdlist    : [""]
    property alias      writtenName : userNameInput.value

    signal select(int idx)

    OpacityRectangle
    {

    }

    MxRectangleFrame
    {
        id : userNameRectId
        width : 230
        height : 30
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30

        TextEditBox
        {
            id : userNameInput
            anchors.centerIn: parent
            isLargeTextBox: true
            maxChar: 32
            width : parent.width
            onFocusChanged:
            {
                sendOnlyKeepAlive()
                fillList()
            }
        }

        MxText
        {
            id : userSearchLabel
            anchors.verticalCenter: userNameInput.verticalCenter
            anchors.left: userNameInput.left
            anchors.leftMargin: 6
            color : "white"
            font.pointSize:fontSizeTextInputs
            text :dispStr.readDispStr(DispStr.MSG_SEARCH_USER_842) /*"Search User"*/
            visible: (writtenName.length > 0) ? false : true
            font.italic: true
        }

        Image
        {
            id: searchImg
            source: imageLocation + "/Controls/Search.png"
            anchors.verticalCenter: userNameInput.verticalCenter
            anchors.right: userNameInput.right
            anchors.rightMargin: 5
        }
    }

    MxRectangleFrame
    {
        id : mainRect
        width : 230
        height: alphaNumKeypadF ? 90 : 180
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : userNameRectId.bottom
        anchors.topMargin: 10

        MxText
        {
            id : noRecordsFoundText
            anchors.top : mainRect.top
            anchors.topMargin: 10
            anchors.horizontalCenter:  mainRect.horizontalCenter
            font.pointSize : fontSizeTextInputs
            color: "white"
            text : dispStr.readDispStr(DispStr.MSG_NO_RECORDS_FOUND_393) /*"No Records Found..."*/
            font.italic: true
            visible : ((noOfUsers > 0) || (writtenName.length == 0)) ? false : true
        }

        Component
        {
            id:listDelegate
            Item
            {
                id  : compItem
                width: mainRect.width - 10
                height: 30
                MouseArea
                {
                    id:mouse1
                    anchors.fill: parent
                    onClicked:
                    {
                        select(index)
                    }
                }
                MxText
                {
                    id:usrId
                    text: UserID
                    font.pointSize : fontSizeTextInputs
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left : parent.left
                    color: "white"
                    width:95
                    elide: Text.ElideRight
                }
				MxText
                {
                    id:name
                    text: UserName
                    font.pointSize : fontSizeTextInputs
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left : parent.left
                    anchors.leftMargin: 100
                    color: "white"
                    width:115
                    elide: Text.ElideRight
                }

                Seperator
                {

                }
            }
        }
        ScrollBar
        {
            noOfElements        : noOfUsers
            visibleElements     : (alphaNumKeypadF ? 3 : 6)
            position            : nameList.visibleArea.yPosition
        }

        ListModel
        {
            id:userNameModel
        }
        ListView
        {
            id                      : nameList
            height                  : mainRect.height
            width                   : mainRect.width - 10
            anchors.top             : mainRect.top
            anchors.horizontalCenter: parent.horizontalCenter

            model                   : userNameModel
            delegate                : listDelegate

            snapMode                : ListView.SnapToItem
            boundsBehavior          : Flickable.StopAtBounds
            clip                    : true
            cacheBuffer             : flickableCacheBuffer
            pressDelay              : flickablePressDelay
            smooth                  : true
            flickDeceleration       : flickableFlickDeceleration

            onMovementStarted:
            {
                unloadKeypad()
                sendOnlyKeepAlive()
            }
        }
    }

    Connections
    {
        target: alphaNumKeypadId
        onHidePress :
        {
            userNameInput.focus = false
            fillList()
        }
    }

    onWrittenNameChanged:
    {
        if(writtenName.length > 0)
        {
            askforUserList()
        }
        else
        {
            noOfUsers = 0
            fillList()
        }
    }

    onSelect:
    {
        sendOnlyKeepAlive()
        mainLoader.item.userSearchResult(userNamelist[idx])
        popUpLoader.unloadPopUp()
    }

    function askforUserList()
    {
        if(isEnrollUserF == true)
        {
            appData.data =  EnumClass.MSG_ENROL_SEARCH_USER_LIST_EVENT +
                            String.fromCharCode(uiMsgFsp).toString() +
                            userNameInput.value +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
        }
        else
        {
            appData.data =  EnumClass.MSG_DEL_CRED_SEARCH_USER_LIST_EVENT +
                            String.fromCharCode(uiMsgFsp).toString() +
                            userNameInput.value +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
        }
    }

    function fillList()
    {
        userNameModel.clear()
        var tmpPayload
        for(var usrindex = 0; (usrindex < noOfUsers); usrindex++)
        {
            tmpPayload = userNamelist[usrindex].split(String.fromCharCode (deshFsp).toString())
            userNameModel.insert (usrindex,
                                    {
                                       "UserName"   : tmpPayload[1],
                                       "UserID"     : tmpPayload[0],
                                   })
        }
    }

    function initUserSearch()
    {
        userNameModel.clear()
        userNameInput.forceActiveFocus()
        loadAlphaNumKeypad()
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        if(writtenName.length > 0)
        {
            fillList()
        }
        else
        {
            noOfUsers = 0
        }
    }
}
