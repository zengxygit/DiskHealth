import QtQuick 2.14
import "./public.js" as Public
Text {
    property var textWidth: sub.width
    property var w: this.Window
    property var canPopup: w===undefined?true:!w.subWinPopuped
    property alias tipVisible: tip.visible
    property alias tooltipWidth: tip.implicitWidth
    property alias tooltipHeight: tip.implicitHeight
    property alias origalText: sub.text
    font.family: window.font.family
    MouseArea{
        id:_ma
        anchors.fill:parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }
    EuToolTip{
        id:tip
        text:parent.text
        visible: parent.truncated&&_ma.containsMouse&&parent.canPopup
    }
    Text{
        id:sub
        visible: false
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        font.weight:parent.font.weight
    }
    elide: Text.ElideRight
}
