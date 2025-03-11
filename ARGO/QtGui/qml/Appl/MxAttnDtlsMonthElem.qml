import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    property int monthHeaderHeight          : 20
    property int monthHeaderFontSize        : 12
    property int monthCalenderBoxHeight     : 120
    property int monthCalenderFontSize      : 11
    property int monthlyDetailsBoxHeight    : 17
    property int monthlyDetailsFontSize     : 12
    property int monthYearFontSizeVal       : 14

    property bool separatLateInEarlyOutF    : false
    property bool presentMonthF             : false

    property string monthWorkHrs            : ""
    property string monthOvertime           : ""
    property string monthLateIn             : ""
    property string monthEarlyOut           : ""
    property string monthColor              : "#58afeb"
    property string monthColor1             : "#58afeb"


    property alias currentMonth             : monthCalender.currentMonth
    property alias currentYear              : monthCalender.currentYear
    property alias eventType                : monthCalender.eventType

    function showCalender()
    {
        monthCalender.showCalender()
    }

    // Month Header
    Rectangle
    {
        id          : monthYearInfoRect
        height      : monthHeaderHeight
        width       : lcd_width
        anchors.top : parent.top
        color       : "transparent"

        Rectangle
        {
            color   : "#000000"
            opacity : 0.4
            width   : lcd_width
            height  : monthHeaderHeight

        }
        MxText
        {
            id                  : monthYearText
            text                : dispStr.timeDispStr(monthCalender.currentYear,monthCalender.currentMonth,1,0,0,0,0,DispStr.MMM_YYYY);
            height              : monthHeaderHeight
            anchors.right       : parent.right
            anchors.rightMargin : 11
            verticalAlignment   : Text.AlignVCenter
            color               : monthColor
            font.pointSize      : monthYearFontSizeVal
            font.capitalization : Font.AllUppercase

            Behavior on color {
                SequentialAnimation {
                    loops: 2
                    ColorAnimation { from: monthColor1; to: "black"; duration: 150 }
                    ColorAnimation { from: "black"; to: monthColor1; duration: 150 }
                }
            }
        }
    }

    // Calender
    Rectangle
    {
        id              : monthCalenderRect
        anchors.top     : monthYearInfoRect.bottom
        color           : "black"
        opacity         : 0.6
        height          : monthCalenderBoxHeight
        width           : lcd_width
        border.color    : "#444444"
        border.width    : 1

    }

    Calender
    {
        id              : monthCalender
        anchors.top     : monthYearInfoRect.bottom
        width           : (lcd_width - (lcd_width % 7) + 1)
        height          : monthCalenderBoxHeight
        textFontSize    : monthCalenderFontSize
    }

    // Work Hours + Overtime Label
    Rectangle
    {
        id                  : monthWrkHrsAndOvrtmLabels
        height              : monthlyDetailsBoxHeight
        width               : lcd_width
        anchors.top         : monthCalenderRect.bottom
        anchors.topMargin   : 1
        color               : "transparent"

        Row
        {
            anchors.verticalCenter  : parent.verticalCenter
            anchors.left            : parent.left

            Rectangle
            {
                color   : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                      : monthWrkHrLabel
                    anchors.verticalCenter  : parent.verticalCenter
                    width                   : (lcd_width/2)-1
                    text                    : dispStr.readDispStr(DispStr.MSG_WORK_HOURS_918) //"Work Hours"
                    font.pointSize          : monthlyDetailsFontSize
                    color                   : "#696969"
                    horizontalAlignment     : Text.AlignHCenter
                    font.bold               : true
                }
            }

            Rectangle
            {
                color   : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                     : monthOvrtimeLbl
                    anchors.verticalCenter : parent.verticalCenter
                    width                  : (lcd_width/2)-1
                    text                   : dispStr.readDispStr(DispStr.MSG_OVERTIME_770) //Overtime
                    font.pointSize         : monthlyDetailsFontSize
                    color                  : "#696969"
                    horizontalAlignment    : Text.AlignHCenter
                    font.bold              : true
                }
            }
        }
    }

    // Work Hours + Overtime val
    Rectangle
    {
        id          : monthWrkHrsAndOvrtmValue
        height      : monthlyDetailsBoxHeight
        width       : lcd_width
        anchors.top : monthWrkHrsAndOvrtmLabels.bottom
        color       : "transparent"

        Row
        {
            anchors.left : parent.left

            Rectangle
            {
                color   : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                  : monthWrkHrVal
                    width               : (lcd_width/2)-1
                    text                : monthWorkHrs
                    font.pointSize      : monthlyDetailsFontSize
                    color               : "#ffffff"
                    horizontalAlignment : Text.AlignHCenter
                    font.bold           : true
                }
            }

            Rectangle
            {
                color   : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                     : monthOvrtimeVal
                    width                  : (lcd_width/2)-1
                    text                   : monthOvertime
                    font.pointSize         : monthlyDetailsFontSize
                    color                  : "#ffffff"
                    horizontalAlignment    : Text.AlignHCenter
                    font.bold              : true
                }
            }
        }
    }

    // separate Vertical Line
    Rectangle
    {
        color                       : "#444444"
        opacity                     : 0.6
        width                       : 1
        height                      : 25
        anchors.horizontalCenter    : monthWrkHrsAndOvrtmLabels.horizontalCenter
        anchors.top                 : monthWrkHrsAndOvrtmLabels.top
        anchors.topMargin           : 5
    }

    // "Late IN / Early OUT" label
    Rectangle
    {
        id                  : monthLateInEarlyOutLbl
        height              : monthlyDetailsBoxHeight
        width               : lcd_width
        anchors.top         : monthWrkHrsAndOvrtmValue.bottom
        anchors.topMargin   : 1
        color               : "transparent"
        visible             : (!separatLateInEarlyOutF)

        Rectangle
        {
            color   : "#000000"
            opacity : 0.4
            width   : lcd_width
            height  : monthlyDetailsBoxHeight

        }
        MxText
        {
            anchors.verticalCenter  : parent.verticalCenter
            anchors.left            : parent.left
            width                   : parent.width
            text                    : dispStr.readDispStr(DispStr.MSG_LATE_IN_EARLY_OUT_919)  //"Late IN / Early OUT"
            font.pointSize          : monthlyDetailsFontSize
            color                   : "#696969"
            horizontalAlignment     : Text.AlignHCenter
            font.bold               : true
        }
    }

    // "Late IN / Early OUT" Value
    Rectangle
    {
        id          : monthLateInEarlyOutVal
        height      : monthlyDetailsBoxHeight
        width       : lcd_width
        anchors.top : monthLateInEarlyOutLbl.bottom
        color       : "transparent"
        visible     : (!separatLateInEarlyOutF)

        Rectangle
        {
            color   : "#000000"
            opacity : 0.4
            width   : (lcd_width/2)
            height  : monthlyDetailsBoxHeight
            anchors.fill: parent

        }
        MxText
        {
            width               : lcd_width
            text                : monthLateIn
            font.pointSize      : monthlyDetailsFontSize
            color               : "#ffffff"
            horizontalAlignment : Text.AlignHCenter
            font.bold           : true
        }
    }

    // Late IN and Early OUT label
    Rectangle
    {
        id                  : monthLateInEarlyOutSepLbl
        height              : monthlyDetailsBoxHeight
        width               : lcd_width
        anchors.top         : monthWrkHrsAndOvrtmValue.bottom
        anchors.topMargin   : 1
        color               : "transparent"
        visible             : separatLateInEarlyOutF

        Row
        {
            anchors.verticalCenter  : parent.verticalCenter
            anchors.left            : parent.left

            Rectangle
            {
                color       : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                      : monthLateInLabel
                    anchors.verticalCenter  : parent.verticalCenter
                    width                   : (lcd_width/2)-1
                    text                    : dispStr.readDispStr(DispStr.MSG_LATE_IN_314)  // HRD Late IN
                    font.pointSize          : monthlyDetailsFontSize
                    color                   : "#696969"
                    horizontalAlignment     : Text.AlignHCenter
                    font.bold               : true
                }
            }

            Rectangle
            {
                color       : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                     : monthEarlyOutLbl
                    anchors.verticalCenter : parent.verticalCenter
                    width                  : (lcd_width/2)-1
                    text                   : dispStr.readDispStr(DispStr.MSG_EARLY_OUT_180) // Early OUT
                    font.pointSize         : monthlyDetailsFontSize
                    color                  : "#696969"
                    horizontalAlignment    : Text.AlignHCenter
                    font.bold              : true
                }
            }
        }
    }

    // Late IN and Early OUT value
    Rectangle
    {
        id          : monthLateInEarlyOutSepValue
        height      : monthlyDetailsBoxHeight
        width       : lcd_width
        anchors.top : monthLateInEarlyOutSepLbl.bottom
        color       : "transparent"
        visible     : separatLateInEarlyOutF

        Row
        {
            anchors.left : parent.left

            Rectangle
            {
                color       : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                  : monthLateInVal
                    width               : (lcd_width/2)-1
                    text                : monthLateIn
                    font.pointSize      : monthlyDetailsFontSize
                    color               : "#ffffff"
                    horizontalAlignment : Text.AlignHCenter
                    font.bold           : true
                }
            }

            Rectangle
            {
                color   : "transparent"
                width   : (lcd_width/2)
                height  : monthlyDetailsBoxHeight

                Rectangle
                {
                    color   : "#000000"
                    opacity : 0.4
                    width   : (lcd_width/2)
                    height  : monthlyDetailsBoxHeight

                }
                MxText
                {
                    id                     : monthEarlyOutVal
                    width                  : (lcd_width/2)-1
                    text                   : monthEarlyOut
                    font.pointSize         : monthlyDetailsFontSize
                    color                  : "#ffffff"
                    horizontalAlignment    : Text.AlignHCenter
                    font.bold              : true
                }
            }
        }
    }

    // Separate Vertical Line
    Rectangle
    {
        color                       : "#444444"
        opacity                     : 0.6
        width                       : 1
        height                      : 25
        anchors.horizontalCenter    : monthLateInEarlyOutSepLbl.horizontalCenter
        anchors.top                 : monthLateInEarlyOutSepLbl.top
        anchors.topMargin           : 5
        visible                     : separatLateInEarlyOutF
    }

    Rectangle
    {
        id 					: topBorderLineForUpdteTimeBar
        height				: 2
        width				: lcd_width
        anchors.left		: parent.left
        anchors.top         : monthLateInEarlyOutSepValue.bottom
        color				: "#484848"
    }
    Rectangle
    {
        id 					: bottomBorderLineForUpdteTimeBar
        height				: 2
        width				: lcd_width
        anchors.left		: parent.left
        anchors.top         : monthLateInEarlyOutSepValue.bottom
        anchors.topMargin   : 27
        color				: "#484848"
    }
    Rectangle
    {
        id 					: leftBorderLineForUpdteTimeBar
        height				: 27
        width				: 2
        anchors.left		: monthLateInEarlyOutSepValue.left
        anchors.top         : monthLateInEarlyOutSepValue.bottom
        color				: "#484848"
    }
    Rectangle
    {
        id 					: rightBorderLineForUpdteTimeBar
        height				: 27
        width				: 2
        anchors.right		: monthLateInEarlyOutSepValue.right
        anchors.top         : monthLateInEarlyOutSepValue.bottom
        color				: "#484848"
    }

}
