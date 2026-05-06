import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14 as NEW
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import Qt.labs.settings 1.0
import "../publicQML"

NEW.ApplicationWindow
{
    visible: true
    id:window
    width: blur === 0?840:856
    height: blur === 0?590:606
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint
    color: "#00000000"
    title:qsTr("EaseUS DiskHealth")
    property alias theme:theme
    property bool beforeRefresh:true

    property var blur:(diskInfoMgr.showWindowShadow()&&window.visibility!==4)?window.theme.style("shadow","blur"):0

    Theme
    {
        id:theme
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

    function languageShort()
    {
       return title.languageShort();
    }

    function toFirstCautionIndex()
    {
        var i = diskInfoMgr.firstCautionIndex();
        if (i !== -1)
        {
            disks.currentIndex = i;
        }
    }

    function toDiskIndex(index)
    {
        disks.currentIndex = index;
    }

    onClosing:
    {
        if (diskInfoMgr.isTrayExist())
        {
            close.accepted = false;
            window.hide();
        }
    }

    Component.onCompleted:
    {
        window.visible = diskInfoMgr.showWindow();
        diskInfoMgr.setWindowIcon(window);
        if (window.visible)
        {
            window.showNormal()
            window.flags = Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint | Qt.WindowStaysOnTopHint
            window.flags = Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint
            diskInfoMgr.refreshDisks();
        }
        else
        {
            diskInfoMgr.refreshForPop();
        }
    }

    onVisibleChanged:
    {
        autoStartItem.checked = true;
        diskInfoMgr.setVisible(visible)
    }

    HoverWidget
    {
        id:hoverItem
        visible:false
        win:window
    }

    CautionWidget
    {
        id:cautionWidget
        visible:false
        win:window
    }

    Timer
    {
        id:hideTimer
        interval:300
        repeat:true
        onTriggered:
        {
            var pos = diskInfoMgr.mousePos();

            var x = pos.x;
            var y = pos.y;

            var con = (x >= hoverItem.x && x<= hoverItem.x + hoverItem.width
                       &&y >= hoverItem.y && y<= hoverItem.y + hoverItem.height);

            if (!con)
            {
                hoverItem.hide();
                hideTimer.running = false;
            }
        }
    }

    Connections
    {
        target:diskInfoMgr
        function onSigShowFrame()
        {
            window.showNormal()
            window.flags = Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint | Qt.WindowStaysOnTopHint
            window.flags = Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint
            window.requestActivate()
        }

        function onSigExit()
        {
            diskInfoMgr.doExit();
        }

        function onSigHoverEnter(x, y)
        {
            hideTimer.stop();
            if (!hoverItem.visible && !diskInfoMgr.isBusy())
            {
                hoverItem.refreshInfo();
                hoverItem.x=x - hoverItem.width/2;
                hoverItem.y=y - hoverItem.height-10

                if (hoverItem.x < 0)
                    hoverItem.x = 0;

                if (hoverItem.y < 0)
                    hoverItem.y = 0;

                var pt = diskInfoMgr.screenSize()

                if (hoverItem.x + hoverItem.width > pt.x)
                    hoverItem.x = pt.x - hoverItem.width;

                if (hoverItem.y + hoverItem.height > pt.y)
                    hoverItem.y = pt.y - hoverItem.height;

                hoverItem.show();
                hoverItem.raise();
            }
        }

        function onSigHoverLeave()
        {
            hideTimer.start();
        }

        function onSigCheckForPop()
        {
            if (!window.visible && !cautionWidget.visible)
            {
                cautionWidget.tryPop();
            }
        }
    }

    DropShadow{
        id:shadow
        anchors.fill: parent
        anchors.margins: radius
        horizontalOffset: 0
        verticalOffset: 0
        radius: bg.anchors.margins
        color: window.theme.style("shadow","color")
        source: bg
        visible:window.blur>0
    }

    Rectangle
    {
        id:bg
        Rectangle
        {
            id:mask
            color: "#00000000"
            z:100
            anchors.fill: parent
            visible: true
            property alias showShadow:rotatItem.showShadow
            MouseArea
            {
                anchors.fill: parent
                hoverEnabled: true
            }

            RotatingWait
            {
                id:rotatItem
                anchors.centerIn: parent
            }

            clip: true

            Connections
            {
                target:diskInfoMgr
                function onSigDiskInfosChanged()
                {
                    window.beforeRefresh = false;
                    mask.visible = false;
                }

                function onSigAboutRefresh()
                {
                    mask.visible = true;
                }
            }
        }

        color:theme.style("window")
        anchors.fill:parent
        anchors.margins:window.blur
        border.color: theme.style("winBorder")
        border.width:theme.style("winBorderWidth")
        radius:window.blur?4:0

        Title
        {
            id:title
            width:parent.width
            height:32
            anchors.top:parent.top
            win:window
            clip: true
            titleMsg: qsTr("EaseUS DiskHealth")
        }

        Text
        {
            visible:!window.beforeRefresh
            id:des
            anchors.top:title.bottom
            anchors.left:parent.left
            anchors.topMargin: 16
            anchors.leftMargin: 32
            text:qsTr("Which disk do you care about?")
            font.pixelSize: 14
            color:theme.style("text")//"#090a0a"
            font.weight: Font.DemiBold
        }

        EuCombobox
        {
            id:disks
            anchors.left:des.left
            anchors.top:des.bottom
            anchors.topMargin: 8
            height:28
            width:718
            visible:!window.beforeRefresh
            model:diskInfoMgr.diskInfos

            Connections
            {
                target:diskInfoMgr
                function onSigDiskInfosChanged()
                {
                    var i = diskInfoMgr.diskIndexArg();
                    if (i !== -1)
                        disks.currentIndex = i;
                }

                function onSigSwitchDiskIndex()
                {
                    var i = diskInfoMgr.diskIndexArg();
                    if (i !== -1)
                        disks.currentIndex = i;
                }

                function onSigLanguageChanged()
                {
                    disks.onResetAllInfo();
                    disks.model = diskInfoMgr.diskInfos;
                }
            }

            function onResetAllInfo()
            {
                leftViewRpt.model = diskInfoMgr.getDiskStatusInfo(disks.currentIndex);
                var v = diskInfoMgr.getDiskStatus(disks.currentIndex);
                diskInfoMgr.loadInfoList(disks.currentIndex)
                statusItem.statusText = v.statusInfo;
                statusItem.infoText = v.life;

                switch(v.statusIndex)
                {
                case 0:
                    statusItem.bgImg = "qrc:/res/pic_status_none.png";
                    break;
                case 1:
                    statusItem.statusColor = "#2dbd8b";
                    statusItem.bgImg = "qrc:/res/pic_status_good.png";
                    break;
                case 2:
                case 3:
                    statusItem.statusColor = "#ff5327";
                    statusItem.bgImg = "qrc:/res/pic_status_bad.png";
                    break;
                }

                tempItem.statusText = v.tempValue;
                tempItem.infoText = v.tempDesc;

                switch(v.tempIndex)
                {
                case 0:
                    tempItem.bgImg = "qrc:/res/pic_temprature_none.png";
                    break;
                case 1:
                    tempItem.statusColor = "#308fff";
                    tempItem.bgImg = "qrc:/res/pic_temprature_normal.png";
                    break;
                case 2:
                case 3:
                    tempItem.statusColor = "#ff5327";
                    tempItem.bgImg = "qrc:/res/pic_temprature_warning.png";
                    break;
                }

                var b = diskInfoMgr.showRaw(disks.currentIndex);
                if (b)
                    infoList.rawMode();
                else
                    infoList.fullMode();


            }

            Connections
            {
                target:diskInfoMgr
                function onSigDiskInfosChanged()
                {
                    disks.onResetAllInfo();
                }

                function onSigIsTempFchanged()
                {
                     disks.onResetAllInfo();
                }
            }

            onCurrentIndexChanged:
            {
                diskInfoMgr.selectDisk(disks.currentIndex);
                onResetAllInfo();
            }

        }

        EuRoundButton
        {
            visible:!window.beforeRefresh
            anchors.left: disks.right
            anchors.top:disks.top
            anchors.leftMargin: 12
            implicitWidth: 46
            height:28

            EuThemeImage
            {
                anchors.centerIn: parent
                source:"qrc:/res/icon_button_refresh_blue18.png"
            }

            onClicked:
            {
                mask.visible = true;
                mask.showShadow = true;
                diskInfoMgr.clickRefresh();
                diskInfoMgr.refreshDisks();
            }
        }

        InfoItem
        {
            visible:!window.beforeRefresh
            id:statusItem
            anchors.top:disks.bottom
            anchors.topMargin: 16
            anchors.left:disks.left

            bgImg: "qrc:/res/pic_status_good.png"
            titleText:qsTr("Status")
            statusText:qsTr("Good")
            statusColor:"#2dbd8b"
            infoText: "97%"
        }

        InfoItem
        {
            visible:!window.beforeRefresh
            id:tempItem
            anchors.left:statusItem.right
            anchors.leftMargin: 8
            anchors.top:statusItem.top
            bgImg: "qrc:/res/pic_temprature_normal.png"
            titleText:qsTr("Temprature")
            statusText:"42°C"
            statusColor:"#308fff"
            infoText: qsTr("Normal")

            MouseArea
            {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onClicked:
                {
                    diskInfoMgr.isTempF = !diskInfoMgr.isTempF;
                }
            }

        }

        Rectangle
        {
            visible:!window.beforeRefresh
            id:argItems
            width:288
            height:330
            radius:4
            color:theme.style("argItemBgColor")///"#f2f4f7"
            anchors.top:statusItem.bottom
            anchors.topMargin: 8
            anchors.left:statusItem.left

            Column
            {
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 9
                anchors.bottom: parent.bottom
                spacing: 8
                Repeater
                {
                    id:leftViewRpt
                    anchors.top:parent.top
                    anchors.bottom: parent.bottom

                    Item
                    {
                        height:Math.max(argValue.height + argItem.height + 2, 17)
                        width:288
                        Text
                        {
                            id:argItem
                            font.pixelSize: 13
                            anchors.left: parent.left
                            color:theme.style("itemTitleText")//"#6e7480"
                            text:modelData.arg
                        }

                        Text
                        {
                            id:argValue
                            anchors.top: argItem.bottom
                            anchors.topMargin: 2
                            font.pixelSize: 13
                            width:150
                            wrapMode:Text.WordWrap
                            color:theme.style("text")//"#090a0a"
                            text:modelData.value
                        }
                    }

                }
            }

        }


        InfoList
        {
            id:infoList
            visible:!window.beforeRefresh
            anchors.top:statusItem.top
            anchors.left: tempItem.right
            anchors.leftMargin: 8
            width:480
            anchors.bottom: argItems.bottom
        }

        EuCheckBox
        {
            visible:!window.beforeRefresh
            id:autoStartItem
            anchors.left: argItems.left
            anchors.top: argItems.bottom
            anchors.topMargin: 20
            checked:true//diskInfoMgr.isAutoStart()
            //text: qsTr("Display alert when disk problem are found. (%1)").arg(makeColorFont(qsTr("Ask for autostart boot"), "#f59b00", 400,14))
            //textColor: "#090a0a"
        }

        Text
        {
            visible:!window.beforeRefresh
            anchors.left: autoStartItem.right
            anchors.leftMargin: 4
            anchors.top: autoStartItem.top
            anchors.topMargin: -2
            wrapMode: Text.WordWrap
            width:640
            font.pixelSize: 14
            textFormat: Text.RichText
            text: qsTr("Display alert when disk problem are found. (%1)").arg(makeColorFont(qsTr("Ask for autostart boot"), "#f59b00", 400,14))
            color: theme.style("promptcolor")//"#090a0a"
        }


        EuDefaultRoundButton
        {
            id:closeBtn
            visible:!window.beforeRefresh
            anchors.right: parent.right
            anchors.top:argItems.bottom
            anchors.rightMargin: 32
            anchors.topMargin: 16
            text:qsTr("Done")
            implicitWidth: 100
            height: 28

            onClicked:
            {
                if (diskInfoMgr.doDone(autoStartItem.checked))
                    window.hide();
                else
                    window.close();
            }
        }


    }

}
