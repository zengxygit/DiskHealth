import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0
import "../publicQML"

EuThemeImage
{
    id:infoItem
    property var bgImg: null
    property var titleText:null
    property var statusText:null
    property var statusColor:null
    property var infoText: null
    source:bgImg
    width: 140
    height:70

    Text
    {
        id:title
        anchors.left: parent.left
        anchors.top:parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 4
        font.pixelSize: 12
        text:infoItem.titleText
        color:theme.style("itemTitleText")//"#6e7480"
    }

    MouseArea
    {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        visible:text.width > text.elideWidth
        onHoveredChanged:
        {
            if (text.width > text.elideWidth)
            {
                if (!full.visible)
                    full.y =mouseY + 5
                full.visible = containsMouse
            }
        }
        z:100
    }

    EuToolTip
    {
        id:full
        text: infoItem.statusText
        visible:false
        z:100
    }

    TextMetrics
    {
        id:text
        font.pixelSize: 16
        font.weight: Font.DemiBold
        text: infoItem.statusText
        elideWidth:80
        elide: Text.ElideRight
    }

    Text
    {
        id:status
        font.pixelSize: 16
        font.weight: Font.DemiBold
        anchors.left: title.left
        anchors.top: title.bottom
        anchors.topMargin: 6
        text:text.elidedText
        color:infoItem.statusColor
    }

    Text
    {
        id:info
        anchors.left: status.left
        anchors.top: status.bottom
        anchors.topMargin: 2
        font.pixelSize: 12
        font.weight: Font.DemiBold
        text:infoItem.infoText
        color:theme.style("text")
    }


}
