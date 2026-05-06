import QtQuick 2.12
import QtQuick.Controls.Styles 1.4
import "."
TreeViewStyle
{
    id:__style
    property var sbw: 6
    property var headerHeight: control.getHeaderHeight!==undefined?control.getHeaderHeight():0
    property var headerColor: control.headerColor!==undefined?control.headerColor:"#00000000"
    property var frameBW: 1
    backgroundColor: 'white'
    property var borderColor: "#e1e7ef"
    property var mouseIn: false
    frame : Rectangle{
        color: __style.backgroundColor
        border.width: __style.frameBW
        border.color: __style.borderColor
        radius: 4
    }
    handle: Rectangle {
        implicitHeight: styleData.horizontal?sbw:height
        implicitWidth: !styleData.horizontal?sbw:width
        color:window.theme.style("scrollbar","handle")[styleData.pressed?"pressed":(styleData.hovered?"hovered":"normal")]
        radius: 4
        z:100
    }
    decrementControl: EuRoundedRectangle {
        width: !styleData.horizontal?sbw:headerHeight
        height:styleData.horizontal?sbw:headerHeight
        color: headerColor
        roundedMask:EuRoundedRectangle.RIGHTTOP
    }

    incrementControl:Item{}
    scrollBarBackground:Rectangle{
        color: window.theme.style("scrollbar","background")
        height: (styleData.horizontal?sbw:height)
        width: !styleData.horizontal?sbw:width
        radius: 4
        visible: __style.mouseIn || styleData.hovered
    }

    corner: Item{}

    //transientScrollBars:true
    Component.onCompleted: {
        __style.mouseIn = Qt.binding(function(){
            var ret = __style.control.__mouseArea.containsMouse
            return ret
        })
    }
}
