import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0
import "../publicQML"

EuTreeView
{

    TableViewColumn {
        id:colST
        title: qsTr("ST")
        role: "status"
        width: 32
        resizable: false
        movable: false
    }
    TableViewColumn {
        id:colID
        title: qsTr("ID")
        role: "id"
        width: 32
        resizable: false
        movable: false
    }
    TableViewColumn {
        id:colAttr
        title: qsTr("Attribute")
        role: "attribute"
        width: colRaw.visible?268:198
        resizable: false
        movable: false
    }

    TableViewColumn {
        id:colRaw
        title: qsTr("RawValue")
        role: "raw"
        width: 200
        resizable: false
        movable: false
        visible:false
    }

    TableViewColumn {
        id:colCurrent
        title: qsTr("Current")
        role: "current"
        width: 100
        resizable: false
        movable: false
        visible:!colRaw.visible
    }
    TableViewColumn {
        id:colThreshold
        title: qsTr("Threshold")
        role: "threshold"
        resizable: false
        movable: false
        width:110
        visible:!colRaw.visible
    }

    rowDelegate:Rectangle{
        id:rowdelegate
        height:28
        width: parent.width
        color: styleData.row % 2 === 0?
        window.theme.style("treebg1"):window.theme.style("treebg2")
    }

    function rawMode()
    {
        colRaw.visible = true;
    }

    function fullMode()
    {
        colRaw.visible = false;
    }

    itemDelegate:
    Item
    {
        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged:
            {
                if (textMetrics.width > textMetrics.elideWidth)
                {
                    full.y =
                            mouseY + 5

                    full.visible = containsMouse
                }
            }
            z:100
        }

        EuToolTip
        {
            id:full
            text: styleData.value
            visible:false
        }

        Rectangle
        {
            id:statusRec
            width:10
            height:10
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            color:styleData.value === 1 ? "#10a91d":(styleData.value === 2?"#f59b00":"#ee0909")
            radius:5
            visible:styleData.column === 0
        }

        TextMetrics
        {
            id: textMetrics
            elide: Text.ElideMiddle
            elideWidth:colAttr.width - 3
            text:styleData.value
            font.pixelSize: 14
        }

        Text
        {
            visible:styleData.column !== 0
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 14
            width:colAttr.width
            color:
            {
                var status =diskInfoMgr.getModel().data(styleData.index, 257);
                status === 1?window.theme.style("text"):(status === 2?"#f59b00":"#ee0909")
            }

            text:textMetrics.elidedText
        }

    }



    model:diskInfoMgr.getModel()


}
