import QtQuick 2.0
Item{
    signal clicked()
    property alias radius: ebsr.radius
    property alias color: ebsr.color
    property alias gradient: ebsr.gradient
    property alias alwaysShow: ebsr.alwaysShow
    property alias border: ebsr.border
    property alias shadowColor: ebsr.shadowColor
    property alias v_shadow: ebsr.v_shadow
    property alias h_shadow: ebsr.h_shadow
    property alias blur: ebsr.blur
    property alias samples: ebsr.samples
    property alias spread: ebsr.spread
    property alias hovered: ma.containsMouse
    property alias containsMouse: ebsr.containsMouse
    property alias acceptedButtons: ebsr.acceptedButtons
    property alias mouseArea: ma
    property alias animateHeight: ebsr.animateHeight
    property var content: Component{Item{}}
    state: ebsr.state
    id:control
    EuBorderShadowRect{
        id:ebsr
        width: parent.width
        height: parent.height-animateHeight
        anchors.topMargin: animateHeight
        anchors.top: parent.top
        hoverEnabled: false
        Loader{
            anchors.fill: parent
            sourceComponent: control.content
        }
    }
    MouseArea{
        id:ma
        anchors.fill:parent
        hoverEnabled: true
        function validMouseEvt(){
            return true
//            if(mouseY<ebsr.animateHeight&&mouseY>=0){
//                if(ebsr.state === "normal") return false
//            }
//            if(mouseY<parent.height-ebsr.animateHeight&&mouseY>=0){
//                if(ebsr.state === "hovered") return false
//            }
//            return true
        }

        onContainsMouseChanged: {
            if(!validMouseEvt()) return
            ebsr.state = containsMouse?"hovered":"normal"
        }
        onClicked: {
            if(!validMouseEvt()) return
            parent.clicked()
        }
    }
    onStateChanged: {
        ebsr.state = state
    }
    Component.onCompleted: {
        var sbling = visibleChildren
        for (var i = 0; i < sbling.length; i++){
            if(ma===sbling[i] || ebsr === sbling[i]) continue
            var topm = sbling[i].anchors.topMargin
            if(topm===undefined) return
            sbling[i].anchors.topMargin = Qt.binding(function(){return ebsr.anchors.topMargin+topm})
        }
    }
}
