import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : window
    height  : lcd_height
    width   : lcd_width

    property int fontsizeUserID         : 12      // Font Size of User ID
    property int fontsizeUserName       : 14      // Font Size of User Name
    property int boxSizeUserinfo        : 55      // Size of Box (User info)

    property bool userPhotoF                        : true
    property bool refreshInPrgrsF                   : false
    property bool presentMnthVisblF                 : true
    property bool directionChngMultplTimeF          : false
    property bool aplyScrndirectionChngMultplTimeF  : false
    property bool atndncCorrectionVisibleF          : false
    property bool nxtScrnIncrCount                  : false

    property int mouseXstart            : 00
    property int mouseYstart            : 00
    property int mouseXprvs             : 00
    property int mouseYprvs             : 00
    property int scrren1leftMargin      : 00
    property int startleftMargin        : 00
    property int xdiff                  : 00
    property int ydiff                  : 00
    property int direction              : 00
    property int currentScreenNo        : 1
    property int attendanceTimer        : 00
    property int lastUpdatedHours       : 00
    property int lastUpdatedMinutes     : 00
    property int lastUpdatedSeconds     : 00
    property int lastUpdatedDate        : 00
    property int lastUpdatedMonth       : 00
    property int lastUpdatedYear        : 0000
    property int currentLeaveCodeNmbr   : 00
    property int currentAbsentScreen    : 00
    property int lastAbsentScreen       : 00
    property int maxLeaveCode           : 00
    property int aplyLeaveRspns         : 0

    property string userIdForPhoto      : ""
    property string userIdx             : ""
    property string aplyLeaveRspnsStrng : ""

    property variant    absentDates     : [""]
    property variant    firstHalf       : [""]
    property variant    secondHalf      : [""]
    property variant    leaveTypes      : []
    property variant    leaveCodes      : [""]
    property variant    balance         : []
    property variant    leaveTypesNames : ["Paid Leave","Lay off","Unpaid","Restricted Holiday","Compensatory Off","Tour"]

    Rectangle
    {
        color           : "#000000"
        anchors.fill    : parent
    }

    OpacityRectangle
    {
        opacity : 0.6
    }

    ApplyLeaveScreen
    {
        id : leaveApplyScreen
    }

    // Main Rectangle
    Item
    {
        id					: mainRect
        height 				: lcd_height
        width 				: lcd_width
        anchors.bottom		: parent.bottom
        anchors.top			: parent.top
        anchors.left		: parent.left
        anchors.leftMargin	: scrren1leftMargin

        //User Photo and UserID/Name
        MxRectangleFrame
        {
            id                          : userInfoId
            width                       : parent.width
            height                      : boxSizeUserinfo
            anchors.top                 : parent.top
            anchors.topMargin           : 23
            property int enrollIdType   : 0
            property int labelWidth     : 80
            property int dataWidth      : 120

            // User Photo
            Item
            {
                id                      : userPhotoBox
                height                  : boxSizeUserinfo-5
                width                   : boxSizeUserinfo-5
                anchors.verticalCenter  : parent.verticalCenter
                anchors.left            : parent.left
                anchors.leftMargin      : 10
                AnimatedImage
                {
                    id              : userPhoto
                    anchors.fill    : parent
                    fillMode        : Image.PreserveAspectFit
                    smooth          : true
                    source          : defaultUserPhotoPath
                }
            }

            // User ID and User Name
            Column
            {
                anchors.verticalCenter  : parent.verticalCenter
                anchors.left            : userPhotoBox.right
                anchors.leftMargin      : 5
                spacing                 : 5

                //User ID
                MxText
                {
                    id              : userIdText
                    text            : ""
                    anchors.left    : parent.left
                    color           : "#d8d8d8"
                    font.pointSize  : fontsizeUserID
                }

                //User Name
                MxText
                {
                    id              : userNameText
                    text            : ""
                    anchors.left    : parent.left
                    color           : "#d8d8d8"
                    font.pointSize  : fontsizeUserName
                    elide           : Text.ElideRight
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

        // Present Month
        MxAttnDtlsMonthElem
        {
            id              : presentMonth
            anchors.top     : userInfoId.bottom
            visible         : presentMnthVisblF
            opacity         : 1
            presentMonthF   : true
        }

        // Previous Month
        MxAttnDtlsMonthElem
        {
            id              : previousMonth
            anchors.top     : userInfoId.bottom
            visible         : !presentMnthVisblF
            opacity         : 0
            presentMonthF   : false
        }

        // Footer Bar : Updated on + Home
        MxRectangleFrame
        {
            id                      : lastUpdateTimeRect
            height                  : 34
            width                   : parent.width
            anchors.bottom          : parent.bottom
            mxSubRectopacity        : 0

            Row
            {
                id                      : lastUpdateTimeRow
                spacing                 : 5
                anchors.left            : parent.left
                anchors.leftMargin      : 5
                anchors.verticalCenter  : parent.verticalCenter

                AnimatedImage
                {
                    id: refreshPrgsImg
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: imageLocation + "/Others/RefreshProgress.gif"
                    visible: refreshInPrgrsF
                    anchors.verticalCenter: parent.verticalCenter
                }
                AnimatedImage
                {
                    id: refreshImage
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: imageLocation + "/Others/RefreshIcon.png"
                    visible: (!refreshInPrgrsF)
                    anchors.verticalCenter: parent.verticalCenter
                }

                MxText
                {
                    id              : updateText
                    text            : dispStr.readDispStr(DispStr.MSG_UPDATED_ON_894) /*"Updated On"*/+" : "
                    color           : "#898989"
                    font.pointSize  : 11
                    font.italic     : true
                    font.bold       : true
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment:Text.AlignVCenter
                }

                MxText
                {
                    id              : updateInfoText
                    text            : (lastUpdatedYear === 1970) ? "" : dispStr.timeDispStr(lastUpdatedYear, lastUpdatedMonth,lastUpdatedDate,0, lastUpdatedHours,lastUpdatedMinutes,0,DispStr.DD_MMM_YYYY_HH_MM)
                    color           :"#898989"
                    font.pointSize  : 11
                    font.italic     : true
                    width           : 100
                    font.bold       : true
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment:Text.AlignVCenter
                }
            }

            MouseArea
            {

                anchors.fill: parent
                enabled     : true
                onPressed:
                {
                    refreshInPrgrsF = true
                    appData.data = EnumClass.GUI_BUTTON_REFRESH_ATTNDNS_DTL +
                                            String.fromCharCode(uiMsgFsp).toString() +
                                            userIdx + String.fromCharCode(uiMsgFsp).toString() +
                                            String.fromCharCode(eotFsp).toString()
                }
            }

            Item
            {
                anchors.right           : parent.right
                anchors.rightMargin     : 10
                height                  : parent.height
                width                   : 20
                anchors.verticalCenter  : parent.verticalCenter
                Image
                {
                    id              : homeImage
                    anchors.centerIn: parent
                    fillMode        : Image.PreserveAspectFit
                    source          : imageLocation + "/Footer/Home.png"
                    smooth          : true
                    height          : homeImageArea.pressed ? 16 : 22
                    width           : homeImageArea.pressed ? 16 : 22
                }

                MouseArea
                {
                    id              : homeImageArea
                    anchors.centerIn: homeImage
                    height  		: parent.height
                    width           : homeImage.width+10
                    enabled         : true
                    onPressed:
                    {
                        appData.data = EnumClass.GUI_BUTTON_HOME +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    }
                }
            }
        }
    }

    function screenChanges()
    {
        if( ( (currentScreenNo == 1) && ( (direction === 1) || (direction === 2) ) ) ||
            ( (direction === 1) && (currentAbsentScreen == lastAbsentScreen) ) ||
            ( (direction === 2) && (currentAbsentScreen == 1) ) )
        {
            window.state = "scrren1leftMargin"
            scrren1leftMargin = (currentScreenNo == 1) ? 0 : -lcd_width
            if(currentScreenNo == 1)
            {
                currentLeaveCodeNmbr = 1
                if(direction === 1)
                {
                    window.state = "screen1onRight"
                    scrren1leftMargin = -lcd_width
                    currentAbsentScreen = 1
                    nxtScrnIncrCount = true
                }
                else
                {
                   currentAbsentScreen = lastAbsentScreen
                    nxtScrnIncrCount = false
                }
                leaveApplyScreen.absntDateColor = (leaveApplyScreen.absntDateColor == "#d8d8d8") ? "black" : "#d8d8d8"
            }
            else
            {
                if(direction === 2)
                {
                    window.state = "screen1onRight"
                    scrren1leftMargin = 0
                }
            }
            startleftMargin = scrren1leftMargin;
        }
    }
    // Swipe Area
    Item
    {
        height              : parent.height - 70  //(total height - StatusBar(30) - updatedOnBar(40))
        width               : parent.width
        anchors.top         : parent.top
        anchors.topMargin   : 30

        MouseArea
        {
            id              : touchArea
            height  		: parent.height
            width           : parent.width
            enabled         : true

            onPressed:
            {
                mouseXstart     = mouseX;
                mouseYstart     = mouseY;
                mouseXprvs      = mouseX;
                direction       = 0
                attendanceTimer = 0

                appData.data = EnumClass.GUI_BUTTON_DELETE_IDLE_SCREEN_TIMER +
                String.fromCharCode(uiMsgFsp).toString() +
                String.fromCharCode(eotFsp).toString()
            }

            onReleased:
            {
                applyMouseChanges(mouseX)
                attendanceTimer = 0
                if(direction == 0) // on press and hold screen for 3 sec
                {
                    attendanceTimer = 3
                }
                else // on swipe load screen for 5 sec
                {
                    attendanceTimer = 5
                }

                appData.data = EnumClass.GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER +
                                        String.fromCharCode(uiMsgFsp).toString() +
                                        attendanceTimer + String.fromCharCode(uiMsgFsp).toString() +
                                        String.fromCharCode(eotFsp).toString()

                switch(direction)
                {
                case 1:
                case 2:
                {
                    if( (currentScreenNo == 1) ||
                        ( (direction === 1) && (currentAbsentScreen == lastAbsentScreen) ) ||
                        ( (direction === 2) && (currentAbsentScreen == 1) ) )
                    {
                        var diffMargin = (scrren1leftMargin > startleftMargin) ? (scrren1leftMargin - startleftMargin) : (startleftMargin - scrren1leftMargin)
                        if(diffMargin > lcd_width/3)
                        {
                            currentScreenNo = (currentScreenNo == 1) ? 2 : 1
                        }
                    }
                    else
                    {
                        leaveApplyScreen.opacity = 1
                        currentAbsentScreen = ( currentAbsentScreen + ( (direction === 1) ? +1 : -1) )
                        leaveApplyScreen.absntDateColor = (leaveApplyScreen.absntDateColor == "#d8d8d8") ? "black" : "#d8d8d8"
                    }
                    window.state = "scrren1leftMargin"
                    scrren1leftMargin = (currentScreenNo == 1) ? 0 : -lcd_width
                    startleftMargin = scrren1leftMargin;

                    if(currentScreenNo == 1)
                    {
                        currentAbsentScreen = 0
                        currentLeaveCodeNmbr = 0
                    }
                    else
                    {
                        currentLeaveCodeNmbr = 1
                    }
                    aplyLeaveRspnsStrng = ""
                    atndncCorrectionVisibleF = false
                  }
                break

                case 3:
                case 4:
                {
                    if(currentScreenNo == 1)
                    {
                        if(directionChngMultplTimeF === false)
                        {
                            directionChngMultplTimeF = true
                            if(presentMnthVisblF == true)
                            {
                                presentMnthVisblF = false
                                presentMonth.opacity = 0
                                previousMonth.opacity = 1
                            }
                            else
                            {
                                presentMnthVisblF = true
                                previousMonth.opacity = 0
                                presentMonth.opacity = 1
                            }

							// For blink month name and year
                            if(presentMnthVisblF == true)
                            {
                                presentMonth.monthColor1 = "#58afeb"
                                presentMonth.monthColor = (presentMonth.monthColor == "#919191") ? "black" : "#919191"
                            }
                            else
                            {
                                previousMonth.monthColor1 = "#d8d8d8"
                                previousMonth.monthColor = (previousMonth.monthColor == "#919191") ? "black" : "#919191"
                            }
                        }
                    }
                    else if(currentScreenNo == 2)
                    {
                        if(aplyScrndirectionChngMultplTimeF === false)
                        {
                            aplyScrndirectionChngMultplTimeF = true
                        }
                    }
                    aplyLeaveRspnsStrng = ""
                    leaveApplyScreen.opacity = 1
                }
                break
                }
            }

            onMousePositionChanged:
            {
                xdiff = (mouseXstart > mouseX) ? mouseXstart - mouseX : mouseX - mouseXstart
                ydiff = (mouseYstart > mouseY) ? mouseYstart - mouseY : mouseY - mouseYstart

                if(direction === 0)
                {
                    if(xdiff > ydiff)
                    {
                        if(xdiff > 10)
                        {
                           direction = (mouseXstart > mouseX) ? 2 : 1
                        }
                    }
                    else if(ydiff > 10)
                    {
                        direction = (mouseYstart > mouseY) ? 4 : 3
                    }
                    if(direction !== 0)
                    {
                        directionChngMultplTimeF = false
                        aplyScrndirectionChngMultplTimeF = false
                        screenChanges()
                    }
                }
                applyMouseChanges(mouseX)
            }
        }
    }

    function applyMouseChanges(mouseX)
    {
        if( ( (direction === 1) && (mouseXstart > mouseX) ) || ( (direction === 2) && (mouseXstart <= mouseX) ) )
        {
            direction = (mouseXstart > mouseX) ? 2 : 1

            screenChanges()
        }

        switch(direction)
        {

        case 1:
        case 2:
        {
            if( (currentScreenNo == 1) ||
                ( (direction === 1) && (currentAbsentScreen == lastAbsentScreen) ) ||
                ( (direction === 2) && (currentAbsentScreen == 1) ) )
            {
                scrren1leftMargin += (mouseX - mouseXprvs);
                leaveApplyScreen.opacity = 1
            }
            else
            {
                scrren1leftMargin = startleftMargin;
                leaveApplyScreen.opacity = lcd_width / (lcd_width + xdiff*15)
            }
        }
        break

        case 3:
        case 4:
        {
            if(currentScreenNo == 1)
            {
                if(directionChngMultplTimeF === false)
                {
                    if(presentMnthVisblF == true)
                    {
                        presentMonth.opacity = lcd_height / (lcd_height + ydiff*15)
                        previousMonth.opacity = 1 - lcd_height / (lcd_height + ydiff*2)
                    }
                    else
                    {
                        previousMonth.opacity = lcd_height / (lcd_height + ydiff*15)
                        presentMonth.opacity = 1 - lcd_height / (lcd_height + ydiff*2)
                    }
                    if(ydiff > (lcd_height / 2) )
                    {
                        directionChngMultplTimeF = true
                        if(presentMnthVisblF == true)
                        {
                            presentMnthVisblF = false
                            presentMonth.opacity = 0
                            previousMonth.opacity = 1
                        }
                        else
                        {
                            presentMnthVisblF = true
                            previousMonth.opacity = 0
                            presentMonth.opacity = 1
                        }
                    }
                }
            }
            else if(currentScreenNo == 2)
            {
                if(aplyScrndirectionChngMultplTimeF == false)
                {
                    if(direction == 4)
                    {
                        if(currentLeaveCodeNmbr == maxLeaveCode)
                        {
                            currentLeaveCodeNmbr = 1
                        }
                        else
                        {
                            currentLeaveCodeNmbr++
                        }
                        leaveApplyScreen.leaveColor = (leaveApplyScreen.leaveColor == "#d8d8d8") ? "black" : "#d8d8d8"
                    }
                    else if(direction == 3)
                    {
                        if(currentLeaveCodeNmbr == 1)
                        {
                            currentLeaveCodeNmbr = maxLeaveCode
                        }
                        else
                        {
                            currentLeaveCodeNmbr--
                        }
                        leaveApplyScreen.leaveColor = (leaveApplyScreen.leaveColor == "#d8d8d8") ? "black" : "#d8d8d8"
                    }
                    aplyScrndirectionChngMultplTimeF = true
                    if(leaveTypesNames[leaveTypes[currentLeaveCodeNmbr-1]] === ("Attendance Correction"))
                    {
                        atndncCorrectionVisibleF = true
                    }
                    else
                    {
                        atndncCorrectionVisibleF = false
                    }
                    aplyLeaveRspnsStrng = ""
                }
                leaveApplyScreen.opacity = lcd_height / (lcd_height + ydiff*15)
            }
        }
        break
      }
      mouseXprvs = mouseX;
    }

    // Submit button Area
    Item
    {
        id : submitRectMouseArea
        height 						: 40
        width 						: lcd_width - 5
        anchors.bottom				: parent.bottom
        anchors.bottomMargin		: 5
        anchors.horizontalCenter	: parent.horizontalCenter
        visible: (currentScreenNo == 1) ? false : true

        MouseArea
        {
            id                      : submitBtnTouchArea
            height                  : parent.height
            width                   : parent.width
            enabled                 : ( (currentScreenNo == 1) || (leaveApplyScreen.submitEnabled == false) ) ? false : true
            anchors.bottom          : parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            onPressed:
            {
                if(loadNextAbsntDayScrn.running == false)
                {
                    leaveApplyScreen.submitEnabled = false
                    touchArea.enabled = false
                    appData.data = EnumClass.GUI_BUTTON_APPLY_LEAVE +
                                            String.fromCharCode(uiMsgFsp).toString() +
                                            userIdx + String.fromCharCode(uiMsgFsp).toString() +
                                            absentDates[currentAbsentScreen-1] + String.fromCharCode(uiMsgFsp).toString() +
                                            leaveCodes[currentLeaveCodeNmbr-1] + String.fromCharCode(uiMsgFsp).toString() +
                                            String.fromCharCode(eotFsp).toString()
                }
            }
        }
    }

    states: State {
        name:"screen1onRight"
        AnchorChanges {
            target: leaveApplyScreen
            anchors.left: parent.left
        }
        PropertyChanges {
            target: leaveApplyScreen
            anchors.leftMargin: scrren1leftMargin
        }
        AnchorChanges {
            target: mainRect
            anchors.left: leaveApplyScreen.right
        }
        PropertyChanges {
            target: mainRect
            anchors.leftMargin: 0
        }
    }

    State {
        name:"screen1onLeft"
        AnchorChanges {
            target: mainRect
            anchors.left: parent.left
        }
        PropertyChanges {
            target: mainRect
            anchors.leftMargin: scrren1leftMargin
        }
        AnchorChanges {
            target: leaveApplyScreen
            anchors.left: mainRect.right
       }
        PropertyChanges {
            target: leaveApplyScreen
            anchors.leftMargin: 0
        }
    }

    // This timer is use for = If apply leave response success, then load success screen for 1 sec and after load next absent date
    Timer
    {
        id      :loadNextAbsntDayScrn
        interval:1000
        repeat  : false

        onTriggered:
        {
            if(loadNextAbsntDayScrn.running)
            {
                loadNextAbsntDayScrn.stop()
            }

            if( ( (nxtScrnIncrCount == true) && (currentAbsentScreen == lastAbsentScreen) ) ||
                ( (nxtScrnIncrCount == false) && (currentAbsentScreen == 1) ) )                //Load calender screen
            {
                currentAbsentScreen = 0
                currentLeaveCodeNmbr = 0
                leaveApplyScreen.submitEnabled = true
                leaveApplyScreen.screen2VisibleF = 1
                currentScreenNo = 1
                window.state = "scrren1leftMargin"
                scrren1leftMargin = 0
                startleftMargin = scrren1leftMargin;
            }
            else
            {
                leaveApplyScreen.opacity = 1
                currentAbsentScreen = ( currentAbsentScreen + ( (nxtScrnIncrCount == true) ? +1 : -1) )
                currentLeaveCodeNmbr = 1
                leaveApplyScreen.screen2VisibleF = 1
                leaveApplyScreen.submitEnabled = true
                leaveApplyScreen.absntDateColor = (leaveApplyScreen.absntDateColor == "#d8d8d8") ? "black" : "#d8d8d8"
                leaveApplyScreen.leaveColor = (leaveApplyScreen.leaveColor == "#d8d8d8") ? "black" : "#d8d8d8"
            }
        }
    }

    function processDeviceResponse (msgId,rcvData)
    {
        switch(msgId)
        {
            case EnumClass.GUI_MSG_SHOW_USER_ATTENDANCE_DETAIL:
            {
                var tmpPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

                userIdx                             = tmpPayload[0]
                userIdForPhoto                      = tmpPayload[1]
                userNameText.text                   = tmpPayload[2]
                userIdText.text                     = tmpPayload[3]
                userInfoId.loadUserPhoto()
                lastUpdatedDate                     = parseInt(tmpPayload[7])
                lastUpdatedMonth                    = parseInt(tmpPayload[8])
                lastUpdatedYear                     = parseInt(tmpPayload[9])
                lastUpdatedHours                    = parseInt(tmpPayload[10])
                lastUpdatedMinutes                  = parseInt(tmpPayload[11])

                presentMonth.currentMonth           = parseInt(tmpPayload[4])
                presentMonth.currentYear            = parseInt(tmpPayload[5])
                presentMonth.eventType              = tmpPayload[6].split(',')
                presentMonth.showCalender()
                presentMonth.separatLateInEarlyOutF = (parseInt(tmpPayload[12],10) === 0) ? false : true
                presentMonth.monthWorkHrs           = tmpPayload[13]
                presentMonth.monthOvertime          = tmpPayload[14]
                presentMonth.monthLateIn            = tmpPayload[15]
                presentMonth.monthEarlyOut          = tmpPayload[16]

                previousMonth.currentMonth          = parseInt(tmpPayload[17])
                previousMonth.currentYear           = parseInt(tmpPayload[18])
                previousMonth.eventType             = tmpPayload[19].split(',')
                previousMonth.showCalender()
                previousMonth.separatLateInEarlyOutF= (parseInt(tmpPayload[20],10) === 0) ? false : true
                previousMonth.monthWorkHrs          = tmpPayload[21]
                previousMonth.monthOvertime         = tmpPayload[22]
                previousMonth.monthLateIn           = tmpPayload[23]
                previousMonth.monthEarlyOut         = tmpPayload[24]

                leaveCodes      = tmpPayload[25].split(',')
                balance         = tmpPayload[26].split(',')
                leaveTypes      = tmpPayload[27].split(',')
                absentDates     = tmpPayload[28].split('.') //it separate by '.' b'coz string itself contains ','
                firstHalf       = tmpPayload[29].split(',')
                secondHalf      = tmpPayload[30].split(',')

                lastAbsentScreen    = absentDates.length
                maxLeaveCode        = leaveCodes.length
                refreshInPrgrsF     = false

                if(absentDates[0] === "")
                {
                    leaveApplyScreen.screen2VisibleF = 2
                    leaveApplyScreen.submitEnabled = false
                    lastAbsentScreen = 1
                }
                break
            }

            case EnumClass.GUI_MSG_APPLY_LEAVE_RESPONSE:
            {
                leaveApplyScreen.submitEnabled = true
                touchArea.enabled = true

                var tmpLeaveAplyPayload = rcvData.split(String.fromCharCode (uiMsgFsp).toString())

                aplyLeaveRspns = parseInt(tmpLeaveAplyPayload[0],10)
                if(aplyLeaveRspns == 1) // success
                {
                    leaveApplyScreen.screen2VisibleF = 3
                    leaveApplyScreen.submitEnabled = false
                    loadNextAbsntDayScrn.start()
                }
                else if(aplyLeaveRspns == 0) // fail
                {
                    aplyLeaveRspnsStrng = tmpLeaveAplyPayload[1]
                }
                break
            }
        }
    }
}
