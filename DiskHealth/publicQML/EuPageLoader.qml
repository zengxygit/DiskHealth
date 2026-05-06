import QtQuick 2.0
import QtQuick.Controls 1.4
import "./public.js" as Public

Item {
    id:root
    property int index: 0
    property var pages:[]
    property var items: []
    property var forword: true
    property var pageAniDuration: 400
    property var canNext: true
    property var canBack: true
    property alias currentItem: stackview.currentItem
    property var animating: stackview.busy||timer.running
    property var isHorTransition: true
    property var destroyOnPop: false
    property var loadAllOnInit: false
    property var autoClean: pages.length>2
    signal sigAboutToCreatePages()
    clip:true
    onPagesChanged: {
        //timer.running = true
        //timer.list = items
        items =[]
        load()
    }

    function createItem(source,sIdx){

        var item = Qt.createComponent(source.url)
        var ret = root.destroyOnPop?item:item.createObject()

        if(source.destroyOnBack!==undefined&&ret.destroyOnBack!==undefined){
            ret.destroyOnBack =  source.destroyOnBack
        }

        if(ret.pageIndex!==undefined){
            ret.pageIndex =  sIdx
        }
        if(ret.animating!==undefined){
            ret.animating = Qt.binding(function(){
                return root.animating&&ret.visible
            })
        }
        if(ret.sigGotoPage!== undefined){
            ret.sigGotoPage.connect(function(idx){
                root.gotoPage(idx)
            })
        }
        if(ret.sigHome!== undefined){
            ret.sigHome.connect(function(){
                root.returnHome()
            })
        }
        if(ret.canNext !== undefined){
            canNext = ret.canNext
            if(ret.buttonStateChanged!== undefined){
                ret.buttonStateChanged.connect(function(b){
                    if(b) canNext = ret.canNext
                })
            }
            if(ret.sigNext!== undefined){
                ret.sigNext.connect(function(){
                    if (!ret.animating){
                        var cannotNext = (ret.preNext!==undefined&&!ret.preNext())
                        if(!cannotNext){
                            root.next()
                        }
                    }
                })
            }
        }
        if(ret.canBack !== undefined){
            canBack = ret.canBack
            if(ret.buttonStateChanged!== undefined){
                ret.buttonStateChanged.connect(function(b){
                    if(!b) canBack = ret.canBack
                })
            }
            if(ret.sigBack!== undefined){
                ret.sigBack.connect(function(){
                    if (!ret.animating){
                        var cannotBack = (ret.preBack!==undefined&&!ret.preBack())
                        if(!cannotBack){
                            root.back()
                        }
                    }
                })
            }
        }

        return ret
    }

    function load(){
        if(loadAllOnInit){
            sigAboutToCreatePages()
        }

        for(var i = 0;i<pages.length;i++){
            if(loadAllOnInit){
                var sIdx = (pages[i].index === undefined?i:pages[i].index)
                items.push(createItem({url:pages[i].source,destroyOnBack:pages[i].destroyOnBack},sIdx))
            }
            else
                items.push(null)
        }
    }
    Timer{
        id:timer
        interval: pageAniDuration*2; running: false; repeat: false
        property var list: []
        onTriggered: {
            for(var i = 0;i<list.length;i++){
                var item = list[i]
                if(item!==null){
                    Public.log("---will destroy---",item)
                    item.destroy()
                }
            }
            list = []
            running = false
        }
    }

    StackView{
        id:stackview
        anchors.fill: parent

        delegate:StackViewDelegate {
            function getTransition(properties)
            {
                return root.isHorTransition?hTransition:verticalTransition
            }
            function transitionFinished(properties)
            {
                if(properties.enterItem!==null&&properties.enterItem!==undefined){
                    if(properties.enterItem.aniamtionFinished!==undefined){
                        properties.enterItem.aniamtionFinished(properties)
                    }
//                    if(properties.enterItem.animating!==undefined){
//                        properties.enterItem.animating = false
//                    }
                }
            }
            property Component hTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "x"
                    from: target!==null?(!root.forword?-target.width:target.width):800
                    to: 0
                    duration: root.pageAniDuration
                    easing.type: Easing.InOutQuart
                }
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1.0
                    duration: root.pageAniDuration
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: target!==null?(root.forword?-target.width:target.width):800
                    duration: root.pageAniDuration
                    easing.type: Easing.InOutQuart
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: 1.0
                    to: 0
                    duration: root.pageAniDuration
                }
            }
            property Component verticalTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "y"
                    from: !root.forword?-target.height:target.height
                    to: 0
                    duration: root.pageAniDuration
                    easing.type: Easing.InOutQuart
                }
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1.0
                    duration: root.pageAniDuration
                }
                PropertyAnimation {
                    target: exitItem
                    property: "y"
                    from: 0
                    to: root.forword?-target.height:target.height
                    duration: root.pageAniDuration
                    easing.type: Easing.InOutQuart
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: 1.0
                    to: 0
                    duration: root.pageAniDuration
                }
            }
        }
    }
    Component.onCompleted: {
        //load()
    }

    function next(){
        if(!canNext||pages.length===0) return
        var curIdx = index
        curIdx++
        curIdx %= pages.length
        gotoPage(curIdx,true)
    }
    function back(){
        if(!canBack||pages.length===0) return
        var curIdx = index
        curIdx += pages.length
        curIdx--
        curIdx %= pages.length
        gotoPage(curIdx,false)
    }

    function cleanItems(idx)
    {
        if(idx===0){
            if(autoClean){
                timer.running = true
                timer.list = items
                items=[]
                load()
            }
        }
    }

    function gotoPageById(id){
        var idx = id
        for(var i = 0;i<pages.length;i++){
            if(pages[i].id === id){
                idx = i
                break
            }
        }
        gotoPage(idx)
    }
    function returnHome()
    {
        index = 2
        gotoPage(0)
    }

    function gotoPage(idx,direction)
    {
        if(pages.length<=0||idx>=pages.length||isNaN(idx)) return
        if(index === idx && null !== items[index]){
            stackview.replace(items[index])
            return
        }

        if(direction!==undefined){
            forword = direction
        }else{
            forword = (idx>index)
        }
        if(index<items.length){
            var lastItem = items[index]
            if(lastItem!==null && lastItem.destroyOnBack&&!forword){
                timer.list.push(lastItem)
                items.splice(index,1,null)
                timer.running = true
            }
        }

        cleanItems(idx)
        var item = items[idx]
        if(item === undefined) return
        if(item === null){
            var sIdx = (pages[idx].index === undefined?idx:pages[idx].index)
            item = createItem({url:pages[idx].source,destroyOnBack:pages[idx].destroyOnBack},sIdx)
            items.splice(idx,1,item)
            item = items[idx]
        }
        index = idx
        stackview.replace(item)
    }
}
