import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    height : lcd_height
    width  : lcd_width

    OpacityRectangle
    {

    }

    MxMenuScreen
    {
        iconIdxList:
            (!doorFirmwareType)?
            (//direct door
                (doorApplicationType == 2 ) ?
                [//ecanteen
                    EnumClass.ICON_ADMIN,
                    EnumClass.ICON_HRD,
                    EnumClass.ICON_ABOUT_DEVICE,
                    EnumClass.ICON_E_CANTEEN
                ] :
                [//T&A,accesscontrol
                    EnumClass.ICON_ADMIN,
                    EnumClass.ICON_HRD,
                    EnumClass.ICON_ABOUT_DEVICE
                ]
             ):
             (//panel door
                 (currentDoorStatus !== EnumClass.DOOR_ONLINE) ?
                 [//degrade mode
                    EnumClass.ICON_ADMIN,
                    EnumClass.ICON_ABOUT_DEVICE
                 ] :
                 [//online
                    EnumClass.ICON_ADMIN,
                    EnumClass.ICON_HRD,
                    EnumClass.ICON_ABOUT_DEVICE
                 ]
             )


        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_ADMIN :
                {
                    abtDeviceScreenF = false
                    isAdminHrdF = true
                    appData.data = EnumClass.GUI_BUTTON_ADMIN_MENU +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                    String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_HRD :
                {
                    abtDeviceScreenF = false
                    isAdminHrdF = false
                    appData.data = EnumClass.GUI_BUTTON_HRD_MENU +
                                       String.fromCharCode(uiMsgFsp).toString() +
                                       String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_ABOUT_DEVICE :
                {
                    isAdminHrdF = true
                    abtDeviceScreenF = true
                    appData.data = EnumClass.GUI_BUTTON_SYSTEM_STATUS +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }

                case EnumClass.ICON_E_CANTEEN :
                {
                    abtDeviceScreenF = false
                    isAdminHrdF = false
                    appData.data = EnumClass.GUI_BUTTON_E_CANTEEN_MENU +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }
    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId,rcvData)
    }
}
