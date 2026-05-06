import QtQuick.Shapes 1.14
import QtQuick 2.14
import RoundCapProgress 1.0

RoundCapProgress{
    property alias font: text.font
    property alias textColor: text.color
    property var showText: true
    property var precision: 0
    id:control
    Text {
        id:text
        visible: control.showText
        text: (control.precision>0?(control.value).toFixed(control.precision):parseInt(control.value))+"%"
        anchors.centerIn: parent
    }
}
