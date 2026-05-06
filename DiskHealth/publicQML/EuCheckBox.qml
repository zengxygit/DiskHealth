import QtQuick 2.14
import QtQuick.Controls 2.14
import "../publicQML/public.js" as Public

CheckBox{
    id:control
    height: 16
    property var textColor: window.theme.style("checkbox")
    property var fontSize: 14
    property var fontWeight: Font.Normal
    property var textElide: Text.ElideNone
    property var textWidth
    property var truncated: false
    leftPadding:0
    rightPadding:0
    spacing:control.text.length>0?6:0
    indicator: Item{
        implicitWidth: 14
        implicitHeight: implicitWidth
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        EuThemeImage{
            anchors.fill: parent
            x: control.leftPadding
            anchors.centerIn: parent
            source: Public.checkSource(control.checkState,control.enabled)
        }
    }
    contentItem: Text {
      text: control.text
      font.pixelSize: control.fontSize
      opacity: enabled ? 1.0 : 0.3
      color: control.textColor
      font.weight: control.fontWeight
      elide: control.textElide
      verticalAlignment: Text.AlignVCenter
      leftPadding: control.indicator.width + control.spacing
      Component.onCompleted: {
          if(control.textWidth!==undefined){
            width = control.textWidth
          }
      }
      onTruncatedChanged: {
          control.truncated=truncated
      }
    }
}
