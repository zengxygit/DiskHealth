import QtQuick 2.0
import QtGraphicalEffects 1.14
MouseArea {
    id:control
    property alias hovered: control.containsMouse
    property alias border: frame.border
    property var shadowColor:"#e5e7f1"
    property var v_shadow: 0
    property var h_shadow: 0
    property var blur: 16.0
    property var samples: 16
    property var spread: 0
    property bool alwaysShow: animating || state ==="hovered"
    acceptedButtons: Qt.NoButton
    hoverEnabled: true
    property alias color: bg.color
    property alias radius: bg.radius
    property alias gradient: bg.gradient
    property alias center:bg
    property var inited: false
    state: "inited"
    property var animating: false
    property var defaultMargin: 0
    property var animateHeight:6
    states:[
        State {
            name: "inited"
            PropertyChanges {
                target: control
                color:"#ffffff"
            }
        },
        State {
            name: "normal"
            PropertyChanges {
                target: control
                anchors.topMargin:defaultMargin
            }
        },
        State {
            name: "hovered"
            PropertyChanges {
                target: control
                anchors.topMargin:defaultMargin-animateHeight
            }
        }
    ]
    transitions: [
        Transition {
        to: "hovered"
        NumberAnimation { properties:"anchors.topMargin";easing.bezierCurve:[0,0,0.58,1];duration: 150 }
        },
        Transition {
          to: "normal"
          NumberAnimation {properties:"anchors.topMargin";easing.bezierCurve:[0,0,0.58,1];duration: 150 }
        }
    ]
    onHoveredChanged: {
        inited = true
        state = hovered?"hovered":"normal"
    }
//    NumberAnimation {
//        properties:"anchors.topMargin"
//        easing.type: Easing.InOutQuad;
//        duration: 2000
//    }
    Item{
        anchors.fill: parent
        Rectangle{
            id:bg
            anchors.fill: parent
            anchors.margins: control.border.width*2
            radius: control.radius
            color: control.color
        }
        DropShadow {
           anchors.fill: parent
           anchors.margins: 2
           horizontalOffset:control.h_shadow
           verticalOffset: control.v_shadow
           radius: control.blur
           samples: control.samples
           color: control.shadowColor
           spread: control.spread
           source: bg
           visible: (control.containsMouse||control.alwaysShow)
        }
        Rectangle{
            id:frame
            radius: control.radius
            color: "#00000000"
            anchors.fill: parent
            anchors.margins: control.border.width
        }
    }
    Component.onCompleted: {
        defaultMargin=anchors.topMargin
    }
}
