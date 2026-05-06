import QtQuick 2.12
import QtQuick.Shapes 1.12
import QtGraphicalEffects 1.12

Item {
    id:__control
    property real value: 0
    property var samples: 10
    property alias trackColor: trackPath.strokeColor
    property alias trackWidth: trackPath.strokeWidth
    property alias backgroudColor: trackPath.fillColor
    property alias barWidth: path.strokeWidth
    property alias sweepAngle: pathArc.sweepAngle
    property alias startAngle: pathArc.startAngle
    property alias capStyle: path.capStyle
    property var startColor: 'white'
    property var endColor: 'black'
    property alias text:txt
    property alias enableAnimation: behavior.enabled
    property alias showText: txt.visible
    property var precision:0

    Behavior on value{
        id:behavior
        NumberAnimation{
            duration: 500
            easing.type:Easing.OutQuart
        }
    }

    Shape{
        id:bgTrack
        smooth: true
        layer.enabled: true
        layer.samples: __control.samples
        anchors.fill: parent
        ShapePath{
            id:trackPath
            fillColor: 'transparent'
            strokeColor: 'transparent'
            strokeWidth: __control.trackWidth
            capStyle: ShapePath.RoundCap
            PathAngleArc{
                sweepAngle: 360
                startAngle: -90
                centerX: bgTrack.width/2 ; centerY: bgTrack.height/2
                radiusX: bgTrack.width/2 - trackPath.strokeWidth
                radiusY: bgTrack.height/2 - trackPath.strokeWidth
            }
        }
    }
    ConicalGradient{
        id:source
        anchors.fill: parent
        visible: false
        angle: __control.startAngle+90
        property var circleWidth: 3.1415925*2*__control.width
        property var maxAngle: (circleWidth-__control.barWidth*2)*360/circleWidth
        property var v: Math.min(__control.sweepAngle,maxAngle)
        gradient: Gradient {
            GradientStop { position: __control.barWidth/source.circleWidth; color: __control.startColor}
            GradientStop { position: source.v/360; color: __control.endColor }
            GradientStop { position: 1-__control.barWidth/2/source.circleWidth; color: __control.startColor }
        }
    }
    Shape{
        id:shapeMask
        smooth: true
        layer.enabled: true
        layer.samples: __control.samples
        visible: false
        anchors.fill: parent
        ShapePath{
            id:path
            fillColor: 'transparent'
            strokeColor: "#ffffff"
            strokeWidth: __control.barWidth
            capStyle: ShapePath.RoundCap
            PathAngleArc{
                id:pathArc
                sweepAngle: __control.value*360/100
                centerX: shapeMask.width/2 ; centerY: shapeMask.height/2
                radiusX: shapeMask.width/2 - Math.max(__control.trackWidth,__control.barWidth)
                radiusY: shapeMask.height/2 - Math.max(__control.trackWidth,__control.barWidth)
            }
        }
    }

    OpacityMask{
        source: source
        maskSource: shapeMask
        anchors.fill: shapeMask
    }
    Text {
        id: txt
        anchors.centerIn: parent
        text: __control.value.toFixed(__control.precision)+"%"
        font.pixelSize: 26
    }
}
