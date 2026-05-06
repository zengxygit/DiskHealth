import QtQuick 2.0
import QtQuick 2.12
EuGradientProgressBar {
    id:__control
    property var speed:20
    property alias color: __control.startColor
    property var sweep: 2
    property alias running: timer.running
    showText:false


    trackColor:"transparent"
    trackWidth: 0
    barWidth: width/12
    backgroudColor:"transparent"
    startAngle: speed*step
    sweepAngle: sweep*90
    endColor: '#0881ff'
    property var step:0
    Timer{
        id:timer
        running: true
        interval: 40
        repeat: true
        onTriggered: {
            var s = __control.step
            s++
            __control.step=s%(360/__control.speed)
        }
    }
}
