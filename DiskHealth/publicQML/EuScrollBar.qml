import QtQuick 2.12
import QtQuick.Controls 2.4

ScrollBar
{
    id:control
    property var side: 6
    property var h: orientation === Qt.Horizontal
    x:!h?parent.width-width-2:0
    y:h?parent.height-height-2:0
    width: h?parent.availableWidth:side
    height: !h?parent.availableHeight:side
    z:1
    padding: 0
    visible: policy!==ScrollBar.AlwaysOff
    contentItem:Rectangle{
        radius: (control.h?height:width)/2
        color: control.pressed?"#9aaec4":(control.hovered?"#b1d1e7":"#b3c1d0")
    }
    background:Rectangle{
        radius: (control.h?height:width)/2
        color: "#e6edf5"
        //opacity: 0.8
    }
}
