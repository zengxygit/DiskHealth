import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.14
import "public.js" as Public
Button{
    height: 32
    property var fontPixcelSize: 16
    property var bgGradient
    property var backgroundValue: window.theme.style("background",type)
    property var semiBold: true
    property var rp: 0
    property var lp: 0
    property var staticButton: false
    property var busy: false
    property var type:"normal"
    implicitWidth: 140
    id:control
    enabled: !busy

    function stateColor(c){
        var ret="#00000000"
        if(c!==undefined){
            if(!enabled){
                ret = c.disabled
            }else if(pressed){
                ret = c.pressed
            }else if(hovered){
                ret = c.hovered
            }else{
                ret = c.normal
            }
        }
        return ret
    }
    function enabledValue(c){
        var ret
        if(c!==undefined){
            ret = (enabled?c.enabled:c.disabled)
            if(!enabled){
                ret = c.disabled
            }else{
                if(c.enabled.pressed!==undefined&&pressed){
                    ret = c.enabled.pressed
                }else if(c.enabled.hovered!==undefined&&hovered){
                    ret = c.enabled.hovered
                }else if(c.enabled.normal!==undefined){
                    ret = c.enabled.normal
                }else{
                    ret = c.enabled
                }
            }
        }
        return ret
    }

    style:ButtonStyle{
        label:Text{
            color:control.enabledValue(window.theme.style("text",type))
            font.pixelSize: control.fontPixcelSize
            font.weight: semiBold?Font.Medium:Font.Normal
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            rightPadding: control.rp
            leftPadding: control.lp
            text: control.text
            visible: !control.busy
        }
        background: Rectangle{
            height: control.height
            radius: window.theme.style("radius",type)
            opacity: control.enabledValue(backgroundValue.opacity)
            border.color:control.stateColor(window.theme.style("border",type).color)
            border.width:window.theme.style("border",type).width
            color: control.stateColor(backgroundValue)
            gradient: control.bgGradient
        }
    }
    MouseArea{
        anchors.fill: parent
        cursorShape: !staticButton?Qt.PointingHandCursor:Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
    TextMetrics{
        id:tm
        font.pixelSize: control.fontPixcelSize
        font.weight: control.semiBold?Font.Medium:Font.Normal
        text: control.text
        onTextChanged:{
            control.reCalcWidth()
        }
    }
    EuRotatingWait{
        id:bi
        running: visible
        visible: parent.busy
        anchors.centerIn: parent
        width: 16
        height: 16
        z:1
    }
    function reCalcWidth()
    {
        var min = tm.boundingRect.width+2*10 +control.rp+control.lp
        control.width = Math.max(min,control.implicitWidth)
    }

    onWidthChanged: {
        reCalcWidth()
    }
    onImplicitWidthChanged: {
        reCalcWidth()
    }
}
