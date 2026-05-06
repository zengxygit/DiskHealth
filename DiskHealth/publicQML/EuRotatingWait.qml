import QtQuick 2.14
import QtGraphicalEffects 1.14
Item {
    property var fromcolor:"#ff6377D3"
    property var tocolor: "#006377D3"
    property var borderw: 2
    property alias duration: ani.duration
    property alias running: ani.running
    id:control
    Rectangle{
        id:rect
        color: "#00000000"
        radius: width/2
        border.width: borderw
        anchors.fill: parent
        visible: false
    }
    ConicalGradient{
        anchors.fill: parent
        source:rect
        gradient: Gradient{
            GradientStop{
                position: 1.0;color: fromcolor
            }
            GradientStop{
                position: 0.0;color: tocolor
            }
        }
        Rectangle{
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            width: rect.border.width
            height: width
            radius: width/2
            color: fromcolor
            border.width: 0
        }

        RotationAnimator on rotation {
            id:ani
            running: control.visible
            loops: Animation.Infinite
            from: 0
            to:360
            duration: 1000
        }
    }

}
