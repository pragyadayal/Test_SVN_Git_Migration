import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Rectangle
{
    width: lcd_width
    height: lcd_height
    color : "black"

    property int currState: 1
    property int maxState: 7
    signal      testBlockpress(int idx)

    MxText
    {
        id : displayText
        text :dispStr.readDispStr(DispStr.MSG_PRESS_RED_BLOCK_830) /*"Press Red Block"*/
        color : "White"
        anchors.centerIn: parent
        font.pointSize: 12
        anchors.verticalCenterOffset: -30
    }

    TestBlock
    {
        id : blockNo1
        anchors.top: parent.top
        anchors.left: parent.left
        visible : (currState == 1) ? true : false
        onBlockPress:
        {
            testBlockpress(1)
        }
    }

    TestBlock
    {
        id : blockNo2
        anchors.top: parent.top
        anchors.right: parent.right
        visible : (currState == 2) ? true : false
        onBlockPress:
        {
            testBlockpress(2)
        }
    }

    TestBlock
    {
        id : blockNo3
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        visible : (currState == 3) ? true : false
        onBlockPress:
        {
            testBlockpress(3)
        }
    }

    TestBlock
    {
        id : blockNo4
        anchors.centerIn: parent
        visible : (currState == 4) ? true : false
        onBlockPress:
        {
            testBlockpress(4)
        }
    }

    TestBlock
    {
        id : blockNo5
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        visible : (currState == 5) ? true : false
        onBlockPress:
        {
            testBlockpress(5)
        }
    }

    TestBlock
    {
        id : blockNo6
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        visible : (currState == 6) ? true : false
        onBlockPress:
        {
            testBlockpress(6)
        }
    }

    TestBlock
    {
        id : blockNo7
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        visible : (currState == 7) ? true : false
        onBlockPress:
        {
            testBlockpress(7)
        }
    }

    onTestBlockpress:
    {
        if(idx == currState)
        {
            currState++
        }

        if(currState > maxState)
        {
            displayText.text = dispStr.readDispStr(DispStr.MSG_TEST_SUCCESS_714)//"Test : SUCCESS"
            appData.data = EnumClass.MSG_TOUCH_SCREEN_TEST_SUCCESS +
                    String.fromCharCode(uiMsgFsp).toString() +
                    String.fromCharCode(eotFsp).toString()
        }
    }
}
