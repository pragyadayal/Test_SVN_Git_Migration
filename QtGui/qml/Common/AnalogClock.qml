import QtQuick 1.1
import DispStr 1.0
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id      : clock
    property string localImgLoc : imageLocation + "/AnalogClock/"

    Item
    {
        id : dataSubRect
        anchors.top : parent.top
        visible : true
        opacity : 0.8
        MxText
        {
            id                     : dayAndDateStr
            color                  : "white"
            horizontalAlignment    : Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            smooth                 : true
            font.bold              : true
            font.pointSize         : 20
            text                   : dispStr.timeDispStr(currentYearMultiL,currentMonthMultiL,currentDateMultiL,currentDayMultiL,hours,minutes,seconds,DispStr.DD_MMM_YYYY_DAY)
            font.family            : textFontFamily
            style                  : Text.Outline
            styleColor             : "black"
        }
    }


    Item
    {
        width   : 150
        height  : 150
        anchors.top : dataSubRect.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        Image
        {
            id                  : background
            anchors.centerIn    : parent
            source              :localImgLoc + "clock.png"
        }

        Image
        {
            id                      : hourHand
            anchors.horizontalCenter: parent.horizontalCenter
            y                       : 50
            source                  : localImgLoc + "hour.png"
            smooth                  : true
            transform: Rotation
            {
                id          : hourRotation
                origin.x    : (hourHand.sourceSize.width/2 )
                origin.y    : 24
                angle       : (hours * 30) + (minutes * 0.5)
                Behavior on angle
                {
                    SpringAnimation { spring: 1; damping: 1; modulus: 360 }
                }
            }
        }

        Image
        {
            id                          : minuteHand
            anchors.horizontalCenter    : parent.horizontalCenter
            y                           : 36
            source                      : localImgLoc + "minute.png"
            smooth                      : true
            transform: Rotation
            {
                id          : minuteRotation
                origin.x    : (minuteHand.sourceSize.width/2 )
                origin.y    : 40
                angle       : minutes * 6
                Behavior on angle
                {
                    SpringAnimation { spring: 1; damping: 1; modulus: 360 }
                }
            }
        }

        Image
        {
            id                      : secondHand
            anchors.horizontalCenter: parent.horizontalCenter
            y                       : 30
            source                  : localImgLoc + "second.png"
            smooth                  : true
            transform: Rotation
            {
                id          : secondRotation
                origin.x    : (secondHand.sourceSize.width/2 )
                origin.y    : 43
                angle       : seconds * 6
                Behavior on angle
                {
                    SpringAnimation { spring: 1; damping: 1; modulus: 360 }
                }
            }
        }

        Image
        {
            anchors.centerIn            : background
            anchors.horizontalCenter    : parent.horizontalCenter
            source                      : localImgLoc + "center.png"
        }
    }
 }
