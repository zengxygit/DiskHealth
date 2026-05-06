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
    id:cautionWidget
    flags:Qt.FramelessWindowHint|Qt.Tool
    color: "#00000000"
    width:340
    height:201+disks.height+des.height
    property var win:null

    function refreshInfo()
    {
        listModel.clear();
        var v = diskInfoMgr.getCautionDiskStatusInfo()
        for (var i = 0; i < v.length;++i)
        {
            var t = v[i];
            listModel.append({status:t})
        }

        var height = v.length*74

        if (height > 160)
            height = 160
        disks.height = height
    }

    function tryPop()
    {
        if (diskInfoMgr.popCondition())
        {
            var v = diskInfoMgr.getCautionDiskStatusInfo()
            if (v.length === 0)
                return;

            var pt = diskInfoMgr.screenSize()

            cautionWidget.refreshInfo();

            cautionWidget.x = pt.x - cautionWidget.width;
            cautionWidget.y = pt.y - cautionWidget.height - 1;

            cautionWidget.showNormal();
            cautionWidget.raise();
        }
    }

    Rectangle
    {
        anchors.fill: parent
        radius:4
        color:"#d0e7ff"

        Image
        {
            id:ico
            source:"qrc:/res/ico_logo_pallets_disksmart16.png"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 16
            anchors.topMargin: 8
        }

        Text
        {
            id: title
            text: qsTr("Caution!")
            anchors.left:ico.right
            anchors.verticalCenter: ico.verticalCenter
            anchors.leftMargin: 8
            font.pixelSize: 14
            color:"#1b1c40"
            font.weight: Font.DemiBold
        }

        EuImageButton
        {
            id:closeBt
            anchors.right:parent.right
            anchors.rightMargin: 8
            anchors.top:parent.top
            anchors.topMargin: 10
            resName:"ico_popup_close"
            onClicked:
            {
                if (remindBox.checked)
                    diskInfoMgr.quit();
                cautionWidget.hide();
            }
        }

        Rectangle
        {
            id:line
            anchors.left: parent.left
            anchors.right: parent.right
            height:1
            anchors.top: parent.top
            anchors.topMargin: 36
            color:"#bbd5f0"
        }

        Text
        {
            id: des
            text: qsTr("Your disk may have a physical failure:")
            anchors.left: ico.left
            anchors.top:line.bottom
            anchors.topMargin: 16
            anchors.right:parent.right
            anchors.rightMargin: 8
            wrapMode: Text.WordWrap
            color:"#1b1c40"
            font.pixelSize: 14
        }


        EuTreeViewOld
        {
            id:disks
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.top: des.bottom
            anchors.topMargin: 9
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
                height:styleData.row === listModel.count - 1?64:72
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

                Rectangle
                {
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
                    width:170
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

                    anchors.verticalCenterOffset: -4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16

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



            }
           }


        }

        EuRoundButtonOld
        {
            anchors.left: des.left
            anchors.bottom: viewBt.top
            anchors.bottomMargin: 10
            text:qsTr("Launch Disk Health")
            fontPixcelSize:14
            implicitWidth: 308
            implicitHeight: 32

            onClicked:
            {
                diskInfoMgr.showFromCaution();
                win.toFirstCautionIndex();
                diskInfoMgr.sigShowFrame();
            }
        }

        Rectangle
        {
            id:viewBt
            anchors.left: des.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 49
            width:308
            height:32
            radius:4
            color:"white"

            Text
            {
                id:txt
                anchors.centerIn: parent
                font.pixelSize: 14
                color:"#1f6fff"
                text:qsTr("View Solutions")
                font.weight: Font.DemiBold
            }

            MouseArea
            {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onHoveredChanged:
                {
                    if (containsMouse)
                    {
                        viewBt.color = "#d2e2ff"
                    }
                    else
                    {
                        viewBt.color = "white"
                    }
                }

                onClicked:
                {
                    diskInfoMgr.openUrl();
                    Qt.openUrlExternally(diskInfoMgr.getPopLink(win.languageShort()))
                }

                onPressed:
                {
                    viewBt.color = "#bbd3ff"
                }

                onReleased:
                {
                    viewBt.color = "#d2e2ff"
                }
            }
        }

        EuCheckBox
        {
            id:remindBox
            anchors.left: ico.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 18
            text: qsTr("Don't remind me again")
            textColor: "#090a0a"
        }



    }

}
