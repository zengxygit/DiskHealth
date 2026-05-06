import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.14
import "."
import "../publicQML/public.js" as Public
import QtGraphicalEffects 1.14
import QtQuick.Shapes 1.14
import QtQuick.Controls 2.14
Menu{
    id:win
    property var blur:window.blur
    property var triggleOffset: 0
    property var wbw: window.theme.style("winBorderWidth")
    leftPadding: blur>0?(20-blur-3):wbw
    rightPadding: leftPadding
    topPadding: (showTriangle?path.th:wbw)+8
    bottomPadding: 8+blur
    property var icon: ""
    property var itemHeight: 32
    property var showTriangle: false
    implicitWidth:260
    property var showParentMask: false

    property var rootColor: window.theme.style("background","color","/publicQML/MenuTheme")

    background:Item{
        implicitWidth: win.implicitWidth
        Rectangle{
            id:bg
            color: "#00000000"
            anchors.fill: parent
            Rectangle{
                id:root
                anchors.margins: blur
                anchors.topMargin: win.showTriangle?(path.th-border.width):0
                anchors.fill: parent
                radius: (blur>0)?4:0
                border.color: window.theme.style("background","border","/publicQML/MenuTheme").color
                border.width: (blur>0)?0:window.theme.style("background","border","/publicQML/MenuTheme").width
                color:win.rootColor
            }
            Shape {
               id:shape
               visible: win.showTriangle
               smooth: true
               antialiasing: true
               ShapePath {
                   property var tw: 20
                   property var th: 12
                   id:path
                   strokeColor: window.theme.style("shadow","strokeColor")
                   strokeWidth: shadow.visible?0:root.border.width
                   fillColor: root.color
                   startX: (bg.width-tw)/2+win.triggleOffset
                   startY: th
                   PathLine { x: bg.width/2+win.triggleOffset; y: 0 }
                   PathLine { x: bg.width/2+path.tw/2+win.triggleOffset; y: path.th }
               }
            }
        }
        DropShadow{
          id:shadow
          anchors.fill: parent
          radius: window.theme.style("shadow","blur")
          color: window.theme.style("shadow","color")
          source: bg
          visible: blur>0
        }
    }
    delegate: EuMenuItem{
        implicitHeight:win.itemHeight
    }

    onVisibleChanged: {
        if(window.notifyPopup!== undefined) window.notifyPopup(visible)
    }
    Component.onDestruction:{
        if(window.notifyPopup!== undefined) window.notifyPopup(false)
        if(window.showParentMask) window.popupWin(this,false)
    }
}
