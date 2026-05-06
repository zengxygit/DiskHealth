var winRadius=8
var winBorderWidth=1
var winBorderColor="#e1e7ef"
var winShadowColor="#48475963"
var winShadowBlur=8
var rpInfo={
    bgColor:{normal:"#E0EAFC",hovered:"#C5D5F3",
        pressed:"#A6BDE6",disabled:"#E9ECF2",
        opacity:{enabled:1.0,disabled:1.0}
    },
    borderColor:{normal:"#1F6FFF",hovered:"#227AFF",
        pressed:"#1B63FF",disabled:"#E9ECF2"
    },
    borderWidth:0,
    textColor:{enabled:"#1F6FFF",disabled:"#B6B8BD"},
    bgRadius:4
}

var nrpInfo=
{
    bgColor:{normal:"#E0EAFC",hovered:"#C5D5F3",
        pressed:"#A6BDE6",disabled:"#E9ECF2",
        opacity:{enabled:1.0,disabled:1.0}
    },
    borderColor:{normal:"#1F6FFF",hovered:"#227AFF",
        pressed:"#1B63FF",disabled:"#E9ECF2"
    },
    borderWidth:0,
    textColor:{enabled:"#1F6FFF",disabled:"#B6B8BD"},
    bgRadius:4
}

var drpInfo={
    bgColor:{normal:"#1F6FFF",hovered:"#227AFF",
        pressed:"#1B63FF",disabled:"#E9ECF2",
        opacity:{enabled:1.0,disabled:1.0}
    },
    borderColor:{normal:"#1F6FFF",hovered:"#227AFF",
        pressed:"#1B63FF",disabled:"#E9ECF2"
    },
    borderWidth:0,
    textColor:{enabled:"#FFFFFF",disabled:"#B6B8BD"},
    bgRadius:4
}

var rplInfo={
    bgColor:{normal:"#E0EAFC",hovered:"#C5D5F3",
        pressed:"#A6BDE6",disabled:"#E9ECF2",
        opacity:{enabled:1.0,disabled:1.0}
    },
    borderColor:{normal:"#1F6FFF",hovered:"#227AFF",
        pressed:"#1B63FF",disabled:"#E9ECF2"
    },
    borderWidth:0,
    textColor:{enabled:"#1F6FFF",disabled:"#B6B8BD"},
    bgRadius:4
}
function functionText(arguments){
    var logMessages = [];
    logMessages.push.apply(logMessages, arguments)
    var args =[].slice.call(arguments);
    return args.join()
}

function log() {
    var text = functionText(arguments)
    var date = new Date()
    var d1 = new Date(date.getFullYear(),date.getMonth(),date.getDate(),date.getHours(),date.getMinutes(),date.getSeconds())
    var v = (date.getTime()-d1.getTime())
    var str = date.getFullYear()+"-"+(date.getMonth()+1)+"-"+date.getDate()+" "+date.toTimeString()+":"+parseInt(v)+"-->"+text
    console.log(str)
    if(str.length<1000) apploader.writeQMLLog(text)
}
function makeColorFont(source,color,fw,fs)
{
    var ret = "<span style=\"color:" + color
    if(fw!==undefined){
        ret +=";font-weight:" + fw
    }
    if(fs!==undefined){
        ret +=";font-size:" + fs
    }
    ret +="\">" + source + "</span>"
    return ret
}

function makeLink(text,url,linkColor){
    var c = "<style> a {text-decoration: underline;color:%1} </style><a href=%2>"+text+"</a>"
    c = c.arg((linkColor===undefined||""===linkColor)?"#037dff":linkColor).arg((url===undefined||""===url)?text:url)
    return c
}
function checkSource(state,enabled)
{
    var s = "unselect"
    switch(state)
    {
    case 1:
        s = "halfsel"
        break;
    case 2:
        s = "select"
        break;
    }
    var disabled = enabled?"":"_disable"
    var ret = "qrc:/res/check_%1%2.png"
    return ret.arg(s).arg(disabled)
}
function radioSource(checked,hovered,enabled){
    return !enabled?(checked?"qrc:/res/ico_radio_select_disable.png":"qrc:/res/ico_radio_disable.png"):(checked?"qrc:/res/ico_radio_select.png":(hovered?"qrc:/res/ico_radio_hover.png":"qrc:/res/ico_radio_normal.png"))
}

function versionNo()
{
    var date = new Date()
    var ds = publicloader.buildNo()
    var value = readConfigIni("main/versionNo","17.0.0").split(".")
    var ret={major:value[0]===undefined?0:value[0],
            minor:value[1]===undefined?0:value[1],
            revision:value[2]===undefined?0:value[2],
            buildno:ds}
    return ret
}
function stringVersionNo(buildNo){
    var ret="%1.%2.%3%4"
    var v = versionNo()
    var last="(build:%1)"
    last = (buildNo?last.arg(v.buildno):"")
    return ret.arg(v.major).arg(v.minor).arg(v.revision).arg(last)
}

function aboutVersionNo()
{
    return stringVersionNo(true)
}


function currentVersion(installversion){
    installversion = (installversion===true)
    return apploader.currentVersion(window.productRegKey,installversion)
}
function writeConfigIni(key,value)
{
    var configpath=publicloader.currentPath()
    var file=configpath+"/"+"config.ini"
    writeIni(key,file,value)
}

function readConfigIni(key,defaultv)
{
    var configpath=publicloader.currentPath()
    var file=configpath+"/"+"config.ini"
    var ret = readIni(key,file,defaultv)
    return ret
}
function readUserIni(key,defaultv){
    var file=publicloader.userDataFile(window.productRegKey)
    var ret = readIni(key,file,defaultv)
    return ret
}
function writeUserIni(key,value){
    var file=publicloader.userDataFile(window.productRegKey)
    writeIni(key,file,value)
}

function readIni(key,file,defaultv)
{
    return publicloader.readIni(key,file,defaultv)
}
function writeIni(key,file,value)
{
    publicloader.writeIni(key,file,value)
}
function makeJsonAttr(list)
{
    var length = list.length
    if(0===length) return ""
    var ret = "{%1}"
    var value=""
    for(var i = 0;i<length;i++){
        var fmt="\"%1\":\"%2\""
        var v = list[i]
        value+=fmt.arg(v.key).arg(v.value)
        if(i !== length-1){
            value +=","
        }
    }
    return ret.arg(value)
}
function makeVID()
{
    var ret
    if(buywnd !== undefined && buywnd!==null)
        ret = buywnd.makeVID(versionNo())
    return ret
}
//function makeUEWindow(w){
//    return w+"_"+makeVID()
//}

function sendUserInfo(window,action,list,source){
    //publicloader.sendUserInfo(makeUEWindow(window),action,list === undefined?"":makeJsonAttr(list),source === undefined?"":source)
    publicloader.sendUserInfo(window,action,list === undefined?"":makeJsonAttr(list),source === undefined?"":source, makeVID())
}
function sendUserInfoEx(window,action,param,source){
    //publicloader.sendUserInfo(makeUEWindow(window),action,param === undefined?"":JSON.stringify(param),source === undefined?"":source)
    var json
    if(typeof(param)  === "string"){
        if(param.length>0) json = JSON.parse(param)
    }else{
        json = param
    }

    publicloader.sendUserInfo(window,action,param === undefined?"":JSON.stringify(json),source === undefined?"":source, makeVID())
}
function sendUserInfoReverse(window,action,list,source){
    //publicloader.sendUserInfo(makeUEWindow(window),action,list === undefined?"":makeJsonAttrReverse(list),source === undefined?"":source)
    publicloader.sendUserInfo(window,action,list === undefined?"":makeJsonAttrReverse(list),source === undefined?"":source, makeVID())
}
function makeUrlParams(url,params)
{
    var c = (url.indexOf("?")>=0)?"&":"?"
    return url+c+params
}
function capacity2String(num,precision,zeroFill)
{
    if (num === 0) return zeroFill===undefined?'0 KB':zeroFill;
    var k = 1024
    var sizeStr = ['Bytes','KB','MB','GB','TB','PB','EB','ZB','YB'];
    var i = 0
    for(var l=0;l<sizeStr.length;l++){
      if(num / Math.pow(k, l) < 1){
        break
      }
      i = l
    }
    var prec = (precision===undefined?2:precision)
	var v = num / Math.pow(k, i+1)
	if(v<1&&v.toFixed(prec)>=1){
		i++
	}
    var ret = (num / Math.pow(k, i)).toFixed(prec) + " " + sizeStr[i]
    var replace = prec>0?".":""
    for(var cnt = 0;cnt<prec;cnt++){
        replace+="0"
    }
    return ret.replace(replace,"")
}

function dateFmt(date,fmt)
{
    if(date === undefined || date.getMonth === undefined) return ""
    if(fmt === undefined){
        fmt = "yyyy-MM-dd hh:mm:ss"
    }

    var o = {
     "M+" : date.getMonth()+1,
     "d+" : date.getDate(),
     "h+" : date.getHours(),
     "m+" : date.getMinutes(),
     "s+" : date.getSeconds(),
     "q+" : Math.floor((date.getMonth()+3)/3),
     "S" : date.getMilliseconds()
    };
    if(/(y+)/.test(fmt))
    fmt=fmt.replace(RegExp.$1, (date.getFullYear()+"").substr(4 - RegExp.$1.length));
    for(var k in o)
    if(new RegExp("("+ k +")").test(fmt))
    fmt = fmt.replace(RegExp.$1, (RegExp.$1.length===1) ? (o[k]) : (("00"+ o[k]).substr((""+ o[k]).length)));
    return fmt;
}

function textLength(text,fontsize){
    var tm = Qt.createQmlObject("import QtQuick 2.14;TextMetrics{}",window)
    tm.font.pixelSize = (fontsize!==undefined?fontsize:14)
    tm.text = text
    var ret = tm.boundingRect.width
    tm.destroy()
    return ret
}

function textHeight(text,fontsize){
    var tm = Qt.createQmlObject("import QtQuick 2.14;TextMetrics{}",window)
    tm.font.pixelSize = (fontsize!==undefined?fontsize:14)
    tm.text = text
    var ret = tm.boundingRect.height
    tm.destroy()
    return ret
}
function showNotInstallDriverMB(productName)
{

}
function showMessageBox(info,parent)
{
    var w = (parent === undefined? window:parent)
    var mb = Qt.createQmlObject("import \"./\";import \"../publicQML/\";EuMessageBox{visible:true;}",w)
    if(info.title!==undefined) mb.title=info.title
    if(info.mainTitle!==undefined) mb.mainTitle = info.mainTitle
    if(info.subTitle!==undefined) mb.subTitle=info.subTitle
    if(info.buttons!==undefined) mb.setButtons(info.buttons)
    if(info.icon!==undefined) mb.icon = info.icon
    if(info.defaultIdx!==undefined) mb.defaultIdx = info.defaultIdx
    if(info.height !==undefined) mb.height = info.height
    return mb.open()
}

function handleResponse(xhr, success, error) {
    if (xhr.readyState === XMLHttpRequest.DONE) {
        if (xhr.status === 200) {
            if (success !== null && success !== undefined) {
                var result = xhr.responseText
                try {
                    success(result)
                } catch (e) {
                    success(result)
                }
            }
        } else {
            if (error !== null && error !== undefined)
                var test = false
                if(test){
                    success(xhr.responseText)
                }else{
                    error(xhr.responseText, xhr.status)
                }

        }
    }
}

function logEx(e){
    var fl = functionLine(e)
    log("--++--"+fl+"--++--",functionText(arguments))
}
var cnt = 0
function urlGet(url,success,error,timeout,get)
{
    if(mainframe.isWindows10Greater()){
        var xhr = new XMLHttpRequest
        if(timeout!==undefined){
            xhr.timeout=timeout
        }else{
            xhr.timeout=5000
        }
        logEx(new Error(),"urlGet--0",String("%1").arg(timeout),String("%1").arg(get))
        var method = (get===true)?"GET":"POST"
        xhr.open(method,url,true)
        logEx(new Error(),"urlGet--1",method)
        xhr.send()
        logEx(new Error())
        xhr.onreadystatechange = function () {
            handleResponse(xhr, function(result){
                                   log(String("Get From Url:%1  [Success]\nresult:%2").arg(url).arg(""/*result*/))
                                  if(success!==undefined){success(result)}
                                },
                                function(result,code){
                                    log(String("Get From Url:%1  [Failed]\nresult:%2\ncode:%3").arg(url).arg(result).arg(code))
                                    if(error!==undefined){error(result)}}
                                )
       }
       logEx(new Error())
       return xhr
    }else{
        apploader.urlGet(url)
        if(cnt>0) return
        cnt++
        apploader.onSigUrlGetReady.connect(function(url_,ba){
            if(url!==url_){
                return
            }
            if(ba === "eu_error_occurred")
                error(ba)
            else
                success(ba)
        })
    }
}
function formatTimeTick(tt,spliter,fill){
    var hour = parseInt(tt/3600)
    var mins = parseInt(tt/60)%60
    var secs = parseInt(tt)%60
    secs = Math.max(1,secs)
    var sp = (spliter===undefined?":":spliter)
    var f = (fill === undefined?"0":fill)
    return String("%1%2%3%4%5").arg(String(hour).padStart(2, f)).arg(sp).arg(String(mins).padStart(2, f)).arg(sp).arg(String(secs).padStart(2, f))
}

function makeJsonAttrReverse(list)
{
    if (typeof(list) !== "object")
        return list;

    var length = list.length
    if(0===length) return ""
    var ret = "{%1}"
    var value=""
    var i = 0
    if (list[0].key === undefined)
    {
        for(i = 0;i<length;i++){
            value += makeJsonAttrReverse(list[i])
            if(i !== length-1){
                value +=","
            }
        }
        return ret.arg(value)
    }
    else
    {
        for(i = 0;i<length;i++){
            var v = list[i]
            var fmt=(typeof(v.value)  === "object") ?"\"%1\":%2":"\"%1\":\"%2\""
            value+=fmt.arg(v.key).arg(makeJsonAttrReverse(v.value))
            if(i !== length-1){
                value +=","
            }
        }
        return ret.arg(value)
    }

}
function getWindow(){
    var v = typeof window
    if(v !== "undefined"){
        return window
    }
    return undefined
}

function functionLine(e)
{
    var s = e.stack.split("\n")[0].split("@")[1]
    return s
}


function readFile(file){
    var request = new XMLHttpRequest()
    request.open("GET", file, false)
    request.send(null)
    var ret = request.responseText
    return ret
}

function insertStr(source, start, newStr) {
    return source.slice(0, start) + newStr + source.slice(start)
}
function isFileExist(file){
    return diskInfoMgr.fileExists(file);
}
function isNewPE()
{
    return isFileExist("X:/Windows/explorer.exe")
}
function bindHandlColor(bar){
    var ret = window.theme.style("scrollbar","handle")[bar.pressed?"pressed":(bar.hovered?"hovered":"normal")]
    return ret
}
function bindBgColor(bar){
    var ret = window.theme.style("scrollbar","background")
    return ret
}
function bindBar(bar){
    if(bar===null) return
    var handleSide = window.theme.style("scrollbar","handle").side
    var bgSide = window.theme.style("scrollbar","side")
    if(bar.contentItem){
        bar.contentItem.color = Qt.binding(function(){
            return bindHandlColor(bar)
        })
        bar.contentItem.radius = handleSide/2
    }
    bar.background = Qt.createQmlObject(String("import QtQuick 2.12;Rectangle{id:r;%1;width: %2;height: %3;}")
                                        .arg(!bar.horizontal?"anchors.horizontalCenter: parent.horizontalCenter":"anchors.verticalCenter: parent.verticalCenter")
                                        .arg(bar.horizontal?"parent.width-2;x:1":bgSide)
                                        .arg(!bar.horizontal?"parent.height-2;y:1":bgSide),bar)
    bar.background.color = Qt.binding(function(){
        return bindBgColor(bar)
    })
    bar.background.radius = bgSide/2
    bar.background.visible = Qt.binding(function(){
        return bar.contentItem.opacity>0
    })
}
