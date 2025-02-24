import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import "../Appl"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: statusbar
    visible: true

    property bool alarmF
    property bool serverF
    property bool usbF
    property bool ethernetF

    property int  wifiStatus
    property int  modemStatus
    property int  doorLockUnlock
    property int  splFuncStatus
    property int  bluetoothStatus : 0
    property int  statusBarItemHeight: headerImage.height

    //panel door related

    property bool dndF
    property bool degradeModeF
    property bool networkFaultF
    property bool deadManZoneF
    property bool downloadingF : false
    
    property int degradeModeType

    //Temperature
    property bool temperatureSensorStatusF : false
    property string temperatureString : ""
    property bool temperatureCnfgEnable: false
    property int  wifiStrength: 0

    property string localImgLoc : imageLocation + "/StatusBar/"

    //Face Sync related
    property int fsmSyncStatus : 0
    property bool faceSyncProgressF: fsmSyncStatus == 2 ? true : false
    property bool faceSyncErrorF: (fsmSyncStatus == 1 || fsmSyncStatus == 3) ? true : false

    property variant statusF : (!doorFirmwareType) ?
    [
        temperatureCnfgEnable,
        ethernetF,
        ( (wifiStatus == 0) ? false : true ),
        ( (modemStatus == 0) ? false : true ),
        alarmF,
        ( (doorLockUnlock == 0) ? false : true ),
        true,
        usbF,
        ( ( splFuncStatus == 0) ? false : true ),
        ( ( bluetoothStatus == 0) ? false : true ),
     //   ( (fsmSyncStatus == 0 || fsmSyncStatus == 2 || fsmSyncStatus >= 4) ? false : true),
    ]:
   [
    temperatureCnfgEnable,
    ethernetF,
    ( (wifiStatus == 0) ? false : true ),
    networkFaultF,
    degradeModeF,
    alarmF,
    ( (doorLockUnlock == 0) ? false : true ),
    ( ( splFuncStatus == 0) ? false : true ),
    dndF,
    deadManZoneF,
    ( ( bluetoothStatus == 0) ? false : true ),
   ]

    property variant imgSource : (!doorFirmwareType) ?
    [
        ((temperatureSensorStatusF == 1) ? "Device Connected.png" : "Device DisConnected.png"),
        "Ethernet.png",
        ( (wifiStatus == 1) ? "WiFiSigLevel0.png" : getWifiIcon(wifiStrength)),
        ( (modemStatus == 1) ? "Modem.png" : "ModemConnected.png" ),
        "Alarm.png",
        ( (doorLockUnlock == 1) ? "DoorLock.png" : "DoorUnlock.png" ),
        ( serverF ? "ServerConnected.png" : "Server.png" ),
        "USB.png",
        ( (splFuncStatus == 1) ? "LateIn.png" : "EarlyOut.png" ),
        ( (bluetoothStatus == 1) ? "BluetoothConnected.png" : "Bluetooth.png" ),
    //    ( (fsmSyncStatus == 1 || fsmSyncStatus == 3) ? "error.png":"" ),
    ]:
    [
        ((temperatureSensorStatusF == 1) ? "Device Connected.png" : "Device DisConnected.png"),
         "Ethernet.png",
         ( (wifiStatus == 1) ? "WiFiSigLevel0.png" : getWifiIcon(wifiStrength) ),
         "NetworkFault.png",
         "DegradeMode.png",
          "Alarm.png",
         ( (doorLockUnlock == 1) ? "DoorLock.png" : "DoorUnlock.png" ),
         ( (splFuncStatus == 1) ? "LateIn.png" : "EarlyOut.png" ),
         "DND.png",
         "DeadManZone.png",
         ( (bluetoothStatus == 1) ? "BluetoothConnected.png" : "Bluetooth.png" ),
    ]

    Image
    {
        id: headerImage
        source: localImgLoc + "headerbar.png"
        Row
        {
            id: statusRow
            smooth: true
            anchors.right: headerImage.right
            anchors.rightMargin: 10
            anchors.verticalCenter: headerImage.verticalCenter
            spacing: 3

            AnimatedImage
            {
                id : downloadingImgId
                source : localImgLoc + "Downloading.gif"
                visible: downloadingF
                playing : true
            }
	    
            AnimatedImage
            {
                id : faceSyncImgId
                source : localImgLoc + "rotating.gif"
                visible: faceSyncProgressF
                playing : true
            }

            AnimatedImage
            {
                id : faceSyncImgID
                source : localImgLoc + "error.png"
                visible: faceSyncErrorF
                playing : true
            }

            Repeater
            {
                model : imgSource.length
                Image
                {
                    source : localImgLoc + imgSource[index]
                    visible : statusF[index]
                }
            }

        }
    }

    MxText
    {
        id: time
        anchors.left: headerImage.left
        anchors.leftMargin: 5
        anchors.verticalCenter: headerImage.verticalCenter
        color: "white"
        smooth: true
        font.pointSize: 15
        text: timeFormat ? Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                  hours,minutes,seconds),"hh:mm AP").toString().slice(0, -2) :
                           Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                  hours,minutes,seconds), "hh:mm" )
    }
    MxText
    {
        id                   : ampmText
        color                : mxColorBlue
        smooth               : true
        font.bold            : true
        font.pointSize       : 13
        anchors.left         : time.right
        anchors.verticalCenter: headerImage.verticalCenter
        visible              : timeFormat ? true : false
        text                 : Qt.formatTime(new Date(currentYear,currentMonth,currentDate,
                                                      hours,minutes,seconds),"AP")
    }

    MxText
    {
        id: temperatureText
        anchors.left: ampmText.right
        anchors.leftMargin: 5
        anchors.verticalCenter: headerImage.verticalCenter
        color: "white"
        smooth: true
        font.pointSize: 20
        text:   temperatureString
        visible: (temperatureCnfgEnable == 1) ? true : false
    }

    SlideScreen{
        id: slider
    }

    function unloadSlideScreen()
    {
        slider.hideSlideScreen()
    }

    function startStopSlideScreen(enableStatus)
    {
        if((statusBarExpandedF === true) && (slider.enabled !== enableStatus))
        {
            unloadSlideScreen()
            slider.enabled = false/*enableStatus*/
        }
    }
    function getWifiIcon(wifiStr)
    {
        if(wifiStr == EnumClass.NO_WIFI_SIGNAL)
        {
            return "WiFiSigLevel0.png"
        }
        else if (wifiStr == EnumClass.WEAK_WIFI_SIGNAL)
        {
            return "WiFiSigLevel1.png"
        }
        else if (wifiStr == EnumClass.MODERATE_WIFI_SIGNAL)
        {
            return "WiFiSigLevel2.png"
        }
        else if (wifiStr == EnumClass.STRONG_WIFI_SIGNAL)
        {
            return "WiFiSigLevel3.png"
        }
        else if (wifiStr == EnumClass.STRONGEST_WIFI_SIGNAL)
        {
            return "WiFiSigLevel4.png"
        }
        else
        {
            return
        }

    }
}
