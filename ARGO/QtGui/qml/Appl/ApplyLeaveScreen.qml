import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item {
    id          : leaveApplyScreen
    height      : lcd_height
    width       : lcd_width
    anchors.top : mainRect.top
    anchors.left: mainRect.right

    property int screen2VisibleF 	: 1
    property bool submitEnabled 	: true
    property string leaveColor		: "#d8d8d8"
    property string absntDateColor	: "#d8d8d8"

    // NO Leave
    Item
    {
        id                  : noAbsentDayScreen
        height              : lcd_height
        width               : lcd_width
        visible             : (screen2VisibleF == 2) ? true : false
        anchors.topMargin   : 30

        Column
        {
            anchors.centerIn    : parent
            spacing             : 10

            Image
            {
                id: noABDaysImage
                source: imageLocation + "/AttendanceSummary/NoABDays.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            MxText
            {
                text            	: dispStr.readDispStr(DispStr.MSG_YOU_RE_ALL_GOOD_923) //"You're All Good!"
                color           	: "#9d9d9d"
                font.pointSize  	: 14
                width           	: parent.width
                horizontalAlignment	:  Text.AlignHCenter
            }

            MxText
            {
                text            : dispStr.readDispStr(DispStr.MSG_NO_ABSENT_DAYS_TO_APPLY_FOR_924) //"No Absent days to apply for"
                color           : "#767676"
                font.pointSize  : 12
            }
        }
    }

    // Success Screen
    Item
    {
        id                  : successScreen
        height              : lcd_height
        width               : lcd_width
        visible             : (screen2VisibleF == 3) ? true : false
        anchors.topMargin   : 30

        Column
        {
            anchors.centerIn    : parent
            spacing             : 10

            Image
            {
                id                      : successImage
                source                  : imageLocation + "/AttendanceSummary/LeaveApplicationSuccess.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            MxText
            {
                text            : dispStr.readDispStr(DispStr.MSG_SUCCESS_594) //"Success!"
                color           : "#c9c9c9"
                font.pointSize  : 14
            }
        }
    }

    // Leave Details
    Item
    {
        id                  : leaveScreen
        height              : lcd_height
        width               : lcd_width
        visible             : (screen2VisibleF == 1) ? true : false
        anchors.top         : parent.top
        anchors.topMargin   : 25

        // Header Info
        Rectangle
        {
            id 			: headerRect
            height 		: 30
            width 		: lcd_width
            color		: "transparent"
            anchors.top	: parent.top

            Rectangle
            {
                height 		: 30
                width 		: lcd_width
                color		: "#000000"
                opacity		: 0.4
                anchors.top	: parent.top

            }
            Row
            {
                height 					: 30
                anchors.verticalCenter	: parent.verticalCenter
                spacing					: 5
                anchors.left			: parent.left
                anchors.leftMargin		: 5

                MxText
                {
                    text            		: dispStr.readDispStr(DispStr.MSG_APPLICATION_920)+" : "   //"Application : "
                    color           		: "#919191"
                    anchors.verticalCenter	: parent.verticalCenter
                    font.pointSize  		: 13
                    verticalAlignment		: Text.AlignVCenter
                }
                MxText
                {
                    text            		: (currentLeaveCodeNmbr == 0) ? "" : leaveTypesNames[leaveTypes[currentLeaveCodeNmbr-1]]
                    color           		: "#919191"
                    anchors.verticalCenter	: parent.verticalCenter
                    font.pointSize  		: 13
                    verticalAlignment		: Text.AlignVCenter
                }
                MxText
                {

                    text                    : ( (currentLeaveCodeNmbr == 0 ) || (atndncCorrectionVisibleF == true) ) ? "" : (" - " + leaveCodes[currentLeaveCodeNmbr-1])
                    color           		: "#919191"
                    anchors.verticalCenter	: parent.verticalCenter
                    font.pointSize  		: 13
                    verticalAlignment		: Text.AlignVCenter
                }
            }
       }

        // ^ UP
        Rectangle
        {
            id 							: upDirRect
            anchors.top					: headerRect.bottom
            anchors.topMargin			: 15
            anchors.horizontalCenter	: parent.horizontalCenter

            Image
            {
                id					: upDirImg
                anchors.centerIn	: parent
                source				: imageLocation + "/AttendanceSummary/SwipeUp.png"
            }
        }

        // DATE + first half and second half
        Rectangle
        {
            id					: dateRect
            height 				: lcd_height/5
            width 				: lcd_width-80
            anchors.top			: upDirRect.bottom
            anchors.left		: parent.left
            anchors.leftMargin	: 40
            anchors.topMargin	: 15
            color       		: "transparent"

            Rectangle
            {
                color   		: "#000000"
                opacity 		: 0.4
                anchors.fill	: parent
            }

            Column
            {
                spacing             : 10
                anchors.centerIn	: parent
                width				: parent.width

                MxText
                {
                    text            			: (currentAbsentScreen == 0) ? "" : absentDates[currentAbsentScreen-1]
                    color           			: absntDateColor
                    anchors.horizontalCenter	: parent.horizontalCenter
                    font.pointSize  			: 12
                    font.bold					: true

                    Behavior on color {
                        SequentialAnimation {
                            loops: 2
                            ColorAnimation { from: "#d8d8d8"; to: "black"; duration: 150 }
                            ColorAnimation { from: "black"; to: "#d8d8d8"; duration: 150 }

                        }
                    }
                }

                Row
                {
                    spacing             		: 21
                    anchors.horizontalCenter	: parent.horizontalCenter

                    MxText
                    {
                        text            : (currentAbsentScreen == 0) ? "" : firstHalf[currentAbsentScreen-1]
                        color           : (firstHalf[currentAbsentScreen-1] === "PR") ? ("#d8d8d8"):("#ff2121")
                        font.pointSize  : 12
                        font.bold		: true
                    }

                    Rectangle
                    {
                        id 		: separateLine
                        width	: 1
                        height	: 20
                        color	: "#444444"

                    }

                    MxText
                    {
                        text            : (currentAbsentScreen == 0) ? "" : secondHalf[currentAbsentScreen-1]
                        color           : (secondHalf[currentAbsentScreen-1] === "PR") ? ("#d8d8d8"):("#ff2121")
                        font.pointSize  : 12
                        font.bold		: true
                    }
                }
            }
        }

        // Leave Code and info
        Rectangle
        {
            id                  : applyCodeRect
            height              : (atndncCorrectionVisibleF == true) ? 50 : lcd_height/5
            width               : lcd_width-80
            anchors.top         : dateRect.bottom
            anchors.left        : parent.left
            anchors.leftMargin  : 40
            color               : "transparent"
            visible             : true

            Rectangle
            {
                color   		: "#000000"
                opacity 		: 0.4
                anchors.fill	: parent
            }

            Column
            {
                spacing             : 1
                anchors.centerIn	: parent
                width				: parent.width

                Row
				{
                    height 						: 20
                    spacing						: 5
                    anchors.horizontalCenter	: parent.horizontalCenter

                    MxText
                    {
                        text            	: (currentLeaveCodeNmbr == 0) ? "" : leaveTypesNames[leaveTypes[currentLeaveCodeNmbr-1]]
                        color           	: leaveColor
                        font.pointSize  	: 12
                        horizontalAlignment	: Text.AlignHCenter
                        height          	: parent.height
                        font.bold			: true

                        Behavior on color {
                            SequentialAnimation {
                                loops: 2
                                ColorAnimation { from: "#d8d8d8"; to: "black"; duration: 150 }
                                ColorAnimation { from: "black"; to: "#d8d8d8"; duration: 150 }
                            }
                        }
                    }

                    MxText
                    {
                        text                : ( (currentLeaveCodeNmbr == 0 ) || (atndncCorrectionVisibleF == true) ) ? "" : (" - " + leaveCodes[currentLeaveCodeNmbr-1])
                        color           	: leaveColor
                        font.pointSize  	: 12
                        font.bold			: true
                        horizontalAlignment	: Text.AlignHCenter
                        height          	: parent.height

                        Behavior on color {
                            SequentialAnimation {
                                loops: 2
                                ColorAnimation { from: "#d8d8d8"; to: "black"; duration: 150 }
                                ColorAnimation { from: "black"; to: "#d8d8d8"; duration: 150 }
                            }
                        }
                    }
                }
                Column
                {
                    spacing     : 3
                    width       : parent.width
                    visible     : (!atndncCorrectionVisibleF)

                    MxText
                    {
                        text            			: (currentLeaveCodeNmbr == 0) ? "" : balance[currentLeaveCodeNmbr-1]
                        color 			          	: leaveColor
                        font.pointSize  			: 12
                        font.bold					: true
                        anchors.horizontalCenter	: parent.horizontalCenter

                        Behavior on color {
                            SequentialAnimation {
                                loops: 2
                                ColorAnimation { from: "#d8d8d8"; to: "black"; duration: 150 }
                                ColorAnimation { from: "black"; to: "#d8d8d8"; duration: 150 }
                            }
                        }
                    }

                    MxText
                    {
                        text            			: dispStr.readDispStr(DispStr.MSG_LEAVE_BALANCE_921) //"Leave Balance"
                        color           			: "#828282"
                        font.pointSize  			: 11
                        anchors.horizontalCenter	: parent.horizontalCenter
                        font.bold					: true
                    }
                }
            }
        }

        // DOWN Arrow
        Rectangle
        {
            id 							: downDirRect
            anchors.top					: applyCodeRect.bottom
            anchors.topMargin			: 15
            anchors.horizontalCenter	: parent.horizontalCenter

            Image
            {
                id					: downDirImg
                anchors.centerIn    : parent
                source				: imageLocation + "/AttendanceSummary/SwipeDown.png"
            }
        }

        // Validation Msg
        Rectangle
        {
            id 							: msgRect
            height 						: 20
            width 						: lcd_width
            color						: "transparent"
            anchors.bottom				: submitRect.top
            anchors.bottomMargin		: 15
            anchors.horizontalCenter	: parent.horizontalCenter

            Text
            {
                width               : parent.width
                text                : aplyLeaveRspnsStrng
                color               : "#f44336"
                font.pointSize      : 11
                horizontalAlignment : Text.AlignHCenter
                wrapMode            : Text.WordWrap
            }
        }

        //Submit Button
        Rectangle
        {
            id 							: submitRect
            height 						: 40
            width 						: lcd_width - 5
            anchors.bottom				: parent.bottom
            anchors.bottomMargin		: 30
            anchors.horizontalCenter	: parent.horizontalCenter
            color				       	: "transparent"

            Rectangle
            {
                color			: "#000000"
                opacity			: 0.7
                anchors.fill	: parent
            }

            Text
            {
                id 					: submitBtn
                text 				: (submitEnabled == true) ? (dispStr.readDispStr(DispStr.MSG_SUBMIT_922)) : (dispStr.readDispStr(DispStr.MSG_SUBMITTING_925)) //"SUBMIT" or "Submitting..."
                color				: (submitEnabled == true) ? ("#9eff36") : ("#686868")
                font.pointSize  	: 14
                anchors.centerIn	: parent
            }
        }

        Rectangle
        {
            id 					: topBorderLine
            height				: 1
            width				: lcd_width-80
            anchors.left		: parent.left
            anchors.leftMargin	: 40
            anchors.bottom		: dateRect.top
            color				: "#444444"
        }
        Rectangle
        {
            id 					: leftBorderLine
            height				: (atndncCorrectionVisibleF == true) ? ((lcd_height/5) + 50) : ((lcd_height/5) + (lcd_height/5))
            width				: 1
            anchors.right		: dateRect.left
            color				: "#444444"
            anchors.top			: dateRect.top
        }
        Rectangle
        {
            id 					: bottomBorderLine
            height				: 1
            width				: lcd_width-80
            color				: "#101010"
            anchors.left		: parent.left
            anchors.leftMargin	: 40
            anchors.top			: applyCodeRect.bottom
        }
        Rectangle
        {
            id 					: rightBorderLine
            height				: (atndncCorrectionVisibleF == true) ? ((lcd_height/5) + 50) : ((lcd_height/5) + (lcd_height/5))
            width				: 1
            anchors.left		: dateRect.right
            color				: "#101010"
            anchors.top			: dateRect.top
        }

        Rectangle
        {
            id 					: centerBorderLine
            height				: 1
            width				: lcd_width-80
            color				: "#444444"
            anchors.left		: parent.left
            anchors.leftMargin	: 40
            anchors.top			: upDirRect.bottom
            anchors.topMargin	: (15+(lcd_height/5))
        }
    }
}
