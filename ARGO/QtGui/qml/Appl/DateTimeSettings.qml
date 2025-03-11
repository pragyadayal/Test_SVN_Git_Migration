import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: dateTimeSettingsScreen
    height : lcd_height
    width  : lcd_width

    property alias year : yearSpinBox.value
    property alias month: monthSpinBox.value
    property alias day  : daySpinBox.value
    property alias hour : hourSpinBox.value
    property alias minute: minuteSpinBox.value
    property alias second: secondSpinBox.value
    property alias dayValidation :daySpinBox.topValidation
    property bool dateTimeEnableF                   // Date --> true
                                                    // Time --> false

    OpacityRectangle
    {

    }

    function isLeapYear(y)
    {
        return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
    }

    function daysInMonth(y, m)
    {
        var yearInt
        var monthInt

        yearInt = parseInt(y,10)
        monthInt = parseInt(m,10)

        if (monthInt == 2)
        {
            return isLeapYear(yearInt) ? "29" : "28";
        }
        else if (monthInt == 4 || monthInt == 6 || monthInt == 9 || monthInt == 11)
        {
            return "30";
        }
        else
        {
            return "31";
        }
    }

    MxRectangleFrame
    {
        id : popUpRect
        height: 220         //general height-width for popups
        width : 220
        anchors.centerIn: parent

        MxText
        {
            id: settings_Text
            text : dateTimeEnableF ? dispStr.readDispStr(DispStr.MSG_SET_DATE_538) /*"Set Date"*/ : dispStr.readDispStr(DispStr.MSG_SET_TIME_543)/*"Set Time"*/
            color: "white"
            font.pointSize: fontSizePopUpLabel
            anchors.top: parent.top
            anchors.topMargin : 10
            anchors.left: parent.left
            anchors.leftMargin: 25
            font.bold: true
        }

        Item
        {
            id : spinBoxIndicateRect
            anchors.horizontalCenter : parent.horizontalCenter
            anchors.bottom : dateRow.top
            anchors.bottomMargin: 10
            width : 210

            Item
            {
                id : element1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: element2.left
                anchors.rightMargin: daySpinBox.width + dateRow.spacing
                MxText
                {
                   text : dateTimeEnableF ? dispStr.readDispStr(DispStr.MSG_DD_115)/*"DD"*/ : dispStr.readDispStr(DispStr.MSG_HH_264)/*"HH"*/
                   color: mxColorGrey
                   font.pointSize: fontSizeNormal
                   anchors.centerIn: parent
                }
            }

            Item
            {
                id : element2
                anchors.centerIn: parent
                MxText
                {
                   text : dispStr.readDispStr(DispStr.MSG_MM_348) /*MM*/
                   color: mxColorGrey
                   font.pointSize: fontSizeNormal
                   anchors.centerIn: parent
                }
            }

            Item
            {
                id : element3
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: element2.right
                anchors.leftMargin: daySpinBox.width + dateRow.spacing
                MxText
                {
                   text : dateTimeEnableF ?  dispStr.readDispStr(DispStr.MSG_YYYY_673) /*"YYYY"*/ :dispStr.readDispStr(DispStr.MSG_SS_577) /*"SS"*/
                   color: mxColorGrey
                   font.pointSize: fontSizeNormal
                   anchors.centerIn: parent
                }
            }
        }

        Row
        {
            id : dateRow
            spacing : 5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: settings_Text.top
            anchors.topMargin: 45
            visible : dateTimeEnableF
            SpinBox
            {
                id: daySpinBox
                bottomValidation    : 1
                topValidation       : 31
                noOfDigit           : 2
            }
            SpinBox
            {
                id: monthSpinBox
                bottomValidation    : 1
                topValidation       : 12
                noOfDigit           : 2
                onSpinBoxClicked:
                {
                    updateDayValidation()
                }
            }
            SpinBox
            {
                id: yearSpinBox
                bottomValidation    : 1
                topValidation       : 2100
                noOfDigit           : 4
                onSpinBoxClicked:
                {
                    updateDayValidation()
                }
            }
        }

        Row
        {
            id : timeRow
            spacing : 5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: settings_Text.top
            anchors.topMargin: 45
            visible : (!dateTimeEnableF)
            SpinBox
            {
                id: hourSpinBox
                bottomValidation    : 0
                topValidation       : 23
                noOfDigit           : 2
            }
            SpinBox
            {
                id: minuteSpinBox
                bottomValidation    : 0
                topValidation       : 59
                noOfDigit           : 2
            }
            SpinBox
            {
                id: secondSpinBox
                bottomValidation    : 0
                topValidation       : 59
                noOfDigit           : 2
            }
        }

        ButtonElement
        {
            id : buttons
			isInImageFormat:true
            leftBtnImagePath: imageLocation + "/ButtonIcons/cancel.png"
            rightBtnImagePath: imageLocation + "/ButtonIcons/ok and set.png"
            anchors.bottomMargin: 5
            buttonRowVisibleF: true

            //buttonLeftText: "Cancel"
            onButtonLeftPress:
            {
                popUpLoader.unloadPopUp()
            }

            //buttonRightText: "Set"
            onButtonRightPress:
            {
                generateDateTimePayload()
                popUpLoader.unloadPopUp()
            }
        }
    }

    function updateDayValidation()
    {
        dayValidation = daysInMonth(dateTimeSettingsScreen.year,dateTimeSettingsScreen.month)

        if( daySpinBox.value > dayValidation)
        {
            daySpinBox.value = dayValidation
        }
    }

    function updateDateTimeData(dateTimeUpdateF,date,
                                month,year,hour,minute,second)
    {
        dateTimeEnableF = dateTimeUpdateF
        daySpinBox.value = date
        monthSpinBox.value = month
        yearSpinBox.value = year
        hourSpinBox.value = hour
        minuteSpinBox.value = minute
        secondSpinBox.value = second
        updateDayValidation()
    }

    function generateDateTimePayload()
    {
        var tempDateTimeData

        appData.data   =    EnumClass.MSG_DATE_TIME_VALIDATION_EVENT +
                            String.fromCharCode(uiMsgFsp).toString() +
                            hour + timeFsp +
                            minute + timeFsp +
                            second + timeFsp +
                            String.fromCharCode(uiMsgFsp).toString() +
                            day + dateFsp +
                            month + dateFsp +
                            year + dateFsp +
                            String.fromCharCode(uiMsgFsp).toString() +
                            String.fromCharCode(eotFsp).toString()
    }
}
