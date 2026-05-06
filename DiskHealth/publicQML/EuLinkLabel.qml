import QtQuick 2.14
Text {
    font.pixelSize: 14
    textFormat: Text.RichText
    property var autoOpen: true
    onLinkActivated:{
        if(autoOpen) window.openUrl(link)
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        onPressed: {
            parent.linkActivated(parent.linkAt(mouse.x,mouse.y))
        }
    }
}
