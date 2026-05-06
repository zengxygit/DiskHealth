import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0

import "../publicQML"

EuPopupMenu {
    id:langmenu
    title: qsTr("Language")
    //overlap: (window.blur>0?1:-1)*Public.winShadowBlur
    itemHeight:32
    implicitWidth: 150
    property list<QtObject> languages:[
        QtObject{property var full:"Danish";property var short_:"da-DK";property var display:"Dansk"},
        QtObject{property var full:"German";property var short_:"de-DE";property var display:"Deutsch"},
        QtObject{property var full:"English";property var short_:"en-US";property var display:"English"},
        QtObject{property var full:"Spanish";property var short_:"es-ES";property var display:"Español"},
        QtObject{property var full:"French";property var short_:"fr-FR";property var display:"Français"},
        QtObject{property var full:"Italian";property var short_:"it-IT";property var display:"Italiano"},
        QtObject{property var full:"Dutch";property var short_:"nl-NE";property var display:"Nederlands"},
        QtObject{property var full:"Polish";property var short_:"pl-PL";property var display:"Polski"},
        QtObject{property var full:"Portuguese";property var short_:"pt-BR";property var display:"Português"},
        QtObject{property var full:"Arabic";property var short_:"ar-SA";property var display:"العربية"},
        QtObject{property var full:"Thai";property var short_:"th-TH";property var display:"ไทย"},
        QtObject{property var full:"Japanese";property var short_:"ja-JP";property var display:"日本語";property font font: Qt.font({family:"MS UI Gothic"})},
        QtObject{property var full:"ChineseTrad";property var short_:"zh-TW";property var display:"繁體中文"},
        //QtObject{property var full:"ChineseSimp";property var short_:"zh-CN";property var display:"简体中文";property font font: Qt.font({family:"Microsoft YaHei"})},
        QtObject{property var full:"Korean";property var short_:"ko-KR";property var display:"한국어"}]

    function languageShort()
    {
        var ret = "en-US";
        for (var i = 0; i < languages.length; ++i)
        {
            if (languages[i].full === diskInfoMgr.currentLanguage)
            {
                ret = languages[i].short_;
                break;
            }
        }

        return ret;
    }

    function resetLanguageActions(){
        var a = langmenu.takeAction(0)
        while(a!==null){
            langmenu.removeAction(a)
            a = langmenu.takeAction(0)
        }

        for(var i = 0;i<languages.length;i++){
            var v = languages[i]
            var item = Qt.createQmlObject("
                import \"../publicQML/public.js\" as Public;
                import QtQuick.Controls 2.14;
                Action{
                    property var value;
                    text:value.display;
                    checked:diskInfoMgr.currentLanguage===value.full
                    checkable:true;
                    onTriggered:{
                        diskInfoMgr.currentLanguage=value.full;
                    }
                }",langmenu)
            item.value = v
            langmenu.addAction(item)
        }
    }

    Component.onCompleted: {
        resetLanguageActions()
    }
}
