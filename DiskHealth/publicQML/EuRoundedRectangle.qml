import QtQuick 2.12
Rectangle
{
    property var roundedMask: EuRoundedRectangle.ALL
    enum ROUNDMASK{
        LEFTTOP = 1,
        RIGHTTOP = 2,
        LEFTBOTTOM = 4,
        RIGHTBOTTOM = 8,
        ALL=15
    }
    border.width: 0
    Rectangle{
        id:lt
        visible: ((roundedMask&EuRoundedRectangle.LEFTTOP)===0)
        width: parent.radius
        height: parent.radius
        anchors.top: parent.top
        anchors.left: parent.left
        border.width: parent.border.width
        border.color: parent.border.color
        color: parent.color
        //z:1
    }
    Rectangle{
        visible: lt.visible&&parent.border.width>0
        color: lt.color
        anchors.left: lt.left
        anchors.top: lt.top
        width: lt.width
        height: lt.height
        anchors.leftMargin: lt.border.width
        anchors.topMargin: lt.border.width
        //z:1
    }

    Rectangle{
        id:rt
        anchors.right: parent.right
        anchors.top: parent.top
        visible: ((roundedMask&EuRoundedRectangle.RIGHTTOP)===0)
        width: parent.radius
        height: parent.radius
        border.width: parent.border.width
        border.color: parent.border.color
        color: parent.color
        //z:1
    }
    Rectangle{
        visible: rt.visible&&parent.border.width>0
        color: rt.color
        anchors.right: rt.right
        anchors.top: rt.top
        width: rt.width
        height: rt.height
        anchors.rightMargin: rt.border.width
        anchors.topMargin: rt.border.width
        //z:1
    }



    Rectangle{
        id:lb
        visible: ((roundedMask&EuRoundedRectangle.LEFTBOTTOM)===0)
        width: parent.radius
        height: parent.radius
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: parent.color
        border.width: parent.border.width
        border.color: parent.border.color
        //z:1
    }
    Rectangle{
        visible: lb.visible&&parent.border.width>0
        color: lb.color
        anchors.left: lb.left
        anchors.top: lb.top
        width: lb.width
        height: lb.height
        anchors.leftMargin: lb.border.width
        anchors.topMargin: -lb.border.width
        //z:1
    }



    Rectangle{
        id:rb
        visible: ((roundedMask&EuRoundedRectangle.RIGHTBOTTOM)===0)
        width: parent.radius
        height: parent.radius
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: parent.color
        border.width: parent.border.width
        border.color: parent.border.color
        //z:1
    }
    Rectangle{
        visible: rb.visible&&parent.border.width>0
        color: rb.color
        anchors.right: rb.right
        anchors.top: rb.top
        width: rb.width
        height: rb.height
        anchors.rightMargin: rb.border.width
        anchors.topMargin: -rb.border.width
        //z:1
    }
}
