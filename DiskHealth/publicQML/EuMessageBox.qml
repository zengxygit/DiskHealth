import QtQuick 2.12
import QtQuick.Window 2.12
import "."
EuPopWindow{
    id:poproot
    content: mainContent
    property var mainTitle:""
    property var subTitle:""
    property var icon:"qrc:/res/ico_question48.png"
    property var result: -1
    signal accepted
    signal rejected
    title: mainTitle
    topMargin: 32
    height : Math.max(260,contentHeight+cLoader.y + 48)
    centerInParent: true
    property var contentHeight: 0
    buttonMask:EuPopWindow.CLOSEBUTTON //| EuPopWindow.HELPBUTTON
    Component{
        id:mainContent
        Rectangle{
            id:res
            property var ch: st.y+st.height
            Image {
                id:icon
                width:48
                height:48
                source:poproot.icon
                anchors.left: parent.left
                anchors.top: parent.top
            }
            Text {
                id: title
                text: poproot.mainTitle
                font.pixelSize: 20
                font.weight: Font.Medium
                color: "#090a0a"
                anchors.left: icon.right
                anchors.leftMargin: 12
                anchors.top: icon.top
                wrapMode: Text.WordWrap
                width:poproot.width-32*3-icon.width
                textFormat:Text.RichText
                verticalAlignment: Qt.AlignTop
            }
            Text {
                id: st
                text: poproot.subTitle
                width:title.width
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                color: "#4f545c"
                anchors.left: title.left
                anchors.top: title.bottom
                anchors.topMargin: 8
                textFormat:Text.RichText
                verticalAlignment: Qt.AlignTop
            }
        }
    }
    function open()
    {
        return mainframe.popMsgBox(true,0,String("%1").arg(this))
    }

    onButtonClicked: {
        poproot.visible = false
        if(index === defaultIndex){
            poproot.accepted()
        }else{
            poproot.rejected()
        }
        poproot.result = index
        mainframe.popMsgBox(false,poproot.result,String("%1").arg(this))
    }
    Connections{
        target:poproot.cLoader.item
        function onHeightChanged(){
            poproot.contentHeight = poproot.cLoader.item.ch+poproot.cLoader.item.y
        }
    }

    Component.onDestruction: {
        mainframe.popMsgBox(false,poproot.result,String("%1").arg(this))
        if(poproot.visible) poproot.rejected()
    }
}
