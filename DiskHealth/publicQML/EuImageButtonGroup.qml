import QtQuick 2.14
import QtQuick.Controls 2.14
Rectangle {
    id:group
    color:"red"
    property var items
    property var spacing: 40
    property var fontSize: 14
    property var textColor: "#555b66"
    property var interSpacing: 8
    property var hoverUnderLine: true
    signal buttonClicked(var index)
    Component.onCompleted:{
        var lastItem
        for(var idx in items){
            var c="EuImageButton{anchors.top:group.top;\
                    anchors.verticalCenter: parent.verticalCenter;\
                    anchors.leftMargin: %1;\
                    fontSize: %2;\
                    textColor: pressed?\"#0461C4\":(hovered?\"#037DFF\":\"%3\");\
                    spacing: %4;\
                    staticRes:\"%5\";\
                    text: \"%6\";\
                    hoverUnderline:%7;}"
            c = c.arg(lastItem?spacing:0).arg(fontSize).arg(textColor).arg(interSpacing).arg(items[idx].res).arg(items[idx].text).arg(hoverUnderLine)
            var item = Qt.createQmlObject(c,group)
            if(lastItem){
               item.anchors.left = lastItem.right
            }else{
               item.anchors.left = group.right
            }
           item.index = idx
           item.buttonClicked.connect(buttonClicked)
           lastItem = item
        }
    }
}
