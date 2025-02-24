import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
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

    PageHeading
    {
        title :dispStr.readDispStr(DispStr.MSG_BACKUP_AND_UPDATE_60) /*"Backup And Update"*/
    }

    MxMenuScreen
    {
        iconIdxList:
        [
            EnumClass.ICON_RESTORE_CONFIGURATIONS,                //30
            EnumClass.ICON_WRITE_EVENTS,                          //31
            EnumClass.ICON_FIRMWARE_UPGRADE                       //32
        ]

        onIconPress:
        {
            switch(idx)
            {
                default :
                    break
                case EnumClass.ICON_RESTORE_CONFIGURATIONS :
                {
                    appData.data = EnumClass.GUI_BUTTON_READ_CFG +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_WRITE_EVENTS :
                {
                    appData.data = EnumClass.GUI_BUTTON_WRITE_EVENTS +
                                        String.fromCharCode(uiMsgFsp).toString() +
                                        String.fromCharCode(eotFsp).toString()
                    break
                }
                case EnumClass.ICON_FIRMWARE_UPGRADE :
                {
                    appData.data = EnumClass.GUI_BUTTON_FW_UPGRADE +
                                   String.fromCharCode(uiMsgFsp).toString() +
                                   String.fromCharCode(eotFsp).toString()
                    break
                }
            }
        }
    }
}
