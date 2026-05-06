import QtQuick.Controls 2.14
import QtQuick 2.14
import "../publicQML/public.js" as Public
RadioButton{
    id:control
    property var textColor: "#1b1c40"
    property var fontSize: 14
    property var fontWeight: Font.Normal
//    indicator: Rectangle {
//          implicitWidth: 14
//          implicitHeight: implicitWidth
//          x: control.leftPadding
//          y: parent.height / 2 - height / 2
//          radius: implicitWidth/2
//          border.color: (control.hovered || control.checked) ? "#0079ff" : "#9aa6b3"

//          Rectangle {
//              anchors.centerIn: parent
//              width: 6
//              height: width
//              radius: width/2
//              color: parent.border.color
//              visible: control.checked
//          }
//      }
    indicator:Image{
        anchors.verticalCenter: parent.verticalCenter
        anchors.topMargin: control.topPadding
        anchors.leftMargin: control.leftPadding
        source: Public.radioSource(control.checked,control.hovered,control.enabled)
    }

    contentItem: Text {
        text: control.text
        font.pixelSize: control.fontSize
        color: control.textColor
        font.weight: control.fontWeight
        opacity: enabled ? 1.0 : 0.3
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
        elide: Text.ElideRight
        width: control.width-leftPadding-12
    }
}
