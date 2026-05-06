import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0

import "../publicQML"

Rectangle
{
    visible: true
    id:title
    color:window.theme.style("title")//"#f4f2f7"
    property  var win:nulls
    property alias titleMsg:titleText.text
    property var closeEnable:true
    radius:4

    Rectangle
    {
        color:window.theme.style("title")//"#f4f2f7"
        z:-100
        height:4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Text
    {
        id:titleText
        font.pixelSize: 12
        color:window.theme.style("titleText")//"#4f545c"
        anchors.left:parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea
    {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        property point clickPos: "0,0"
        onPressed:
        {
            clickPos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged:
        {
            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
            win.setX(win.x + delta.x)
            win.setY(win.y + delta.y)
        }
    }

    EuImageButton
    {
        anchors.right: closeBt.left
        anchors.top:closeBt.top
        anchors.rightMargin: 8
        resName:"ico_popup_menu"
        visible:!diskInfoMgr.isMainland()
        onClicked:
        {
            ml.x = 680
            ml.y = 40
            ml.open()
        }
    }

    MultiLanguage
    {
        id:ml
    }

    function languageShort()
    {
       return ml.languageShort();
    }

    EuImageButton
    {
        enabled: closeEnable
        id:closeBt
        anchors.right:parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        resName:"ico_popup_close"
        onClicked:
        {
            if (diskInfoMgr.isTrayExist())
                win.hide();
            else
            {
                win.hide();
                diskInfoMgr.doExit();
            }
        }
    }
}
