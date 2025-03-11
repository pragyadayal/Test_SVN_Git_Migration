import QtQuick 1.1
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Rectangle
{
    id : digitalRect
    height : 100
    width  : 240
    color  : "transparent"

    property  int spcBetText : -5

    Column
    {
        anchors.centerIn: parent
        spacing : -5
        Rectangle
        {
            id:  timeRect
            height: time.height
            width : timeFormat ? time.width + ampmText.width + spcBetText: time.width
            anchors.horizontalCenter: parent.horizontalCenter
            color : "transparent"

            MxText
            {
                id                     : time
                color                  : "white"
                anchors.bottom         : parent.bottom
                smooth                 : true
                font.bold              : true
                font.pointSize         : 32
                text                   : timeFormat ? Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                                             hours,minutes,seconds),"hh:mm AP").toString().slice(0, -2) :

                                                      Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                                             hours,minutes,seconds), "hh:mm" )
                                                                             wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.family            : clockFontFamily
                style                  : Text.Outline
                styleColor             : "black"
            }

            MxText
            {
                id                   : ampmText
                color                : "white"
                smooth               : true
                font.bold            : true
                font.pointSize       : 18
                anchors.left         : time.right
                anchors.leftMargin   : spcBetText
                anchors.bottom       : parent.bottom
                anchors.bottomMargin : (ampmText.height/3)
                visible              : timeFormat ? true : false
                text                 : Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                              hours,minutes,seconds),"AP")
                font.family          : clockFontFamily
                style                : Text.Outline
                styleColor           : "black"
            }
        }

        MxText
        {
            id                     : day
            color                  : "white"
            anchors.horizontalCenter: parent.horizontalCenter
            smooth                 : true
            font.bold              : true
            font.pointSize         : 18
            text                   : dispStr.timeDispStr(currentYearMultiL,currentMonthMultiL,currentDateMultiL,currentDayMultiL,hours,minutes,seconds,DispStr.DAY_DD_MMM)
            font.family            : clockFontFamily
            style                  : Text.Outline
            styleColor             : "black"
        }
    }
}
