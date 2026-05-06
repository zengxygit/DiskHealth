import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import "."
import "../publicQML"
import "../publicQML/public.js" as Public

Item{
	id:activateCom
	
    property alias lisenceKey: idActive.text
    property alias borderColor: idActive.borderColor
    property alias itemWidth: idActive.width

    signal activateBtnEnabled(var enable)
    signal showErrorControl(var nRet)
	
	EuTextInput{
		id:idActive
        width: 540
        height: 28
		bPassInput:false
		maxLength:29
		bActivePut:true
		inputValidator:RegExpValidator{regExp:/[0-9a-zA-Z\-]+/}
        hint: qsTr("Enter license code here if you already have one.")
        borderColor: "#d2d4d6"
        bgColor:"#F5F8FC"

		onTextEdited:{
            var nRes = -1;
			if(idActive.text.length === (maxLength))
			{
                nRes = buywnd.checkLicense(idActive.text);
                showErrorControl(nRes)

                idActive.borderColor = (0===nRes) ? "#d2d4d6" : "#ee0909";
			}
            else
            {
                idActive.borderColor = "#d2d4d6";
                showErrorControl(0)
            }

            activateBtnEnabled(0===nRes)
		}
        MouseArea {
                id: mouseRegion
                anchors.fill: parent;
                acceptedButtons: Qt.RightButton

                onClicked: {
                    if (mouse.button === Qt.RightButton) {
                        var s  = idActive.control.selectionStart
                        var e = idActive.control.selectionEnd
                        option_menu.popup()
                        idActive.control.select(s,e)
                    }
                }
            }

            EuPopupMenu {
                id: option_menu
                implicitWidth:200
                hasTriggle:false
                EuMenuItem {
                    text: qsTr("Undo")
                    enabled:idActive.control.canUndo
                    implicitHeight: 30
                    //shortcut: "Ctrl+Z"
                    onTriggered: {
                        idActive.control.undo()
                    }
                }
                onAboutToShow:{
                    console.log("onAboutToShow")
                    idActive.control.forceActiveFocus(Qt.PopupFocusReason)
                }

                EuMenuItem {
                    text: qsTr("Redo")
                    enabled:idActive.control.canRedo
                    implicitHeight: 30
                    //shortcut: "Ctrl+Y"
                    onTriggered: {
                        idActive.control.redo()
                    }
                }

                MenuSeparator { }

                EuMenuItem {
                    text: qsTr("Cut")
                    implicitHeight: 30
                    //shortcut: "Ctrl+X"
                    onTriggered: {
                        idActive.control.cut()
                    }
                }

                EuMenuItem {
                    text: qsTr("Copy")
                    implicitHeight: 30
                    //shortcut: "Ctrl+C"
                    onTriggered: {
                        idActive.control.copy()
                    }
                }

                EuMenuItem {
                    text: qsTr("Paste")
                    implicitHeight: 30
                    enabled:idActive.control.canPaste
                    //shortcut: "Ctrl+V"
                    onTriggered: {
                        idActive.control.paste()
                    }
                }

                EuMenuItem {
                    text: qsTr("Delete")
                    implicitHeight: 30
                    onTriggered: {
                        idActive.removeSelect()
                    }
                }

                MenuSeparator { }

                EuMenuItem {
                    text: qsTr("Select All")
                    implicitHeight: 30
                    //shortcut: "Ctrl+A"
                    onTriggered: {
                        idActive.control.selectAll()
                    }
                }
        }
    }
}
