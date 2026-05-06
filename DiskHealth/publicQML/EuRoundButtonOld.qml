import "../publicQML/public.js" as Public
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.14
EuRoundButtonLegacy{
    property var bgg:Gradient{
                orientation: properties.gradient===undefined?Gradient.Horizontal:properties.gradient.orientation
                GradientStop { position: 0 ; color: getColor(0)}
                GradientStop { position: 1 ; color: getColor(1)}
            }
    bgGradient:(enabled&&properties.gradient!==undefined)?bgg:undefined
    function getColor(position){
        if(properties.gradient===undefined) return "#00000000"
        var color = properties.gradient.stops[position]
        var ret = pressed?color.pressed:(hovered?color.hovered:color.normal)
        return ret
    }
    Component.onCompleted: {
        properties =Public.drpInfo
    }
}
