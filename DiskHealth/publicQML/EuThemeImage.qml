import QtQuick 2.0
import QtQuick 2.15
import "../publicQML/public.js" as Public
Item{
    id:__control__
    property var source: ""
    property alias fillMode: img.fillMode
    property alias frameCount: img.frameCount
    property alias horizontalAlignment: img.horizontalAlignment
    property alias sourceSize: img.sourceSize
    property alias status: img.status
    property alias verticalalignment: img.verticalAlignment
    property alias asynchronous: img.asynchronous
    property alias cache: img.cache
    property alias mirror: img.mirror
    property alias mipmap: img.mipmap
    property alias paintedHeight: img.paintedHeight
    property alias paintedWidth: img.paintedWidth
    property alias progress: img.progress
    property alias autoTransform: img.autoTransform
    property alias sourceClipRect: img.sourceClipRect
    property alias currentFrame: img.currentFrame
    width: Math.max(0,img.sourceSize.width)
    height: Math.max(0,img.sourceSize.height)

    Image{
        anchors.fill: parent
        id:img
        source: realSource()
        smooth: __control__.smooth
        function realSource(){
            var ret = __control__.source.toString()
            do{
                if(ret === undefined || ret === "") break
                var preStr = "qrc:/res/"
                var newSource = Public.insertStr(ret,preStr.length,window.theme.realStyleName+"/")
                if(!Public.isFileExist(newSource)) break
                ret = newSource
            }while(0)

            return ret
        }
        onSourceSizeChanged: {
            if(Image.Stretch !==fillMode) return
            if(__control__.width !== img.sourceSize.width || __control__.height!==img.sourceSize.height){
                __control__.width = img.sourceSize.width
                __control__.height = img.sourceSize.height
            }
        }
    }
}
