import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.14
import "."
TreeView{
    id:treeview
    //property alias __ma: _ma
    property var fontWeight: Font.Normal
    property var headerHeight: 28
    property var frameRoundMask: EuRoundedRectangle.ALL
    property var indentation: 0
    property var headerColor: "#e6e9ed"
    property var firstColLeftMargin: 16
    property var firstColCheckAbled: false
    property var selectAllState: Qt.Unchecked
    property var scrollBackColor:"#e6edf5"
    property var frameColor:"#e1e7ef"
    signal selectAll(var all)
    signal showContextMenu(var index)
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
        if(wasRowSelected(row,state)) return "#DCE7FA"
        else if(state === "hovered") return "#F1F5FB"
        return "#FFFFFF"
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
//        var pos = item.mapFromItem(_ma, _ma.mouseX,_ma.mouseY)
//        return item.contains(pos)
    }
//    MouseArea{
//        id:_ma
//        anchors.fill: parent
//        anchors.topMargin: headerHeight
//        hoverEnabled: true
//        acceptedButtons: Qt.LeftButton
//        onClicked: {
//            if (mouse.button === Qt.LeftButton)
//            {
//               console.log("on btn clicked", treeview.indexAt(mouse.x,mouse.y).row)
//            }


//            if(mouse.button===Qt.RightButton){
//                treeview.__listView.currentIndex = treeview.__listView.indexAt(mouse.x,mouse.y)
//                showContextMenu(indexAt(mouse.x,mouse.y))
//            }
//        }
//        onWheel: {
//            wheel.accepted = wheelEvent(wheel)
//        }
//    }
    function getFrameBW()
    {
        return 1
    }
    property var backgroundColor: "#ffffff"
    style: EuScrollViewStyleOld{
        scrollBackColor:treeview.scrollBackColor
        backgroundColor: treeview.backgroundColor
        frameColor: treeview.frameColor
        frameBW: getFrameBW()
        indentation: treeview.indentation
        branchDelegate: Item {
            width: 0
            height:0
        }
        headerDelegate: EuRoundedRectangle{
            clip: true
            roundedMask:getRoundedMask()
            height: treeview.headerHeight
            color: treeview.headerColor
            Text {
                text:styleData.value
                elide: Text.ElideRight
                anchors.left: parent.left
                anchors.leftMargin: (styleData.column===0)?treeview.firstColLeftMargin:8
                width: parent.width-2-anchors.leftMargin
                anchors.verticalCenter: parent.verticalCenter
                color: "#090a0a"
                font.pixelSize: 12
            }
            function getRoundedMask(){
                var ret = 0
                if(styleData.column===0){
                    ret = (treeview.frameRoundMask&EuRoundedRectangle.LEFTTOP)
                }else if(1){
                    ret = (treeview.frameRoundMask&EuRoundedRectangle.RIGHTTOP)
                }
                return ret
            }
        }
    }
    rowDelegate:Rectangle{
        id:rowdelegate
        height:treeview.getRowHeight(styleData.row)
        width: parent.width
        color: bgColor()
        radius:treeview.getRowRadius(styleData.row)

        function bgColor(){
            var state = "selected"
//            var ipos = rowdelegate.mapToItem(treeview,10,10)
//            var idx = treeview.indexAt(ipos.x,ipos.y)
//            var canSelected = ((treeview.model.flags(idx)&Qt.ItemIsSelectable)===Qt.ItemIsSelectable)
//            console.log("canSelected",canSelected)
//            if(!canSelected){
//                state = "normal"
//                console.log("-----------")
//            }else{
                do{
//                    if(styleData.selected) break
//                    var pos = rowdelegate.mapFromItem(treeview.__ma, treeview.__ma.mouseX,treeview.__ma.mouseY)
//                    state = rowdelegate.contains(pos)?"hovered":"normal"
                }while(0)
            //}
            return treeview.getBgColor(styleData.row,state)
        }
    }
}

