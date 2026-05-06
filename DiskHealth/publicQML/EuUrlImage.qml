import QtQuick 2.0
Image{
    property var info: undefined
    property var openExternalUrl: (info!==undefined&&info.url!==undefined)
    property alias hovered: ma.containsMouse
    source:(info!==undefined&&info.image!==undefined)?info.image:""
    MouseArea{
        id:ma
        anchors.fill: parent
        acceptedButtons: parent.openExternalUrl?Qt.LeftButton:Qt.NoButton
        cursorShape: (parent.openExternalUrl&&containsMouse) ? Qt.PointingHandCursor : Qt.ArrowCursor
        hoverEnabled: parent.openExternalUrl
        onClicked: {
            if(parent.openExternalUrl)
                window.openUrl(parent.info.url)
        }
    }
    onStatusChanged: {
        if(status === Image.Error){
            openExternalUrl = false
            if(info!==undefined&&info.default!==undefined)
                source = info.default
        }
    }
}
