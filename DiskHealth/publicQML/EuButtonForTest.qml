import QtQuick 2.14
import QtQuick.Controls 2.14
import "../publicQML/public.js" as Public
import "../publicQML"
Label{
    id:control
    Component.onCompleted: {
        control.color = Public.readConfigIni("main/autoTestColor","#00000000")
        control.z = Public.readConfigIni("main/z",-1)
    }
}
