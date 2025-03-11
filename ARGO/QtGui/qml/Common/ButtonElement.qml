import QtQuick 1.1
import "../../../../Common_Source/Linux_Devices_Common_Source/QtGui/qml/Common"

Item
{
   id       : buttonElementItem
   height   : 36

   property int addBtnBottomMargin : 0

   anchors.horizontalCenter: parent.horizontalCenter
   anchors.bottom: parent.bottom
   anchors.bottomMargin: ( alphaNumKeypadF ? 165 : (numKeypadF ? 165 : 50 ) ) + addBtnBottomMargin

   property bool isInImageFormat:false
   property alias leftBtnImagePath: leftButton.buttonImagePath
   property alias rightBtnImagePath: rightButton.buttonImagePath
   property alias centerBtnImagePath: centerButton.buttonImagePath

   property bool buttonRowVisibleF : false
   property bool buttonCenterVisibleF : false

   property bool buttonRightEnable : true
   property bool buttonLeftEnable : true
   property bool buttonCenterEnable : true

   signal buttonLeftPress
   signal buttonRightPress
   signal buttonCenterPress

   property string buttonLeftText
   property string buttonRightText
   property string buttonCenterText

   Row
   {
       id : buttonRow
       spacing : 5
       anchors.centerIn: parent
       visible : buttonRowVisibleF
       ButtonWithLabel
       {
           id : leftButton
           button_Text: (isInImageFormat)?"":buttonLeftText
           enableF : buttonLeftEnable
           isButtonInImageFormat: isInImageFormat
           buttonImagePath: leftBtnImagePath

           onButtonwithLabelPress:
           {
               buttonLeftPress()
           }
       }
       ButtonWithLabel
       {
           id : rightButton
           button_Text:(isInImageFormat)?"":buttonRightText
           enableF : buttonRightEnable
           isButtonInImageFormat: isInImageFormat
           buttonImagePath: rightBtnImagePath

           onButtonwithLabelPress:
           {
               buttonRightPress()
           }
       }
   }

   ButtonWithLabel
   {
       id : centerButton
       anchors.centerIn: parent
       button_Text: (isInImageFormat)?"":buttonCenterText
       visible : buttonCenterVisibleF
       enableF : buttonCenterEnable
       isButtonInImageFormat: isInImageFormat
       buttonImagePath: centerBtnImagePath
	   onButtonwithLabelPress:
       {
           buttonCenterPress()
       }
   }
}
