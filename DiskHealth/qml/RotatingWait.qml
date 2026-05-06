import QtQuick 2.12
import QtQuick.Window 2.12
import "."
import "../publicQML"
import "../publicQML/public.js" as Public
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14

Item{
    anchors.centerIn: parent
    property var showShadow: false
    id:control
    width: 72
    height: 72
    Rectangle{
        color: window.theme.style("window")
        width: 72
        height: 72
        radius: 8
        id:bg
    }
    EuThemeImage{
        id:img
        z:100
        source: "qrc:/res/loading00.png"
        anchors.centerIn: parent
        RotationAnimator on rotation {
            running: visible
            loops: Animation.Infinite
            duration: 1500
            from: 0
            to: 360
        }
    }

    DropShadow{
      anchors.fill: parent
      radius: 8//root.radius
      color: "#2b000000"
      source: bg
      visible:control.showShadow
    }

    Component.onDestruction:{

    }
}
