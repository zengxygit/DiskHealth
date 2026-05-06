import QtQuick 2.12
import "../publicQML/public.js" as Public
QtObject{
    id:thObj
    property var defaultThemes:JSON.parse(fileContent("qrc:/qml/ThemeBase.json"))
    property var realStyleName: diskInfoMgr.theme();
    property var filesCache:{"qrc:/qml/ThemeBase.json":""}
    property var systemAppStyle:thObj.realStyleName

    function fileContent(file)
    {
        var ret=""
        if(filesCache[file]!==undefined && filesCache[file]!==""){
            ret = filesCache[file]
        }else{
            if(ret==="") ret = Public.readFile(file)
            filesCache[file] = ret
        }
        return ret
    }

    function extraThemeColor(themeName,role,file,subRole)
    {
        var ret
        var str = fileContent(file)
        do{
            if(str==="") break
            var v = JSON.parse(str)[themeName]
            if(v === undefined || v[role]===undefined) break
            if(v[role]!==undefined) v=v[role]
            if(v === undefined) break
            ret = v
            if(subRole!==undefined){
                if(v[subRole]!==undefined)
                    ret = v[subRole]
            }
        }while(0)
        return ret
    }

    function style(role,subRole,disambiguation)
    {
        var url = ""
        if(disambiguation!==undefined){
            url = disambiguation
        }else{
            var arr = new Error().stack.split("\n")[1].split(":")[1].split(".")
            arr.pop()
            url = arr.join(".")
        }

        var t = extraThemeColor(realStyleName,role,url+".json",subRole)
        do{
            if(t!==undefined) break
            t = defaultThemes[realStyleName][role]

            if(t!==undefined&&subRole!==undefined&&t[subRole]!==undefined){
                t = t[subRole]
            }
        }while(0)

        return t
    }
}
