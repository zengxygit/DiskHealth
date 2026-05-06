import QtQuick 2.0
import "."
import QtQuick.Controls 2.14
EuRoundedRectangle{
    id:__control
    property var indicator: __indicator
    property alias indicators: __column
    property alias views: __pageLoader
    property alias currentIndex: btnGroup.currentIndex
    clip: true
    property var busy: false
    signal update()
    color: window.theme.style("window")
    radius: window.blur
    roundedMask: EuRoundedRectangle.LEFTBOTTOM|EuRoundedRectangle.RIGHTBOTTOM
    property alias contentBgColor: loaderBg.color
    function getLabel(mdata)
    {
        return mdata.text
    }
    Component{
        id:__btnMark
        Item{}
    }
    function showItemMark(btnId){
        return true
    }
    function itemMarkCom(btnId){
        return __btnMark
    }

    Component{
        id:__indicator
        Item{
            width: 230
            height: 34
            id:__item
            property alias button: __btn
            property var mData: null
            EuImageButton{
                id:__btn
                property var btnId: 0
                enabled: __control.busy?btnGroup.isCurrent(__btn):true
                spacing: 10
                iconLeftMargin:10
                btnRadius:4
                ltRadius:true
                rtRadius:true
                lbRadius:true
                rbRadius:true
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                bgColorNormal:window.theme.style("button").normal
                bgColorOnHover:window.theme.style("button").hovered
                bgColorOnPress:window.theme.style("button").pressed
                bgColorOnChecked:window.theme.style("button").checked

                fontSize:
                {
                    multilanguage.currentLanguage === "Japanese"?12:14
                }
                Loader{
                    anchors.fill: parent
                    sourceComponent: __control.itemMarkCom(__btn.btnId)
                    id:il
                    onLoaded: {
                        __control.update()
                    }

                    Connections{
                        target: __control
                        function onUpdate() {
                            if(il.item===null) return
                            var b = __control.showItemMark(__btn.btnId)
                            if(il.item.willPromote!==undefined)
                                il.item.willPromote = b
                            else
                                il.item.visible = b
                        }
                    }
                }

                fontWeight:(pressed||checked)?Font.Medium:Font.Normal
                textColor:{
                    if(!enabled){
                        window.theme.style("buttonText").disabled
                    }else if(checked){
                        window.theme.style("buttonText").pressed
                    }else if(pressed){
                        window.theme.style("buttonText").pressed
                    }else{
                        window.theme.style("buttonText").normal
                    }
                   }
            }
        }
    }

    QtObject{
        id:btnGroup
        property var buttons: []
        property var currentIndex: -1
        function addButton(button,id){
            buttons.push(button)
            button.clicked.connect(function(){
                if(button.checked===undefined||button.checked) return
                button.checked = true
                for(var i = 0;i<buttons.length;i++){
                    if(buttons[i]!==button){
                        buttons[i].checked = false
                    }else{
                        if(btnGroup.currentIndex !== i)
                            btnGroup.currentIndex = i
                        __pageLoader.gotoPage(i)
                    }
                }
            })
        }
        function isCurrent(btn){
            if(currentIndex===-1) return false
            return (buttons[currentIndex].btnId === btn.btnId)
        }

        onCurrentIndexChanged: {
            var btn = btnGroup.buttons[currentIndex]
            if(btn !== null&&btn!==undefined)
                btn.clicked()
        }
    }

    EuRoundedRectangle{
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        roundedMask: EuRoundedRectangle.LEFTBOTTOM
        radius:window.blur
        color: parent.color
        width: __column.width
        id:__funBg
        Column{
            id:__column
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Repeater{
                id:__column_repeater
                Loader{
                    height: item.height
                    width: item.width
                    id:__inLoader
                    sourceComponent: __control.indicator
                    Component.onCompleted: {
                        if(__inLoader.item.mData!==undefined) __inLoader.item.mData = modelData
                        __inLoader.item.button.labelText = Qt.binding(function(){return __control.getLabel(modelData)})
                        if(modelData.icon!==undefined) __inLoader.item.button.resName = Qt.binding(function(){return modelData.icon})
                        if(modelData.staticRes!==undefined) __inLoader.item.button.staticRes = Qt.binding(function(){return modelData.staticRes})
                        if(modelData.resProperty!==undefined) __inLoader.item.button.resProperty = Qt.binding(function(){return modelData.resProperty})
                        if(__inLoader.item.button.btnId!==undefined) __inLoader.item.button.btnId = modelData.id
                        btnGroup.addButton(__inLoader.item.button,modelData.id)
                    }
                }
                Component.onCompleted: {
                    model = Qt.binding(function(){return views.pages})
                    var idx = __control.currentIndex
                    if(-1 === idx) idx = 0
                    __column_repeater.itemAt(idx).item.button.clicked()
                }
            }
        }

    }
    EuRoundedRectangle{
        id:loaderBg
        color: 'white'
        roundedMask: EuRoundedRectangle.LEFTTOP|(parent.radius>0?EuRoundedRectangle.RIGHTBOTTOM:0)
        radius: parent.radius>0?parent.radius:8
        anchors.left: __funBg.right
        anchors.right: parent.right
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        EuPageLoader{
            id:__pageLoader
            autoClean: false
            anchors.fill: parent
            onIndexChanged: {
                if(btnGroup.currentIndex !== index)
                    btnGroup.currentIndex = index
            }
        }
    }
}
