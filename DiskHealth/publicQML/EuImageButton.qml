import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "."
import "../publicQML/public.js" as Public
Button{
    property var resProperty:undefined
    property var resName:""
    property var spacing: 0
    property var bgGradient: undefined
    property var labelText:""
    property var staticRes:""
    property var textColor:"black"
    property var fontSize:12
    property var fontBold:false
    property var hoverUnderline: false
    property var bgColorNormal:"#00000000"
    property var bgColorOnHover:"#00000000"
    property var bgColorOnChecked:"#00000000"
    property var bgColorOnPress:"#00000000"
    property var ltRadius:false
    property var rtRadius:false
    property var lbRadius:false
    property var rbRadius:false
    property var btnRadius :0
    property var index :0
    property var busy: false
    property var fontWeight:Font.Normal
    property alias tooltip: tip.text
    property alias tooltipW: tip.implicitWidth
    property alias tooltipTriangleOffset: tip.triangleOffset
    property alias tooltipOffsetX: tip.offsetX
    property alias tooltipOffsetY: tip.offsetY
    property alias tooltipTextColor: tip.textColor
    property alias tooltipTextAlignment: tip.alignment
    property alias tooltipFontSize: tip.fontPixcelSize
    property bool pressAble: true
    property bool imghcenter: false
    property bool imgTexthcenter: false
    property var bgBorderWidth:undefined
    property var bgBorderColor:undefined
    property var textMaxWidth:undefined
    property var iconLeftMargin:16
    enabled: !busy

    signal buttonClicked(var index)
    id:control
    MouseArea{
        anchors.fill: parent
        cursorShape: hoverUnderline?Qt.PointingHandCursor:Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
    onClicked: {
        buttonClicked(index)
    }

    EuToolTip{
        id:tip
        trigger:parent
        text:""
        delay: 500
        visible:parent.hovered&&parent.enabled&&tip.text!==""
    }
    style: ButtonStyle {
        label:Text{
            //color://Public.readConfigIni("main/autoTestColor","#00000000")
            text: control.text
            visible:false// Public.readConfigIni("main/autoTest",false)==="true"
        }
         background: Rectangle{
            id:bg
            border.width:bgBorderWidth===undefined?0:bgBorderWidth
            border.color: bgBorderColor ===undefined?(control.bgColor===undefined?"#000000":control.bgColor):bgBorderColor
            anchors.verticalCenter: parent.verticalCenter
            color: control.getBgColor()
            radius: control.btnRadius!==undefined?control.btnRadius:undefined
            gradient: control.bgGradient
            EuThemeImage{
                id:image
                anchors.verticalCenter: parent.verticalCenter
                source:control.getBgSource()
                width: source===""?0:undefined
                Component.onCompleted: {
                    if(control.imghcenter){
                        anchors.horizontalCenter = parent.horizontalCenter
                    }
                }
                anchors.leftMargin: (text.text===""||control.imgTexthcenter)?undefined:control.iconLeftMargin
                anchors.left: parent.left
           }
            Text{
                id:text
                anchors.verticalCenter: parent.verticalCenter
                text: labelText
                visible: !control.busy
                font.pixelSize: control.fontSize
                font.weight: fontBold?Font.Bold:fontWeight
                wrapMode: control.textMaxWidth===undefined?Text.NoWrap:Text.WordWrap
                width: implicitWidth>control.textMaxWidth?control.textMaxWidth:undefined
                horizontalAlignment: width===undefined?undefined:Text.AlignHCenter
                color: labelText!==""?control.textColor:"#00000000"
                font.underline: (control.hoverUnderline&&control.hovered)
                anchors.left: parent.left
                anchors.leftMargin: control.spacing+image.width+image.anchors.leftMargin
                function textch(){
                    if(text.text===""){
                        var pres =control.resProperty!==undefined?control.resProperty.normal:""
                        if(pres!==""){
                            pres = pres.replace("qrc:/res/","")
                            pres = pres.replace(".png","")
                        }

                        control.text = control.resName + control.staticRes + pres
                    }else{
                        control.text = text.text
                    }

                }
                onTextChanged: {
                    textch()
                }
                Component.onCompleted: {
                    textch()
                }
            }
            Rectangle{
                color: bg.color
                width: bg.radius
                height:bg.radius
                visible: !control.ltRadius
                anchors.left: parent.left
                anchors.top: parent.top
            }
            Rectangle{
                color: bg.color
                width: bg.radius
                height:bg.radius
                visible: !control.rtRadius
                anchors.right: parent.right
                anchors.top: parent.top
            }

            Rectangle{
                color: bg.color
                width: bg.radius
                height:bg.radius
                visible: !control.lbRadius
                anchors.left: parent.left
                anchors.bottom: parent.bottom
            }

            Rectangle{
                color: bg.color
                width: bg.radius
                height:bg.radius
                visible: !control.rbRadius
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
            Component.onCompleted: {
                labelText = control.text
                var w = image.width + spacing + text.width
                control.width = w>control.width?w:control.width
                control.height = Math.max(control.height,(image.height>text.height?image.height:text.height))
                control.text = ""
            }
            Connections{
                target: control
                function onLabelTextChanged(labelText) {
                    if(labelText==="") return
                    var w = image.width + spacing + Public.textLength(text.text,text.font.pixelSize)
                    control.width = w>control.width?w:control.width

                    var bSmail = w<control.width
                    if (control.imghcenter === false && control.imgTexthcenter && bSmail)
                    {
                        image.anchors.left = bg.left
                        image.anchors.leftMargin = (control.width - w)/2
                    }
                }
            }
        }
    }
    EuRotatingWait{
        id:bi
        running: visible
        visible: parent.busy
        anchors.centerIn: parent
        width: 16
        height: 16
        property var fromcolor:"#ff3f4e68"
        z:1
    }
    function isBusy()
    {
        return busy
    }

    function getListRes(){
        var ret = ""
        do{
            if(resProperty===undefined) break
            if(!enabled&&resProperty.disabled!==undefined){
                ret = resProperty.disabled
                break
            }
            if(checked&&resProperty.checked!==undefined){
                ret = resProperty.checked
                break
            }
            if(pressed&&resProperty.pressed!==undefined){
                ret = resProperty.pressed
                break
            }
            if(hovered&&resProperty.hovered!==undefined){
                ret = resProperty.hovered
                break
            }
            if(resProperty.normal!==undefined){
                ret = resProperty.normal
                break
            }

        }while(0)
        return ret
    }

    function getBgSource(){
        var res = getListRes()
        if(res !== "") return res
        if(staticRes!=="") return "qrc:/res/"+staticRes+".png"
        if(resName === "") return ""
        res="qrc:/res/"+resName+"_"
        var state="normal"
        if(hovered) state="hover"
        if(pressed) state=pressAble?"press":"hover"
        if(!enabled) state="disable"
        res+=state
        res+=".png"
        return res
    }
    function getBgColor(){
        var ret = bgColorNormal
        if(hovered) ret = bgColorOnHover
        if(pressed) ret = bgColorOnPress
        if(checked) ret = bgColorOnChecked
        return ret
    }
}
