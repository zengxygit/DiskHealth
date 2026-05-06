import QtQuick 2.12
import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.14
import "."
import "public.js" as Public
ComboBox {
    id: control
    height: 32
    property var menuHeight
    property var menuItemHeight: 32
    property var resName:"ico_list_open"
    property var txtColor: "#1b1c40"
    property var txtFontSize: 14
    property var textFun: undefined
    property var placeholder: null
    property var showBorder: true
    property var showBackground: true
    property var hCenterAlign: false
    spacing: 6
    property var isEnabled: enabled&&count>0
    property var comboDisambiguation: undefined
    property var menuDisambiguation: "/publicQML/MenuTheme"

    function getBgSource(item,resName){
        var res="qrc:/res/"+resName+"_"
        var state="normal"
        if(!item.enabled||control.count<=0) state="disable"
        else if(item.hovered) state="hover"
        else if(item.pressed) state="press"
        res+=state
        res+=".png";
        return res
    }
    property var noneText: ""
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
        id:itemdelegate
        width: control.width
        leftPadding:0
        rightPadding:0
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
        contentItem: EuText {
            anchors.verticalCenter: parent.verticalCenter
            text: control.getText(index,(control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData))
            font.pixelSize: control.txtFontSize
            color: window.theme.style("item","combo",control.menuDisambiguation).enabled
            verticalAlignment: Text.AlignVCenter
            leftPadding: 10
            rightPadding: leftPadding
            width: parent.width
            textFormat: Text.RichText
        }
        highlighted: control.highlightedIndex === index
        background: Item{
                Rectangle{
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                radius: 2
                color: window.theme.style("item","background",control.menuDisambiguation)[itemdelegate.pressed?"pressed":(itemdelegate.hovered?"hovered":"normal")]
            }
        }
    }

    indicator: EuThemeImage {
        id: image
        source:control.getBgSource(image,control.resName)
        anchors.right: parent.right
        anchors.rightMargin: window.theme.style("combobox","padding",control.comboDisambiguation).right
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
        EuText {
            leftPadding: control.hCenterAlign?0:window.theme.style("combobox","padding",control.comboDisambiguation).left
            rightPadding: control.hCenterAlign?0:(control.indicator.width + control.spacing)
            anchors.horizontalCenter: control.hCenterAlign?parent.horizontalCenter:undefined
            anchors.verticalCenter: parent.verticalCenter
            text: control.placeholder ? placeholder :control.getText(control.currentIndex,control.currentText)
            font.pixelSize: control.txtFontSize
            color:  window.theme.style("item","combo",control.menuDisambiguation).enabled
            //opacity: enabled ? 1.0 : 0.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            width: control.width
            textFormat: Text.RichText
            id:t
        }
//        EuThemeImage
//        {
//            id: img
//            source:control.getBgSource(img,control.resName)
//            anchors.left: t.right
//            anchors.leftMargin: control.spacing
//            anchors.verticalCenter: parent.verticalCenter
//            property alias hovered:m.containsMouse
//            property alias pressed:m.containsPress
//            MouseArea{
//                id:m
//                anchors.fill: parent
//                hoverEnabled: true
//                acceptedButtons: Qt.NoButton
//            }
//            visible: control.hCenterAlign
//        }
    }
    state: !control.isEnabled?"disabled":(control.pressed?"pressed":(control.hovered?"hovered":"normal"))
    background: Rectangle {
        color: window.theme.style("combobox","background",control.comboDisambiguation)[control.state]
        border.color: window.theme.style("combobox","border",control.comboDisambiguation).color
        border.width: window.theme.style("combobox","border",control.comboDisambiguation).width
        radius: window.theme.style("combobox","radius",control.comboDisambiguation)
    }

    popup: Popup {
        id:popmenu
        //y: control.height
        width: control.count>0?control.width:0
        implicitHeight: menuHeight===undefined?listview.contentHeight+topPadding+bottomPadding:menuHeight
        topPadding: 4
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 4

        contentItem: ListView {
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollBar.vertical: ScrollBar {}
            Component.onCompleted: {
                if(control.menuHeight!==undefined){
                    implicitHeight = control.menuHeight-10
                }
                Public.bindBar(ScrollBar.vertical)
            }
        }

        background: Rectangle {
            color: window.theme.style("background","color",control.menuDisambiguation)
            border.color: window.theme.style("background","border",control.menuDisambiguation).color
            border.width: 1
            radius: window.theme.style("background","radius",control.menuDisambiguation)
        }
    }

    property var busy: false
    EuBusyIndicator{
        height:parent.height/2
        width:height
        running:parent.busy
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8
        visible: running
    }
}
