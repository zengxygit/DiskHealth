import QtQuick 2.12
import QtQuick.Controls.Styles 1.4
import "."
TreeViewStyle
{
    id:__style
    property var sbw: 6
    property var headerHeight: control.headerHeight!==undefined?control.headerHeight:0
    property var headerColor: control.headerColor!==undefined?control.headerColor:"#00000000"
    property var frameBW: 1
    property var scrollBackColor:"#e6edf5"
    property var frameColor:"#e1e7ef"
    backgroundColor: 'white'
    frame : Rectangle{
        color: __style.backgroundColor
        border.width: __style.frameBW
        border.color: __style.frameColor
        radius: 4
    }
    handle: Rectangle {
        implicitHeight: styleData.horizontal?sbw:height
        implicitWidth: !styleData.horizontal?sbw:width
        color: styleData.pressed?"#9aaec4":(styleData.hovered?"#b1d1e7":"#b3c1d0")
        radius: 4.5
    }
    decrementControl: EuRoundedRectangle {
        width: !styleData.horizontal?sbw:headerHeight
        height:styleData.horizontal?sbw:headerHeight
        color: headerColor
        roundedMask:EuRoundedRectangle.RIGHTTOP
    }

    incrementControl:Item {
        width: 0
        height:0
    }
    scrollBarBackground:Rectangle{
        color: __style.scrollBackColor
        height: (styleData.horizontal?sbw:height)
        width: !styleData.horizontal?sbw:width
    }
}
