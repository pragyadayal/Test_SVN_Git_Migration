// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    width: 350
    height: 350
    id : calMainRect
    property int    currentMonth      :  00
    property int    currentYear       :  0000
    property int    textFontSize      :  calMainRect.width / 14
    property variant    eventType     : [1]

    ListModel
    {
        id:calenderCellModel
    }

    Component
    {
        id          : caleDelegate

        CalenderCell
        {
            id          : infoElementId
            cellText    : CellText
            cellColor   : CellColor
            cellHeight  : CellHeight
            cellwidth   : Cellwidth
            cellFontPointSize : CellFontPointSize
        }
    }

    GridView
    {
        id              : calenderGrid
        width           : calMainRect.width       //total two element in row
        height          : calMainRect.height
        cellWidth       : calMainRect.width / 7   //width of each cell
        cellHeight      : calMainRect.height / 7  //height of each cell
        model           : calenderCellModel
        delegate        : caleDelegate
        snapMode        : GridView.SnapToRow
        boundsBehavior  : Flickable.StopAtBounds
    }

    function isLeapYear(year)
    {
        if (year % 4)
            return false;
        if (!(year % 400))
            return true;
        if (!(year % 100))
            return false;
        return true;
    }

    // 'month' is 0-based.
    function maxDayOfMonth(year, month)
    {
        var daysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

        if (month != 1) // February?
            return daysInMonth[month];
        return isLeapYear(year) ? 29 : 28;
    }

    function showCalender()
    {
        var days =["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"]
        var eventColr = ["#4f4f4f", "#ffffff", "#ff2121","#8f8f8f","#9eff36"]

        var maxDayInMonthCnt = maxDayOfMonth(currentYear,currentMonth)
        var dateTmp = new Date(currentYear,currentMonth,1,0,0,0)
        var startIdx = parseInt(days.indexOf(dateTmp.toDateString().slice(0,2) ) )
        calenderCellModel.clear()
        for(var index = 0; (index < 7); index++)
        {
            calenderCellModel.insert (index,
                               {
                                    "CellText"    : days[index],
                                    "CellColor"   : "#696969",
                                    "CellHeight"  : calMainRect.height / 7,
                                    "Cellwidth"   : calMainRect.width / 7,
                                    "CellFontPointSize" : textFontSize,
                               })
        }

        for(; (index < startIdx+7); index++)
        {
            calenderCellModel.insert (index,
                               {
                                    "CellText"    : "",
                                    "CellColor"   : "black",
                                    "CellHeight"  : calMainRect.height / 7,
                                    "Cellwidth"   : calMainRect.width / 7,
                                    "CellFontPointSize" : textFontSize,

                               })
        }
        var dateIdx = 1
        for(var x=0; (dateIdx <= maxDayInMonthCnt ); dateIdx++,index++,x++)
        {
            calenderCellModel.insert (index,
                               {
                                    "CellText"    : ( (dateIdx < 10) ? "0" : "" ) +dateIdx,
                                    "CellColor"   : eventColr[eventType[x] ],
                                    "CellHeight"  : calMainRect.height / 7,
                                    "Cellwidth"   : calMainRect.width / 7,
                                    "CellFontPointSize" : textFontSize,

                               })
        }

        // For clear cell data
        for(; (index < 49); index++)
        {
            calenderCellModel.insert (index,
                               {
                                    "CellText"    : "",
                                    "CellColor"   : "black",
                                    "CellHeight"  : calMainRect.height / 7,
                                    "Cellwidth"   : calMainRect.width / 7,
                                    "CellFontPointSize" : textFontSize,

                               })
        }
    }
}
