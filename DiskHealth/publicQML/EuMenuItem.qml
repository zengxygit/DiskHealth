import QtQuick 2.12
import QtQuick.Controls 2.14
MenuItem {
    implicitHeight: 32
    id:control
    background:Rectangle{
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        radius: 2
        color: window.theme.style("item","background","/publicQML/MenuTheme")[control.pressed?"pressed":(control.hovered?"hovered":"normal")]
    }
    contentItem:Control{
        id:r
        //leftPadding: window.theme.style("item","padding","/publicQML/MenuTheme")
        //rightPadding: leftPadding
        property var spaceing: 8
        Item{
            id:img
            anchors.left: parent.left
            anchors.leftMargin: ch.source!==""?0:window.theme.style("item","padding","/publicQML/MenuTheme")
            anchors.top: parent.top
            height: parent.height
            width: last.x+last.width
            visible: ch.source!==""||ico.source!==""
            EuThemeImage{
                id:ch
                visible: control.checked
                source: control.checkable?"qrc:/res/ico_language_gou.png":""
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }
            EuThemeImage{
                id:ico
                source: control.subMenu!==null?control.subMenu.icon:control.icon.source
                anchors.left: ch.right
                anchors.leftMargin: (ch.source!==""&&source!=="")?r.spaceing:0
                anchors.verticalCenter: parent.verticalCenter
            }
            Item{
                id:last
                anchors.left: ico.right
                width: r.spaceing
            }
        }

        EuText {
            anchors.left: img.right
            anchors.leftMargin: img.visible?0:r.spaceing
            anchors.right: arrow.left
            anchors.rightMargin:arrow.source!==""?r.rightPadding:0
            text: control.text
            font.pixelSize: 14
            color: window.theme.style("item","text","/publicQML/MenuTheme")[enabled?"enabled":"disabled"]
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
        }
        EuThemeImage{
            id:arrow
            source: subMenu!==null?(control.enabled ? "qrc:/res/ico_list_right_normal.png":"qrc:/res/ico_list_right_disable.png") : ""
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin:r.rightPadding
        }
    }
    indicator:Item{}
    arrow:Item{}
}
