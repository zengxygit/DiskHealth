import QtQuick 2.12
import QtQuick.Window 2.12
import "."
import QtQuick.Controls 2.14
import "../publicQML/public.js" as Public
import QtQuick.Shapes 1.14
import QtGraphicalEffects 1.14
ToolTip{
    id:control
    delay: 2
    property bool showTriangle: false
    property bool upTriggle: true
    property var triggleSize: 12
    property var trigger: null
    property var offsetY: 0
    property var offsetX: 0
    property var triangleOffset: 0
    property var alignment: Qt.AlignHCenter
    property var textColor: window.theme.style("tooltip","text")
    property var textWrapMode:Text.WordWrap
    property var fontPixcelSize: 14
    property var windowBlur: 4
    property var bgColor: window.theme.style("tooltip","background")
    property var borderColor: window.theme.style("tooltip","border").color
    property var radius: 4
    property var defaultHeight: 32
    property var cHeight: defaultHeight
    property var cLineCount: 1
    background: Item{
        height: control.cHeight+(control.showTriangle?control.triggleSize/2:0) + (control.cLineCount-1)*8
        Rectangle{
            id:bg
            anchors.fill: parent
            color: "#00000000"
            Rectangle{
                id:root
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: (control.showTriangle && control.upTriggle) ? control.triggleSize:0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: (control.showTriangle && !control.upTriggle) ? control.triggleSize:0
                color: control.bgColor
                border.width: window.theme.style("tooltip","border").width
                border.color: control.borderColor
                radius: control.radius
            }
            Rectangle{
                visible: control.showTriangle
                width: control.triggleSize
                height: control.triggleSize
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: control.upTriggle?control.triggleSize*0.72:(parent.height-control.triggleSize)
                rotation: 45
                color: root.color
                border.width: root.border.width
                border.color: root.border.color
                smooth: true
            }
            Rectangle{
                anchors.fill: root
                border.width: 0
                color: root.color
                radius: root.radius
                anchors.margins: root.border.width
            }
        }
        DropShadow {
           anchors.fill: parent
           radius: control.windowBlur
           color: root.border.color
           source: bg
           visible: radius>0
        }
    }
    contentItem:Item{
            Text {
             text: control.text
             font.pixelSize: control.fontPixcelSize
             color: control.textColor
             leftPadding: 8
             rightPadding: 8
             topPadding: (control.showTriangle&&control.upTriggle)?10:0
             anchors.centerIn: parent
             wrapMode: control.textWrapMode
             width: control.width
             horizontalAlignment: control.alignment
             //textFormat: Text.RichText
             onHeightChanged: {
                 control.cHeight = Math.max(control.defaultHeight,height)
             }
             onLineCountChanged: {
                 control.cLineCount = lineCount
             }
        }

         //font.family: window.font.family
   }
   Component.onCompleted: {
       if(trigger===null)return
       y=trigger.y+trigger.height+offsetY
   }
}
