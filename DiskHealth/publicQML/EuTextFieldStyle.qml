import QtQuick 2.12
import QtQuick.Controls.Styles 1.4
TextFieldStyle{
    placeholderTextColor: "#959eb3"
    background: Rectangle {
              radius: 4
              border.color: "#40000000"
              border.width: 1
              color: control.enabled?"#ffffff":"#ededed"
          }
    textColor: "#f326344c"
    Component.onCompleted: {
        control.font.pixelSize = 14
    }
}
