import QtQuick 2.0
import QtGraphicalEffects 1.14
ConicalGradient {
    id: control
    width: 16
    height: 16

    property var barWidth: 3
    property var foregroundColor:"#41a8ff"
    property var backgroundColor:"#EBEEF2"
    property alias text: txt
    property var minValue: 0
    property var maxValue: 100
    property var value: 0
    property var progress: value/(maxValue-minValue)
    property var showText:false
    property var precision: 2

    smooth: true
    antialiasing: true
    source: Rectangle {
        width: control.width
        height: control.height
        color: "transparent"
        border.color: control.backgroundColor
        border.width: control.barWidth
        radius: width / 2
    }

    gradient: Gradient {
        GradientStop { position: 0.0; color: control.foregroundColor }
        GradientStop { position: control.progress; color: control.foregroundColor }
        GradientStop { position: control.progress + 0.00001; color: control.backgroundColor }
        GradientStop { position: 1.00001; color: control.backgroundColor }
    }
    Text{
        id:txt
        anchors.centerIn: parent
        text: control.showText?((control.precision>0?(control.progress*100).toFixed(control.precision):(parseInt(control.progress*100)))+"%"):""
        font.weight: Font.Normal
        font.pixelSize: 20
    }
}
