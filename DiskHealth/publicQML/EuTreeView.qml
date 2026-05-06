import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.14
import "."
TreeView{
    id:treeview
    property var fontWeight: Font.Normal
    property var headerHeight: 28
    property var frameRoundMask: EuRoundedRectangle.ALL
    property var indentation: 0
    property var firstColLeftMargin: 40
    property var firstColCheckAbled: false
    property var selectAllState: Qt.Unchecked
    signal selectAll(var all)
    signal showContextMenu(var index)
    property var headerValue: window.theme.style("header")
    property var headerColor: headerValue["background"]
    property var headerText: headerValue["text"]
    property var headerSpliter: headerValue["spliter"]
    function getRowRadius(row){
        return 0
    }

    function getRowHeight(row){
        return 40
    }
//    function firstColumnCheckStateFun(curState){
//        if (curState === Qt.Checked)
//            return Qt.Unchecked
//        else
//            return Qt.Checked
//    }

    function wasRowSelected(row,state){
        return state === "selected"
    }

    function getBgColor(row,state){
        var _state = state
        if(wasRowSelected(row,state)) _state = "selected"
        else if(state === "hovered") _state = "hovered"
        return window.theme.style("background",_state)
    }

    function oncollapseindex(idx){
        var rows = model.rowCount(idx)
        for(var i = 0;i<rows;i++){
            var subIdx = model.index(i,0,idx)
            if(isExpanded(subIdx)){
                oncollapseindex(subIdx)
                collapse(subIdx)
            }
        }
        collapse(idx)
    }

    function wheelEvent(wheel){
        return false
    }


    function doDoubleClicked(index)
    {
        if(isExpanded(index)){
            oncollapseindex(index)
        }else{
            expand(index)
        }
    }

    onDoubleClicked:{
        doDoubleClicked(index)
    }
    function wasHovered(item){
        var pos = item.mapFromItem(__mouseArea, __mouseArea.mouseX,__mouseArea.mouseY)
        return item.contains(pos)
    }
    function getHeaderHeight()
    {
        return headerVisible?headerHeight:0
    }
    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: {
            wheel.accepted = treeview.wheelEvent(wheel)
        }
    }

//    MouseArea{
//        id:_ma
//        //anchors.fill: parent
//        width: 0
//        height: 0
//        anchors.topMargin: treeview.getHeaderHeight()
//        hoverEnabled: true
//        acceptedButtons: Qt.NoButton
////        onClicked: {
////            var ptlv = treeview.__listView.mapFromItem(_ma,mouse.x,mouse.y)
////            var lvIndex = treeview.__listView.indexAt(ptlv.x,ptlv.y)
////            treeview.__listView.currentIndex = lvIndex
////            var pttv = treeview.mapFromItem(_ma,mouse.x,mouse.y)
////            console.log("----------",ptlv,pttv,_ma.mouseX,_ma.mouseY)
////            if(mouse.button===Qt.RightButton){
////                var tvIndex = treeview.indexAt(pttv.x,pttv.y)
////                console.log("----ptlv----",lvIndex,tvIndex)
////                treeview.showContextMenu(tvIndex)
////            }
////        }
//        onWheel: {
//            wheel.accepted = wheelEvent(wheel)
//        }
//    }
//    onClicked: {
////        var ptlv = treeview.__listView.mapFromItem(__mouseArea,__mouseArea.mouseX,__mouseArea.mouseY)
////        var lvIndex = treeview.__listView.indexAt(ptlv.x,ptlv.y)
////        treeview.__listView.currentIndex = lvIndex
////        var pttv = treeview.mapFromItem(__mouseArea,__mouseArea.mouseX,__mouseArea.mouseY)
////        console.log("----------",ptlv,pttv,__mouseArea.mouseX,__mouseArea.mouseY)
////        //if(mouse.button===Qt.RightButton){
////            var tvIndex = treeview.indexAt(pttv.x,pttv.y)
////            console.log("----ptlv----",lvIndex,tvIndex)
////            treeview.showContextMenu(tvIndex)
////        //}
//        treeview.showContextMenu(index)
//    }

    property var rightClicked: false
    Connections{
        target: __mouseArea
        function onPressed(mouse){
            treeview.rightClicked = (mouse.button === Qt.RightButton)
        }
        function onWheel(wheel) {
            wheel.accepted = treeview.wheelEvent(wheel)
        }
    }
    function preClicked(index){}
    onClicked:{
        treeview.preClicked(index)
        if(rightClicked) showContextMenu(index)
    }

    function getFrameBW()
    {
        return window.theme.style("treeview","border").width
    }
    property var backgroundColor: window.theme.style("treeview","background")
    property var branchDelegate: Component {
        Item{
            width: 0
            height:0
            visible: false
        }
    }
    style: EuScrollViewStyle{
        backgroundColor: treeview.backgroundColor
        borderColor:window.theme.style("treeview","border").color
        frameBW: getFrameBW()
        indentation: treeview.indentation
        branchDelegate: treeview.branchDelegate
        headerDelegate: EuRoundedRectangle{
            clip: true
            roundedMask:getRoundedMask()
            height: treeview.getHeaderHeight()
            color: treeview.headerColor
            Text {
                text:styleData.value
                elide: Text.ElideRight
                anchors.left: parent.left
                anchors.leftMargin: (styleData.column===0)?treeview.firstColLeftMargin:8
                width: parent.width-2-anchors.leftMargin
                anchors.verticalCenter: parent.verticalCenter
                color: treeview.headerText
                font.pixelSize: 14
            }
            Rectangle{
                visible:(styleData.column<=0?false:(treeview.getColumn(styleData.column-1).resizable))
                width:1
                height: 16
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                color: treeview.headerSpliter
                id:spliter
            }
            function getRoundedMask(){
                var ret = 0
                if(styleData.column===0){
                    ret = (treeview.frameRoundMask&EuRoundedRectangle.LEFTTOP)
                }else if(!spliter.visible){
                    ret = (treeview.frameRoundMask&EuRoundedRectangle.RIGHTTOP)
                }
                return ret
            }
        }
    }
    rowDelegate:Item{
            height:treeview.getRowHeight(styleData.row)
            Rectangle{
                id:rowdelegate
                height:parent.height
                color: bgColor()
                radius:treeview.getRowRadius(styleData.row)
                width: parent.width/2-6
                function bgColor(){
                    var state = "selected"
                    do{
                        if(styleData.selected||styleData.pressed) break
                        var pos = rowdelegate.mapFromItem(treeview.__mouseArea, treeview.__mouseArea.mouseX,treeview.__mouseArea.mouseY)
                        state = (treeview.__mouseArea.containsMouse&&rowdelegate.contains(pos))?"hovered":"normal"
                    }while(0)
                    return treeview.getBgColor(styleData.row,state)
                }
        }
    }

    Component.onCompleted: {
        __mouseArea.acceptedButtons = (Qt.LeftButton|Qt.RightButton)
        __mouseArea.hoverEnabled = true
    }
}

