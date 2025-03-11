import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
    id      : wifiItem
    width   : lcd_width - 30
    height  : ssidText.height+connectionText.height  //33+24

    property string elementSsidName         : ""
    property int    elementFieldStrength    : 0
    property string   wifiConnectionStatus  : ""
    property bool   elementIsSecured        : false
    property bool   elementWifiConnected    : false

    property bool   elementWpsEnabled       : false
    property string elementBssid            : ""

    signal elementClicked()

    MouseArea
    {
        id              : itemMouseArea
        anchors.fill    : parent
        onPressed :
        {
            elementClicked()
        }
    }

    MxText
    {
        id                  : ssidText
        width               : (parent.width - 40) // -20
        anchors.top         : parent.top
        anchors.topMargin   : 4
        anchors.left        : parent.left
        text                : elementSsidName
        font.pixelSize      : fontSizeProfileInfo
        font.bold	        : elementWifiConnected
        smooth              : true
        color               : "White"
        elide               : Text.ElideRight
    }

    Image
    {
        id                  : strength
        source              : imageLocation + "/Wifi/" + elementFieldStrength.toString() +(elementIsSecured ? "_lock" : "")+".png"
        anchors.right       : parent.right
        anchors.verticalCenter: ssidText.verticalCenter
      	visible             : (isInit && !elementWifiConnected)?  false : true
    }

    MxText
    {
       id                   : connectionText
       anchors.left         : parent.left
       anchors.bottom       : parent.bottom
       anchors.bottomMargin : 4
       text                 : wifiConnectionStatus
       font.pointSize       : fontSizeProfileStatus
       smooth               : true
       color                : "white"
    }

    Seperator
    {

    }
}
