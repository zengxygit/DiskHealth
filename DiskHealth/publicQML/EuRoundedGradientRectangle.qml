import QtQuick 2.12
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import QtQuick.Shapes 1.15
Shape {
    id: __control
    layer.enabled: true
    layer.samples: 100
    layer.smooth: true
    smooth: true
    property var roundedMask: EuRoundedRectangle.ALL
    enum ROUNDMASK{
        LEFTTOP = 1,
        RIGHTTOP = 2,
        LEFTBOTTOM = 4,
        RIGHTBOTTOM = 8,
        ALL=15
    }
    property var radius: 8
    property var ltRadius: ((roundedMask&EuRoundedRectangle.LEFTTOP)===EuRoundedRectangle.LEFTTOP)?radius:0
    property var rtRadius: ((roundedMask&EuRoundedRectangle.RIGHTTOP)===EuRoundedRectangle.RIGHTTOP)?radius:0
    property var lbRadius: ((roundedMask&EuRoundedRectangle.LEFTBOTTOM)===EuRoundedRectangle.LEFTBOTTOM)?radius:0
    property var rbRadius: ((roundedMask&EuRoundedRectangle.RIGHTBOTTOM)===EuRoundedRectangle.RIGHTBOTTOM)?radius:0
    property alias gradient: shapePath.fillGradient
    property alias color: shapePath.fillColor

    ShapePath {
        id:shapePath
        strokeColor: "transparent"

        startX: 0; startY: __control.ltRadius
        PathArc {
            x: __control.ltRadius; y: 0
            radiusX: __control.ltRadius; radiusY: __control.ltRadius
            useLargeArc: false
        }
        PathLine {
            x: __control.width - __control.rtRadius; y: 0
        }
        PathArc {
            x: __control.width; y: __control.rtRadius
            radiusX: __control.rtRadius; radiusY: __control.rtRadius
            useLargeArc: false
        }
        PathLine {
            x: __control.width; y: __control.height - __control.rbRadius
        }
        PathArc {
            x: __control.width - __control.rbRadius; y: __control.height
            radiusX: __control.rbRadius; radiusY: __control.rbRadius
            useLargeArc: false
        }
        PathLine {
            x: __control.lbRadius; y: __control.height
        }
        PathArc {
            x: 0; y: __control.height - __control.lbRadius
            radiusX: __control.lbRadius; radiusY: __control.lbRadius
            useLargeArc: false
        }
        PathLine {
            x: 0; y: __control.ltRadius
        }
    }
}
