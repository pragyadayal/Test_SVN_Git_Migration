import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id      : viewuserProfile
    height  : lcd_height
    width   : lcd_width

    OpacityRectangle
    {

    }

    PageHeading
    {
        title : dispStr.readDispStr(DispStr.MSG_VIEW_USER_PROFILE_642) /*"View User Profile"*/
    }

    MxRectangleFrame
    {
        id                       : userProfileRectId
        width                    : lcd_width - 6
        height                   : 300//225
        anchors.top              : parent.top
        anchors.topMargin        : 55 //topMarginSettingScreen
        anchors.horizontalCenter : parent.horizontalCenter
    }

    Flickable
    {
        id                      : flickableArea
        height                  : userProfileRectId.height -20
        width                   : userProfileRectId.width - 8
        anchors.centerIn        : userProfileRectId

        contentHeight           : userProfileColumn.height
        contentWidth            : userProfileColumn.width

        boundsBehavior          : Flickable.StopAtBounds
        clip                    : true
        pressDelay              : flickablePressDelay
        smooth                  : true
        flickDeceleration       : flickableFlickDeceleration

        onMovementStarted:
        {
             sendOnlyKeepAlive()
        }

        Column
        {
            id      : userProfileColumn
            spacing : spacingBtwSettingElement + 8

            Row
            {
                id      :row1
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_NAME_373) /*Server "Name" op*/
                }
                TextBox
                {
                    id  : usrName
                }
            }

            Row
            {
                id      : row2
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_DESIGNATION_134) /*"Designation"*/
                }
                TextBox
                {
                    id      : designation
                }
            }

            Row
            {
                id      : row3
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_BRANCH_65) /*"Branch"*/
                }
                TextBox
                {
                    id      : branch
                }
            }

            Row
            {
                id      : row4
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_DEPARTMENT_131) /*"Department"*/
                }
                TextBox
                {
                    id      : department
                }
            }

            Row
            {
                id      : row5
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_BLOOD_GROUP_63) /*"Blood Group"*/
                }
                TextBox
                {
                    id      : bloodGroup
                }
            }

            Row
            {
                id      : row6
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label : dispStr.readDispStr(DispStr.MSG_EMERGENCY_CONTACT_187) /*"EmergencyNo."*/
                }
                TextBox
                {
                    id      : emergencyContact
                }
            }

            Row
            {
                id      : row7
                spacing : spacingBtwSettingLabel
                TextLabel
                {
                    label :  dispStr.readDispStr(DispStr.MSG_MEDICAL_HISTORY_341) /*"Med. History"*/
                }
                TextBox
                {
                    id      : medicalHistory
                }
            }
          }

        }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
    }
}
