import QtQuick 1.1
import ApplControl 1.0
import EnumClass 1.0
import DispStr 1.0
import "../Common"
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"
import "../payLoadHandler.js" as PayloadLib

Item
{
    id: jobCodeDisp
    width : lcd_width
    height: lcd_height

    property int        noOfJobCodes  :0
    property variant    jobCodeList : [""]
    property alias      writtenJobCode : jobCodeInput.value

    property variant    btnImgVisibleF : [false, false, false, false, false]
    property variant    dfltBtnImgVisibleF : [false, false, false, false, false]
    property int        currLeftMargin : 0
    property int        prevLeftMargin : 0
    property int        nextLeftMargin : 0
    property int        pageBtnLength : 30
    property variant    btnFontSize : [fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs]
    property variant    dfltBtnFontSize : [fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs]
    property variant    currPageBtnLabel: ["", "", "", "", ""]
    property variant    prevPageBtnLabel: ["", "", "", "", ""]
    property variant    nextPageBtnLabel: ["", "", "", "", ""]
    property int        currentPageIdx: 0
    property int        currStartPageNo: 1
    property int        currSubPageCnt
    property int        prevStartPageNo
    property int        prevSubPageCnt
    property int        nextStartPageNo
    property int        nextSubPageCnt
    property int        maxRow : 10
    property int        maxBtn : 5
    property bool       btnListInteractiveF: true
    property int        currPageList : 1

    signal selectRow(int idx)
    signal selectPage(int idx)

    OpacityRectangle
    {

    }

    PageHeading
    {
        id : jobCodeHeading
        title : dispStr.readDispStr(DispStr.MSG_JOB_CODES_828)  /*"Job Codes"*/
    }

    MxRectangleFrame
    {
        id : jobCodeListRect
        width : lcd_width
        height: alphaNumKeypadF  ? 180 : 330
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : jobCodeHeading.bottom
        anchors.topMargin: 10

        MxRectangleFrame
        {
            id : jobCodeRectId
            width : parent.width
            height : 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            //anchors.topMargin: 30

            TextEditBox
            {
                id : jobCodeInput
                anchors.centerIn: parent
                isLargeTextBox: true
                maxChar: 10
            }

            MxText
            {
                id : jobCodeSearchLabel
                anchors.verticalCenter: jobCodeInput.verticalCenter
                anchors.left: jobCodeInput.left
                anchors.leftMargin: 6
                color : "white"
                font.pointSize:fontSizeTextInputs
                text : dispStr.readDispStr(DispStr.MSG_ENTER_JOB_CODE_687)  /*"Enter Job Code"*/
                visible: (writtenJobCode.length > 0) ? false : true
                font.italic: true
            }

            Image
            {
                id: searchImg
                source: imageLocation + "/Controls/Search.png"
                anchors.verticalCenter: jobCodeInput.verticalCenter
                anchors.right: jobCodeInput.right
                anchors.rightMargin: 5
                visible: (writtenJobCode.length > 0) ? false : true
            }

            Image
            {
                id: clearSearchImg
                source: imageLocation + "/Controls/Clear Search.png"
                anchors.verticalCenter: jobCodeInput.verticalCenter
                anchors.right: jobCodeInput.right
                anchors.rightMargin: 5
                visible: (writtenJobCode.length > 0) ? true : false

                MouseArea
                {
                    id:clearJobCodeMouse
                    anchors.fill: parent
                    onClicked:
                    {
                        jobCodeInput.value = ""
                        searchJobCodeList()
                    }
                }
            }
        }

        MxText
        {
            id : noRecordsFoundText
            anchors.top : jobCodeRectId.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter:  parent.horizontalCenter
            font.pointSize : fontSizeTextInputs
            color: "white"
            text :  dispStr.readDispStr(DispStr.MSG_NO_RECORDS_FOUND_393) /*No Records Found...*/
            font.italic: true
            visible : ((noOfJobCodes > 0) || (writtenJobCode.length == 0)) ? false : true
        }

        Component
        {
            id:listDelegate
            Item
            {
                id  : compRect
                width: jobCodeListRect.width
                height: 30

                property bool    rowImgVisibleF : false

                Image
                {
                    id: jobCodeRowSelect
                    source: imageLocation + "/Controls/Selected_Backgound.png"
                    anchors.centerIn: parent
                    visible : rowImgVisibleF
                }
                MouseArea
                {
                    id:jobCodeMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged:
                    {
                        if(rowImgVisibleF === true)
                        	rowImgVisibleF = false
                    }

                    onPressed:
                    {
                        rowImgVisibleF = true
                    }

                    onClicked:
                    {
                        selectRow(index)
                    }                    
                }
                Rectangle
                {
                    id: bulletRect
                    height: (fontSizeTextInputs * 0.5)
                    width: (fontSizeTextInputs * 0.5)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left : parent.left
                    color: jobCodeId.color
                    radius: 360
                }
                MxText
                {
                    id:jobCodeId
                    text: JobCode
                    font.pointSize : fontSizeTextInputs
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left : bulletRect.right
                    anchors.leftMargin: 10
                    color : (JobCode == "Default") ? "#46bdfb" : (JobCode == "Continue") ? "#adffbc" :  "white"
                  }
            }
        }

        ListModel
        {
            id:jobCodeModel
        }
        ListView
        {
            id                      : codeList
            height                  : jobCodeListRect.height - jobCodeRectId.height
            width                   : jobCodeListRect.width - 10
            anchors.top             : parent.top
            anchors.topMargin       : jobCodeRectId.height
            anchors.horizontalCenter: parent.horizontalCenter

            model                   : jobCodeModel
            delegate                : listDelegate

            snapMode                : ListView.SnapToItem
            boundsBehavior          : Flickable.StopAtBounds
            clip                    : true
            cacheBuffer             : flickableCacheBuffer
            pressDelay              : flickablePressDelay
            smooth                  : true
            flickDeceleration       : flickableFlickDeceleration

            onMovementStarted:
            {
                unloadKeypad()
                sendOnlyKeepAlive()
            }
        }
    }

    Item
    {
        id: paginationRow
        width : codeList.width
        height : pageBtnLength
        anchors.top: jobCodeListRect.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        visible: !alphaNumKeypadF

        Image
        {
            id: previousBtn
            height: parent.height
            width: height
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: imageLocation + "/Controls/Left.png"

            MouseArea
            {
                id: mousePreviousBtn
                anchors.fill: parent
                onClicked:
                {
                    //Display Previous
                    getPreviousJobCodeList()
                }
            }
        }

        VisualItemModel
        {
            id: jobPageBtnModel

            Item
            {
                height  : prevPage.height
                width   : prevPage.width
                clip    : true

                JobCodePageBtnRow
                {
                    id: prevPage

                    rowLeftMargin: prevLeftMargin
                    btnLabel: prevPageBtnLabel
                    imgVisibleF: dfltBtnImgVisibleF
                    labelFontSize: dfltBtnFontSize

                    onBtnClick:
                    {
                        selectPage(idx)
                    }
                }
            }

            Item
            {
                height  : currPage.height
                width   : currPage.width
                clip    :  true

                JobCodePageBtnRow
                {
                    id: currPage

                    rowLeftMargin: currLeftMargin
                    btnLabel: currPageBtnLabel
                    imgVisibleF: btnImgVisibleF
                    labelFontSize: btnFontSize

                    onBtnClick:
                    {
                        selectPage(idx)
                    }
                }
            }

            Item
            {
                height  : nextPage.height
                width   : nextPage.width
                clip    :  true

                JobCodePageBtnRow
                {
                    id: nextPage

                    rowLeftMargin: nextLeftMargin
                    btnLabel: nextPageBtnLabel
                    imgVisibleF: dfltBtnImgVisibleF
                    labelFontSize: dfltBtnFontSize

                    onBtnClick:
                    {
                        selectPage(idx)
                    }
                }
            }
        }

        ListView
        {
            id                      : btnRow
            height                  : parent.height
            width                   : parent.width -30 -previousBtn.width -nextBtn.width
            anchors.top             : parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            model                   : jobPageBtnModel

            interactive             : btnListInteractiveF
            preferredHighlightBegin : 0
            preferredHighlightEnd   : 0
            highlightRangeMode      : ListView.StrictlyEnforceRange
            orientation             : ListView.Horizontal

            snapMode                : ListView.SnapToItem
            boundsBehavior          : Flickable.StopAtBounds
            clip                    : true
            cacheBuffer             : flickableCacheBuffer
            pressDelay              : flickablePressDelay
            smooth                  : true
            flickDeceleration       : flickableFlickDeceleration

            Component.onCompleted:
            {
                positionViewAtIndex(currPageList, ListView.Center)
            }

            onMovementStarted:
            {
                sendOnlyKeepAlive()
            }

            onMovementEnded:
            {
                getJobCodeListOnSwipe(currentIndex)
            }
        }

        Image
        {
            id: nextBtn
            height: parent.height
            width: height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: imageLocation + "/Controls/Right.png"

            MouseArea
            {
                id: mouseNeBtnBtn
                anchors.fill: parent
                onClicked:
                {
                    //Display Next
                    getNextJobCodeList()
                }
            }
        }
    }


    Connections
    {
        target: alphaNumKeypadId
        onHidePress :
        {
            jobCodeInput.focus = false
        }
    }

    onWrittenJobCodeChanged:
    {
        currentPageIdx = 0
        searchJobCodeList()
    }

    onSelectRow:
    {
        appData.data =  EnumClass.MSB_JOB_CODE_SELECTED_EVENT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        ( (currentPageIdx * maxRow)+idx) + String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()

    }

    onSelectPage:
    {
        sendOnlyKeepAlive()
        currentPageIdx = idx
        displayJobCodePage()
    }

    function searchJobCodeList()
    {
        appData.data =  EnumClass.MSG_SEARCH_JOB_CODE_LIST_EVENT +
                        String.fromCharCode(uiMsgFsp).toString() +
                        jobCodeInput.value + String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()        
    }

    function getNextJobCodeList()
    {
        appData.data =  EnumClass.GUI_BUTTON_NEXT_JOB_CODE_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()

    }

    function getPreviousJobCodeList()
    {
        appData.data =  EnumClass.GUI_BUTTON_PREVIOUS_JOB_CODE_LIST +
                        String.fromCharCode(uiMsgFsp).toString() +
                        String.fromCharCode(eotFsp).toString()

    }

    function getJobCodeListOnSwipe(swipeToPage)
    {
        if(swipeToPage > currPageList)
            getNextJobCodeList()
        else if(swipeToPage < currPageList)
            getPreviousJobCodeList()
    }

    function displayJobCodePage()
    {
        var tmp = new Array()

        tmp = [false, false, false, false, false]
        tmp[currentPageIdx] = true
        btnImgVisibleF = tmp

        tmp = [fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs, fontSizeTextInputs]
        tmp[currentPageIdx] = fontSizeTextInputs + 2
        btnFontSize = tmp


        jobCodeModel.clear()
        var startJobCodeIdx = currentPageIdx * maxRow
        for(var jobIdx = 0; ((jobIdx < maxRow) && (startJobCodeIdx < noOfJobCodes)); jobIdx++, startJobCodeIdx++)
        {
            jobCodeModel.insert (jobIdx, {"JobCode":jobCodeList[startJobCodeIdx]})
        }
    }

    function setPaginationLabel()
    {
        var tmpBtnRowLeftMargin = 0
        var tmpPageBtnLabel = new Array()
        var btnIdx;
        for(btnIdx = 0; btnIdx < maxBtn; btnIdx++)
        {
            if(btnIdx < currSubPageCnt)
            {
                tmpPageBtnLabel[btnIdx] = (currStartPageNo + btnIdx);
                tmpBtnRowLeftMargin += (pageBtnLength / 2)
            }
            else
            {
                tmpPageBtnLabel[btnIdx] = ""
            }
        }
        //left is placed at center and left margin is from center to left side
        currLeftMargin = 0 -tmpBtnRowLeftMargin
        currPageBtnLabel = tmpPageBtnLabel
        btnRow.positionViewAtIndex(currPageList, ListView.Center)

        tmpPageBtnLabel = new Array()
        tmpBtnRowLeftMargin = 0
        for(btnIdx = 0; btnIdx < maxBtn; btnIdx++)
        {
            if(btnIdx < prevSubPageCnt)
            {
                tmpPageBtnLabel[btnIdx] = (prevStartPageNo + btnIdx);
                tmpBtnRowLeftMargin += (pageBtnLength / 2)
            }
            else
            {
                tmpPageBtnLabel[btnIdx] = ""
            }
        }
        //left is placed at center and left margin is from center to left side
        prevLeftMargin = 0 -tmpBtnRowLeftMargin
        prevPageBtnLabel = tmpPageBtnLabel

        tmpPageBtnLabel = new Array(5)
        tmpBtnRowLeftMargin = 0
        for(btnIdx = 0; btnIdx < maxBtn; btnIdx++)
        {
            if(btnIdx < nextSubPageCnt)
            {
                tmpPageBtnLabel[btnIdx] = (nextStartPageNo + btnIdx);
                tmpBtnRowLeftMargin += (pageBtnLength / 2)
            }
            else
            {
                tmpPageBtnLabel[btnIdx] = ""
            }
        }
        //left is placed at center and left margin is from center to left side
        nextLeftMargin = 0 -tmpBtnRowLeftMargin
        nextPageBtnLabel = tmpPageBtnLabel
    }

    function processDeviceResponse (msgId,rcvData)
    {
        PayloadLib.parsePayload(msgId, rcvData)
        currentPageIdx = 0
        setPaginationLabel()
        displayJobCodePage()
        //jobCodeInput.forceActiveFocus();
    }
}
