import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0
import "../publicQML"

Window
{
    id:hoverItem
    flags:Qt.FramelessWindowHint|Qt.Tool|Qt.WindowStaysOnTopHint
    color: "#00000000"
    width:340
    height:74+disks.height
    property var win: null

    function refreshInfo()
    {
        listModel.clear();
        var v = diskInfoMgr.getDiskInfos()
        for (var i = 0; i < v.length;++i)
        {
            var t = diskInfoMgr.getDiskStatus(i);
            listModel.append({status:t})
        }

        var height = v.length*74
        if (height > 208)
            height = 208
        disks.height = height
    }

    Rectangle
    {
        anchors.fill: parent
        radius:4
        gradient: Gradient
        {
            orientation: Gradient.Vertical
            GradientStop { position: 0 ; color: "#d0e7ff"}
        }


        Text
        {
            id:title
            text:qsTr("Disk Health is guarding your disk health.")
            color:"#1b1c40"
            font.pixelSize: 16
            font.weight: Font.DemiBold
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }

        EuTreeViewOld
        {
            id:disks
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.top: title.bottom
            anchors.topMargin: 16
            anchors.right:parent.right
            anchors.rightMargin: 2
            //scrollBackColor:"#d0e7ff"
            //frameColor:"#d0e7ff"
            backgroundColor: "transparent"
            headerVisible:false
            headerHeight:0

            TableViewColumn {
                id:colST
                title: qsTr("ST")
                role: "status"
                width: 32
                resizable: false
                movable: false
            }

            rowDelegate:Rectangle{
                id:rowdelegate
                height:listModel.count >1 &&styleData.row === listModel.count - 1?64:72
                width: parent.width
                color: "#d0e7ff"
            }


            model:
            ListModel
            {
                id:listModel
            }

            itemDelegate:Item
            {
                Rectangle
                {



                width:310
                height:64
                Rectangle
                {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    width: 4
                    height:40
                    color:styleData.value.statusIndex === 1?"#10bd1c":(styleData.value.statusIndex===2?"#F59B00":"#EE0909")
                    radius:4
                    z:1
                }
                MouseArea
                {
                    anchors.fill: bg
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked:
                    {
                        win.toDiskIndex(styleData.row);
                        diskInfoMgr.showFrame();
                    }
                    z:100
                }
                Rectangle
                {
                    id:bg
                    anchors.left: parent.left
                    anchors.leftMargin: 2
                    width:308
                    height:64
                    color:"white"
                    radius: 4
                    z:-1
                }

                Image
                {
                    source:"qrc:/res/ico_disk_32.png"
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 14
                    id:image
                }

                Text
                {
                    font.pixelSize: 14
                    color:"#2b2e33"
                    font.weight: Font.DemiBold
                    text:qsTr("Disk %1").arg(styleData.value.diskIndex)
                    anchors.left:image.right
                    anchors.leftMargin: 8
                    anchors.top: parent.top
                    anchors.topMargin: 14
                }

                Text
                {
                    font.pixelSize: 12
                    color:"#6e7480"
                    text:styleData.value.diskDes
                    width:127
                    elide: Text.ElideRight
                    anchors.left:image.right
                    anchors.leftMargin: 8
                    anchors.top: parent.top
                    anchors.topMargin: 37
                }

                Rectangle
                {
                    id:status
                    width:statusText.width+8
                    height:20
                    color:styleData.value.statusIndex === 1?"#caf8db":(styleData.value.statusIndex===2?"#f8f0ca":"#fadad7")
                    radius:11

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -4
                    anchors.right: temp.left
                    anchors.rightMargin: 4

                    Text
                    {
                        id:statusText
                        anchors.centerIn: parent
                        color:styleData.value.statusIndex === 1?"#0fb51b":(styleData.value.statusIndex===2?"#f59b00":"#ee0909")
                        font.pixelSize: 12
                        text:styleData.value.statusInfo
                        font.weight: Font.DemiBold
                    }

                }

                Rectangle
                {
                    width:46
                    height:20
                    color:styleData.value.tempIndex === 1?"#C8E4FF":"#f8d9ca"
                    radius:11
                    id:temp
                    anchors.verticalCenterOffset: -4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right:parent.right
                    anchors.rightMargin: 16

                    Text
                    {
                        anchors.centerIn: parent
                        color:styleData.value.tempIndex === 1?"#1F6FFF":"#fa6b02"
                        font.pixelSize: 12
                        text:styleData.value.tempValue
                        font.weight: Font.DemiBold
                    }

                }


            }
           }


        }




    }

}
