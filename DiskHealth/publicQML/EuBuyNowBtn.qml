import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import "."
import "../publicQML"

Button{
    id:buynowBtn

    property alias btnTxt: buyTxt.text
    property var btnMinWidth: 180

    width: btnMinWidth
    height: 32
    anchors.margins: 5

    background: Rectangle{
        id:bg
        radius: 4
        gradient:Gradient {
            orientation: Gradient.Horizontal
            GradientStop {position: 0.0; color: buynowBtn.pressed?"#ea8b47":(buynowBtn.hovered?"#ffa769":"#ff984d")}
            GradientStop {position: 1.0; color: buynowBtn.pressed?"#eb4044":(buynowBtn.hovered?"#ff6367":"#ff464a")}
        }
    }

    Text {
        id: buyTxt
        anchors.centerIn: parent
        text: qsTr("Buy Now")
        color: "#ffffff"
        font.pixelSize: 16
        font.bold: true
    }

    Component.onCompleted:{
        buynowBtn.width = buyTxt.width > btnMinWidth ? buyTxt.width + 2*5 : btnMinWidth;
    }
}
