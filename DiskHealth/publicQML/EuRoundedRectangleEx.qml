import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0
import QtQuick.Shapes 1.14

Shape{
    id:r
    property var radius: 4
    property alias strokeWidth: path.strokeWidth
    property alias strokeColor: path.strokeColor
    property alias fillGradient: path.fillGradient
    property alias fillColor: path.fillColor
    property alias color: path.fillColor
    property alias joinStyle: path.joinStyle
    property var roundedMask: EuRoundedRectangleEx.ALL
    enum ROUNEDMASK{
        LEFTTOP = 1,
        RIGHTTOP = 2,
        LEFTBOTTOM = 4,
        RIGHTBOTTOM = 8,
        ALL = 15
    }

    ShapePath{
        id:path
        joinStyle:ShapePath.MiterJoin
        startX: lt.radiusX>0?r.radius:0
        startY: 0
        strokeWidth:0

        PathArc{
            id:lt
            x:0
            y:r.radius
            radiusX: ((roundedMask&EuRoundedRectangleEx.LEFTTOP) === EuRoundedRectangleEx.LEFTTOP) ? r.radius:0
            radiusY: radiusX
            direction: PathArc.Counterclockwise
        }

        PathLine{
            x: 0
            y: r.height-(lb.radiusX>0?r.radius:0)
        }

        PathArc{
            id:lb
            x:r.radius
            y:r.height
            radiusX: ((roundedMask&EuRoundedRectangleEx.LEFTBOTTOM) === EuRoundedRectangleEx.LEFTBOTTOM) ? r.radius:0
            radiusY: radiusX
            direction: PathArc.Counterclockwise
        }

        PathLine{
            x:r.width-(rb.radiusX>0?r.radius:0)
            y:r.height
        }

        PathArc{
            id:rb
            x:r.width
            y:r.height-r.radius
            radiusX: ((roundedMask&EuRoundedRectangleEx.RIGHTBOTTOM) === EuRoundedRectangleEx.RIGHTBOTTOM) ? r.radius:0
            radiusY: radiusX
            direction: PathArc.Counterclockwise
        }

        PathLine{
            x:r.width
            y:rt.radiusX>0?r.radius:0
        }

        PathArc{
            id:rt
            x:r.width-r.radius
            y:0
            radiusX: ((roundedMask&EuRoundedRectangleEx.RIGHTTOP) === EuRoundedRectangleEx.RIGHTTOP) ? r.radius:0
            radiusY: radiusX
            direction: PathArc.Counterclockwise
        }

        PathLine{
            x:lt.radiusX>0?r.radius:0
            y:0
        }
//        fillGradient: LinearGradient {
//            x1:0
//            y1:0
//            x2:0
//            y2:r.height
//            GradientStop{
//                position: 0
//                color: "#ff0000"
//            }
//            GradientStop{
//                position: 1
//                color: "#ffff00"
//            }
//        }
    }
}
