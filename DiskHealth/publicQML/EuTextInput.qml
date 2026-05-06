import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.14

FocusScope {
    id: wrapper
    property alias control: input
    property alias text: input.text
    property alias hint: hint.text
    property alias prefix: prefix.text
    property alias borderColor: passRec.border.color
    property alias bgColor: passRec.color
    //property var color: "#ffffff"
    property int font_size: 14
	
    property int maxLength: 32767

    property bool bShowIcon: false
    property var iconShow: "qrc:/res/ico_zhengyan.png"
    property var iconHide: "qrc:/res/ico_biyan.png"
	property bool bPassInput:true
	property bool bActivePut:false
    property var inputValidator:undefined

	
    property var licenseLength:29
    property var licenseSeploter:"-"


    signal accepted
	signal inputFull()
	signal textEdited()

    function removeSelect(){
        control.remove(control.selectionStart,control.selectionEnd)
    }

    Rectangle {
        id:passRec
        anchors.fill: parent
        border.color: "#c1cbd6"
        color: "#ffffff"
        radius: 4

        Text {
            id: hint
            anchors { fill: parent; leftMargin: 14 }
            verticalAlignment: Text.AlignVCenter
            text: "Enter word"
            font.pixelSize: font_size
            color: "#818a99"
            opacity: input.length ? 0 : 1
        }

        Text {
            id: prefix
            anchors { left: parent.left; leftMargin: 14; verticalCenter: parent.verticalCenter }
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: font_size
            color: "#818a99"
            opacity: !hint.opacity
        }

        TextInput {
            id: input
            focus: true
            anchors { left: prefix.right; right: parent.right; top: parent.top; bottom: parent.bottom }
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: font_size
            color: "#818a99"
			maximumLength:maxLength
            selectByMouse:true
            mouseSelectionMode:TextInput.SelectCharacters
            selectedTextColor:"#ffffff"
            selectionColor:"#1B9CFF"
            clip:true
			validator: inputValidator
			property var m_bDel:false
			//onAccepted
			onTextEdited: {					
				if(bActivePut)
				{				
					var strCur = text;
					console.log("strCur",strCur)
					strCur = strCur.replace(" ","");
					while(-1 !== strCur.indexOf(licenseSeploter))
					{
						strCur = strCur.replace(licenseSeploter,"");
					}
					var iCurPos = cursorPosition-parseInt(cursorPosition/licenseLength);
					var strList=new Array();
					var strPart;
					var iCnt = 0;
					while(strCur !== ""&& (iCnt++)<5){
						strPart = strCur.substring(0,5);
						strCur = strCur.substring(strPart.length);
						strList.push(strPart);
					}
					var strNew = strList.join(strList.length>1?licenseSeploter:"");
					console.log(strNew)
					text = strNew.toUpperCase();
					var iNewPos = iCurPos+parseInt(iCurPos/licenseLength);
					console.log("iNewPos",iNewPos)
					if (!m_bDel){
						iNewPos += (strList.length>1);
					}
					cursorPosition = iNewPos;
				}
				if(text.length === licenseLength)
				{
					wrapper.inputFull()
				}
				wrapper.textEdited()
			}

			Keys.onPressed: {
				if (event.key === Qt.Key_Delete) {
					m_bDel = true;
				}else{
					m_bDel = false;
				}
			}
            Keys.onReleased: {
                if(event.modifiers & Qt.ControlModifier){
                    if ((event.key === Qt.Key_Z)){
                        if(event.modifiers & Qt.ShiftModifier){
                            input.redo()
                        }else{
                            input.undo()
                        }
                    }
                }
            }
							
            echoMode: wrapper.bShowIcon?TextInput.Normal:TextInput.Password
        }
        Button{
            id:showButton
			visible:bPassInput
            width: 16
            height: 16
            anchors.top: input.top
            anchors.topMargin: 8
            anchors.right: input.right
            anchors.rightMargin: 16
            style:ButtonStyle{
                background: Rectangle {
                    border.width: 0
                    border.color: "#ffffff"
                    radius: 4
                }
            }
            Image {
                anchors.fill: showButton.fill
                id: showImage
                source: bShowIcon?iconShow:iconHide
            }
            onClicked: {
                wrapper.bShowIcon = !wrapper.bShowIcon
            }
        }
    }

    Component.onCompleted: {
		if(false === wrapper.bPassInput)
		{
			wrapper.bShowIcon = true
		}
    }
}

