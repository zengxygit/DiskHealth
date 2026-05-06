import QtQuick 2.6
Rectangle{
    width: 30
    height: 16
    radius:width/2
    signal clicked
    signal pressed
    property bool switchOn: true
    color: switchOn?"#037dff":"#bcd0e5"
    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            parent.switchOn=!parent.switchOn
            parent.clicked()
        }
        onPressed: parent.pressed()
    }
    Rectangle{
        height: parent.height-2*2
        width: height
        radius: height/2
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: parent.switchOn?parent.width-width-2:2
        anchors.left: parent.left
    }
}
