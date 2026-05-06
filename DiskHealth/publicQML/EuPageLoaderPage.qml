import QtQuick 2.0
import "."
import "./public.js" as Public
Item {
    property alias loader: l
    property Component content
    state: "inited"
    property var pageIndex: 0
    property var canNext: state !== "initing"
    property var canBack: true
    property var animating: true
    property var destroyOnBack: false
    signal buttonStateChanged(var b)
    signal sigNext()
    signal sigBack()
    signal sigGotoPage(var idx)
    signal sigHome()
    signal aniamtionFinished(var properties)
    function preNext(){
        return true
    }
    function preBack(){
        return true
    }
    onCanBackChanged: {
        buttonStateChanged(false)
    }

    onCanNextChanged: {
        buttonStateChanged(true)
    }

    Loader{
        id:l
        anchors.fill: parent
    }
    states: [
        State{
            name: "initing"
            PropertyChanges {
                target: l
                sourceComponent: initing
            }
        },
        State{
            name: "inited"
            PropertyChanges {
                target: l
                sourceComponent: content
            }
        }]
    Component{
        id:initing
        Item{
            EuBusyIndicator{
                visible:true
                anchors.centerIn: parent
            }
        }
    }
    Component.onCompleted: {
        Public.log("-------create------",this)
    }
    Component.onDestruction: {
        Public.log("-------destroy------",this)
    }
}
