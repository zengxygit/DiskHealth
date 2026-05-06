import QtQuick 2.12
import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import "."
import "../publicQML/public.js" as Public
ComboBox {
    id: control
    height: 32
    property var menuHeight
    property var menuItemHeight: 32
    property var rm: 4
    property var lm: 8
    property var txtColor: "#090a0a"
    property var txtFontSize: 14
    property var textFun: undefined
    property var showBorder: true
    property var showBackground: true
    property var hCenterAlign: false
    property var bgRadius:2
    property var noneText: ""
    property bool bEnable: true
    spacing: 6
    enabled: bEnable

//    function isEnabled(){
//        return true
//    }

    function getBgSource(item,resName){
        var res="qrc:/res/"+resName+"_"
        var state="normal"
        if(!item.enabled) state="disable"
        if(item.hovered) state="hover"
        if(item.pressed) state="press"
        res+=state
        res+=".png";
        return res
    }
    function getText(idx,v){
        var ret = v
        if(control.textFun){
            ret = control.textFun(idx,v)
        }
        if(-1 === idx){
            ret = control.noneText
        }
        return ret
    }
    delegate: ItemDelegate {
        id:delegate
        width: control.width
        height:control.menuItemHeight

        property var textForTest: null
        property var textControl: null
        Component.onCompleted: {
            if(textForTest==="") return
            var result = Public.readConfigIni("main/autoTest",false)
            if(result!=="true") return
            if(textControl===null) textControl = Qt.createQmlObject("import \"./\";import \"../publicQML/\";EuButtonForTest{anchors.fill: parent}",this)
            textControl.text = textForTest
        }

        contentItem: Rectangle{
            anchors.fill: parent
            Text {
                anchors.fill: parent
                id:tex
                text: control.getText(index,(control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData))
                font.pixelSize: control.txtFontSize
                color: control.txtColor
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                leftPadding: control.lm
                textFormat: Text.RichText

                onTextChanged:
                {
                    delegate.textForTest =  tex.text
                }

            }
            color: delegate.highlighted?"#f2f4f7":"#ffffff"
            radius: index===0?4:0
            Rectangle{
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                visible: index===0
                color: parent.color
                height: parent.radius
            }
        }
        highlighted: control.highlightedIndex === index
    }

    indicator: Image {
        id: image
        source:control.getBgSource(image,"ico_list_open")
        anchors.right: parent.right
        anchors.rightMargin: control.rm
        anchors.verticalCenter: parent.verticalCenter
        property alias hovered:ma.containsMouse
        property alias pressed:ma.containsPress
        MouseArea{
            id:ma
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
        }
        visible: !control.hCenterAlign
    }

    contentItem: Item{
        Text {
            leftPadding: control.hCenterAlign?0:control.lm
            rightPadding: control.hCenterAlign?0:(control.indicator.width + control.spacing)
            anchors.horizontalCenter: control.hCenterAlign?parent.horizontalCenter:undefined
            text: control.getText(control.currentIndex,control.currentText)
            font.pixelSize: control.txtFontSize
            color: control.txtColor
            opacity: enabled ? 1.0 : 0.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            textFormat: Text.RichText
            anchors.verticalCenter: parent.verticalCenter
            id:t
        }
        Image
        {
            id: img
            source:control.getBgSource(img,"ico_list_open")
            anchors.left: t.right
            anchors.leftMargin: control.spacing
            anchors.verticalCenter: parent.verticalCenter
            property alias hovered:m.containsMouse
            property alias pressed:m.containsPress
            MouseArea{
                id:m
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
            }
            visible: control.hCenterAlign
        }
    }

    background: Rectangle {
        color: enabled?(hovered?"#e4ebf4":(pressed?"#efe2e6":"#ebeef2")):"#dfe2e6"
        opacity: enabled?1:0.3
        radius: control.bgRadius
    }

    popup: Popup {
        id:popmenu
        y: control.height
        width: control.width
        implicitHeight: menuHeight===undefined?listview.contentHeight+topPadding+bottomPadding:menuHeight
        topPadding: 8
        leftPadding: 1
        rightPadding: 1
        bottomPadding: 8

        contentItem: ListView {
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator {
                id:msb
                width:3
                background: Rectangle{
                    color: msb.pressed?"#9aaec4":(msb.hovered?"#b1d1e7":"#b3c1d0")
                    radius: msb.width/2
                }
                visible: false
            }
            ScrollBar.vertical: ScrollBar {
                id: scrollBar
           }
            Component.onCompleted: {
                if(control.menuHeight!==undefined){
                    implicitHeight = control.menuHeight-10
                }
            }
        }

        background:
            Item
        {
            implicitWidth: popmenu.implicitWidth
            Rectangle {
                        id:bg
                       color: "#ffffff"
                       border.color: "#dde4f0"
                       border.width: 1
                       radius: 4//control.bgRadius
                       anchors.fill: parent
                   }

                   DropShadow {
                      anchors.fill: parent
                      radius: 4//control.bgRadius
                      color: "#DDE4F0"
                      source: bg
                      visible: radius>0
                   }
        }
    }
}
