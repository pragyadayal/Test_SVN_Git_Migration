import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"


Image
{
    property alias cafeFooterF      : cafeFooter.visible
    property alias clearMenu        : cafeClearMenu.visible
    property alias payBill          : cafePayBill.visible
    property alias menu             : cafeMenuRect.visible

    signal payBillClicked
    signal clearMenuClicked

    property int footerIconWidth : 65

    id: cafeFooter
    source: imageLocation + "/Footer/Footerbar.png"
    visible: footerVisible
    anchors.bottom : parent.bottom

    FooterIcon
    {
        id               : cafeMenuRect
        width            : footerIconWidth
        footerSource     : imageLocation + "/Footer/Menu.png"
        visible          : false
        anchors.centerIn : parent
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
        }
    }

    FooterIcon
    {
        id               : cafeClearMenu
        width            : footerIconWidth
        footerSource     : imageLocation + "/Footer/ClearMenu.png"
        visible          : false
        anchors.left     : parent.left
        onFooterPress    :
        {
            appData.data = EnumClass.GUI_BUTTON_CLEAR_CAFE_MENU +
                           String.fromCharCode(uiMsgFsp).toString() +
                           String.fromCharCode(eotFsp).toString()
            clearMenuClicked()
        }
    }

    FooterIcon
    {
        id               : cafePayBill
        width            : footerIconWidth
        footerSource     : imageLocation + "/Footer/PayBill.png"
        visible          : false
        anchors.right    : parent.right
        onFooterPress    :
        {
            payBillClicked()
        }
    }
}
