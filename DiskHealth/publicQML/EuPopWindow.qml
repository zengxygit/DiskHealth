import QtQuick 2.12
import QtQuick.Window 2.14
import "."
import QtQuick.Layouts 1.14
import "../publicQML/public.js" as Public
import QtQuick.Controls 1.4

ApplicationWindow {
    width: 640
    height: 200
    id:popupwin
    //modality: Qt.ApplicationModal
    flags:  Qt.FramelessWindowHint|Qt.Window
    color: "#00000000"
    property Component content
    property var buttons:[]
    property var buttonsEnable:null
    property int defaultIndex: 0
    property int leftMargin: 32
    property int rightMargin: 32
    property int topMargin: 24
    //title: titleLabel.text
    signal buttonClicked(int index,var sender)
    property alias contentColor: popwinRootRect.color
    property var btnWidth: 100
    property var btnHeight: 28
    property alias states: popwinRootRect.states
    property alias state: popwinRootRect.state
    property var titleHeight: 32
    property alias bgColor: popwinRootRect.color
    property alias titleColor: winTitle.color
    property var haveBorder: true
    property var centerInParent: true
    property var showParentMask: true
    property var buttonMask: EuPopWindow.CLOSEBUTTON //| EuPopWindow.HELPBUTTON
    property var busy: false
    property alias radius: popwinRootRect.radius
    property alias titleRightMargin: closeBtn.anchors.rightMargin
    property var bottomHeight: popwinRootRect.height-layout.y
    modality: Qt.ApplicationModal
    property var blur: window.blur
    property alias border: popwinRootRect.border
    property alias cLoader: loader
    property var eventLoopKey: ""

    property var helpName: ""
    property var btnObjs: []

    function indexButton(index){
        var ret
        if(index<btnObjs.length)
            ret = btnObjs[index]
        return ret
    }

    enum BUTTONMASK{
        CLOSEBUTTON = 1,
        MAXBUTTON = 2,
        HELPBUTTON = 4
    }
    Rectangle{
        id:popwinRootRect
        anchors.fill: parent
        border.width: haveBorder?window.theme.style("winBorderWidth"):0
        border.color: window.theme.style("winBorder")
        radius: blur>0?Public.winRadius:0
        color: "#ffffff"
        property var borderw: (blur>0&&border.width===0)?0:window.theme.style("winBorderWidth")
        Rectangle{
            height: titleHeight-parent.borderw
            anchors.left: parent.left
            anchors.leftMargin:parent.borderw
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: parent.borderw
            anchors.rightMargin: parent.borderw
            color: window.theme.style("title")
            id:winTitle
            radius: parent.radius
            Rectangle{
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                height: parent.radius
                color: parent.color
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                property point clickPos: "0,0"
                property var dblclicked: false
                onPressed: {
                    dblclicked = false
                    if(popupwin.visibility===4||popupwin.visibility===5) return
                    clickPos = Qt.point(mouse.x, mouse.y)
                }

                onPositionChanged: {
                    if(popupwin.visibility===4||popupwin.visibility===5 || dblclicked) return
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                    popupwin.setX(popupwin.x + delta.x)
                    popupwin.setY(popupwin.y + delta.y)
                }
                onDoubleClicked: {
                    if(maxBtn.visible){
                        dblclicked = true
                        maxBtn.clicked()
                    }
                }
            }
            EuImageButton
            {
               anchors.rightMargin: 12
               resName:"ico_popup_close"
               anchors.right:parent.right
               anchors.verticalCenter:parent.verticalCenter
               id:closeBtn
               onClicked: popupwin.close()
               enabled: !popupwin.busy
               visible: ((popupwin.buttonMask&EuPopWindow.CLOSEBUTTON) === EuPopWindow.CLOSEBUTTON)
            }

            EuImageButton
            {
               anchors.rightMargin: 12
               resName:"ico_popup_question"
               anchors.right:maxBtn.visible ? maxBtn.left : closeBtn.left
               anchors.verticalCenter:parent.verticalCenter
               id:helpBtn
               onClicked:
               {
                   if (popupwin.helpName.length)
                   {
                       var url = networkConfigMgr.getString("url_help_subpage")
                        window.openUrl(url+String("?=")+popupwin.helpName)
                   }
               }
               enabled: !popupwin.busy
               visible: ((popupwin.buttonMask&EuPopWindow.HELPBUTTON) ===EuPopWindow.HELPBUTTON)
            }

            EuImageButton
            {
               anchors.rightMargin: 12
               resName:(popupwin.visibility===4||popupwin.visibility===5)?"ico_popup_reduction":"ico_popup_maximize"
               anchors.right:closeBtn.left
               anchors.verticalCenter:parent.verticalCenter
               id:maxBtn
               visible: ((popupwin.buttonMask&EuPopWindow.MAXBUTTON) ===EuPopWindow.MAXBUTTON)
               onClicked: {
                   if(popupwin.visibility===4||popupwin.visibility===5){
                      popupwin.showNormal()
                   }else{
                        popupwin.showMaximized()
                   }
               }
            }
            Text{
                id:titleLabel
                text:popupwin.title
                font.pixelSize: 14
                color: window.theme.style("text")
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 16
            }
        }
        Loader{
            id:loader
            sourceComponent: content
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: winTitle.bottom
            anchors.bottom: parent.bottom
            anchors.topMargin: popupwin.topMargin
            anchors.leftMargin: popupwin.leftMargin
            anchors.rightMargin: popupwin.rightMargin
            anchors.bottomMargin: layout.height+parent.borderw
            onLoaded: {
                if(item.color!==undefined)item.color=popwinRootRect.color
            }
        }
        RowLayout{
            id:layout
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            spacing: 20
            width: parent.width
        }
    }

    onButtonsChanged: {
        setButtons(buttons)
    }

    onButtonsEnableChanged: {
        for(var i = 1;i <layout.children.length;i++){
            var object = layout.children[i];

            if (popupwin.buttonsEnable !== null)
                object.enabled =popupwin.buttonsEnable[i-1]
        }
    }

    function open(){
        popupwin.visible = true
    }
    function close(){
        popupwin.destroy()
    }
    function addDefaultExpander(buttonlist){
        var hasExpander = false
        for(var idx = 0;idx<buttonlist.length;idx++){
            if(buttonlist[idx].addExpander===true){
                hasExpander = true
                break
            }
        }
        if(!hasExpander){
            Qt.createQmlObject("import QtQuick 2.12;import QtQuick.Layouts 1.14;Item{Layout.fillWidth: true}",layout)
        }
    }

    function setButtons(buttonlist){
        for(var i = 1;i <layout.children.length;i++){
            var object = layout.children[i];
            object.destroy()
        }
        btnObjs = []
        var obj=null
        addDefaultExpander(buttonlist)
        for(var idx = 0;idx<buttonlist.length;idx++){
            var s
            if(idx === defaultIndex||buttonlist[idx].isDefaultBtn === true){
                s = "EuDefaultRoundButton{property int index:0;onClicked:popupwin.buttonClicked(index,this)}"
            }else{
                s = "EuRoundButton{property int index:0;onClicked:popupwin.buttonClicked(index,this)}"
            }
            obj = Qt.createQmlObject(s,layout)

            if (popupwin.buttonsEnable !== null)
            {
                obj.enabled = popupwin.buttonsEnable[idx]
            }

            obj.index = idx;
            obj.text = (buttonlist[idx].text===undefined?buttonlist[idx]:buttonlist[idx].text)
            obj.height = popupwin.btnHeight
            obj.implicitWidth = popupwin.btnWidth
            obj.Layout.preferredWidth = obj.width
            obj.Layout.alignment=Qt.AlignRight
            obj.Layout.preferredHeight = obj.height
            btnObjs.push(obj)

            if(buttonlist[idx].addExpander===true){
               Qt.createQmlObject("import QtQuick 2.12;import QtQuick.Layouts 1.14;Item{Layout.fillWidth: true}",layout)
                if(idx === 0){
                    obj.Layout.leftMargin=popupwin.leftMargin
                }
            }
        }
        if(obj!==null){
            obj.Layout.rightMargin=popupwin.rightMargin
        }
        layout.visible = buttonlist.length>0
    }

    onVisibleChanged: {
        if(window.notifyPopup!== undefined) window.notifyPopup(visible)
        if(showParentMask) window.popupWin(this,visible)
        if(visible&&!centerInParent){
            popupwin.x=(window.x+(window.width-popupwin.width)/2)
            popupwin.y=(window.y+window.titleHeight+blur)
        }
    }
    Component.onCompleted: {
        eventLoopKey = String("%1%2").arg(this).arg(new Date().getTime().toString())
    }

    Component.onDestruction:{
        if(window.notifyPopup!== undefined) window.notifyPopup(false)
        if(showParentMask) window.popupWin(this,false)
    }
}
