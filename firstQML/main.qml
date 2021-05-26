import QtQuick 2.14
import QtQuick.Controls 2.12
import "logic.js" as Logic

ApplicationWindow {
    id: window1
    visible: true
    width: 600
    height: 620
    color: "darkgrey"
    footer: ToolBar {
        id: statusbar
        height: 15
        Label {
            id: foot
        }
    }
    signal changeA()
    Label {
        id: labelLog
        objectName: "labelLog"
        x: 0
        y: 0
        height: 50
        width: 500
        text: "opa"
    }
    Dial {
        id: dial
        x: 545
        y: 0
        visible: true
        width: 55
        height: 50
        wheelEnabled: true
        onValueChanged: {
            var val = dial.value
            foot.text = val
            if(val === 1) {
                window1.close()
            } else if (val > 0.5 && val < 0.6) {
                //Logic.newA(1);
                //changeA()
            } else {
                //Logic.newA(2);
                //changeA()
            }
        }
    }

//! _____________________ VOLTAGE _____________________________________
    Frame {
        id: frameVoltage
        x: 5
        y: 70
        width: 267
        height: 354

        Frame {
            id: frameVoltageMean
            x: 10
            y: 40
            width: 145
            height: 35

            Label {
                id: labelVoltageMean
                objectName: "labelVoltageMean"
                color: "blue"
                x: 0
                y: 0
                text: qsTr("0")
            }
        }

        Label {
            id: labelMean
            color: "blue"
            x: 10
            y: 10
            text: qsTr("Действующее")
            font.pixelSize: 20
            font.family: "Arial"
            renderType: Text.QtRendering
            textFormat: Text.RichText
        }

        Frame {
            id: frameVoltageAmplitude
            x: 10
            y: 115
            width: 145
            height: 35

            Label {
                id: labelVoltageAmplitude
                objectName: "labelVoltageAmplitude"
                color: "blue"
                x: 0
                y: 0
                text: qsTr("0")
            }
        }
        Label {
            id: labelAmplitude
            x: 24
            y: 85
            color: "blue"
            text: qsTr("Амплитудное")
            font.pixelSize: 20
            font.family: "Arial"
            renderType: Text.QtRendering
            textFormat: Text.RichText
        }
        Label {
            id: labelVoltageState
            objectName: "labelVoltageState"
            x: 3
            y: 212
            width: 160
            height: 40
            color: "#03034e"
            FontLoader { id: localFont; source: Qt.resolvedUrl("voltage.ttf"); }
            font.family: localFont.name
            text: qsTr("ПЕРЕМЕННОЕ")
            font.pixelSize: 30
            //font.family: "Times New Roman"
            renderType: Text.QtRendering
            textFormat: Text.RichText
        }
    }
//! ___________________  AMPERAGE _________________________________________________
    Frame {
        id: frameAmp
        x: 284
        y: 70
        width: 300
        height: 354

        Frame {
            id: frameAmperage
            x: 10
            y: 40
            width: 145
            height: 36

            Label {
                id: labelAmperage
                objectName: "labelAmperage"
                x: 0
                y: 0
                color: "#e40909"
                text: qsTr("0")
            }
        }

        Label {
            id: label2
            x: 10
            y: 10
            color: "#e40909"
            text: qsTr("Ток")
            font.family: "Arial"
            renderType: Text.QtRendering
            textFormat: Text.RichText
            font.pixelSize: 20
        }

        Frame {
            id: frame_mA
            x: 10
            y: 82
            width: 150
            height: 193

            Button {
                id: button_5mA
                objectName: "button_5mA"
                signal signal_5mA()
                x: 10
                y: 10
                width: 60
                height: 30
                text: qsTr("5 mA")
                onClicked: signal_5mA()
            }
            CheckBox {
                id: checkBox_5mA
                objectName: "checkBox_5mA"
                x: 80
                y: 10
                width: 30
                height: 30
                checked: false
                enabled: false
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: checkBox_5mA.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 5
                    border.color: checkBox_5mA.down ? "#17a81a" : "red"
                    Rectangle {
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: checkBox_5mA.down ? "#17a81a" : "red"
                        visible: checkBox_5mA.checked
                    }
                }
            }
            Button {
                id: button_10mA
                objectName: "button_10mA"
                signal signal_10mA()
                x: 10
                y: 50
                width: 60
                height: 30
                text: qsTr("10 mA")
                onClicked: signal_10mA()
            }
            CheckBox {
                id: checkBox_10mA
                objectName: "checkBox_10mA"
                x: 80
                y: 50
                width: 30
                height: 30
                checked: false
                enabled: false
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: checkBox_10mA.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 5
                    border.color: checkBox_10mA.down ? "#17a81a" : "red"
                    Rectangle {
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: checkBox_10mA.down ? "#17a81a" : "red"
                        visible: checkBox_10mA.checked
                    }
                }
            }
            Button {
                id: button_20mA
                objectName: "button_20mA"
                signal signal_20mA()
                x: 10
                y: 90
                width: 60
                height: 30
                text: qsTr("20 mA")
                onClicked: signal_20mA()
            }
            CheckBox {
                id: checkBox_20mA
                objectName: "checkBox_20mA"
                x: 80
                y: 90
                width: 30
                height: 30
                checked: false
                enabled: false
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: checkBox_20mA.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 5
                    border.color: checkBox_20mA.down ? "#17a81a" : "red"
                    Rectangle {
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: checkBox_20mA.down ? "#17a81a" : "red"
                        visible: checkBox_20mA.checked
                    }
                }
            }
            Button {
                id: button_50mA
                objectName: "button_50mA"
                signal signal_50mA()
                x: 10
                y: 130
                width: 60
                height: 30
                text: qsTr("50 mA")
                onClicked: signal_50mA()
            }
            CheckBox {
                id: checkBox_50mA
                objectName: "checkBox_50mA"
                x: 80
                y: 130
                width: 30
                height: 30
                checked: false
                enabled: false
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: checkBox_50mA.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 5
                    border.color: checkBox_50mA.down ? "#17a81a" : "red"
                    Rectangle {
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: checkBox_50mA.down ? "#17a81a" : "red"
                        visible: checkBox_50mA.checked
                    }
                }
            }
        }
        Image {
            id: alternating_png
            objectName: "alternating_png"
            source: "Alternating_gimp.png"
            x: 100
            y: 290
            visible: true
            antialiasing: true
            PropertyAnimation {
                id: alternating_png_anim
                objectName: "alternating_png_anim"
                target: alternating_png
                property: "rotation"
                from: 180
                to: 360
                duration: 500
                //loops: Animation.Infinite
                easing.type:  Easing.InOutQuad
            }
        }
        Image {
            id: direct_png
            objectName: "direct_png"
            source: "Direct_gimp.png"
            x: 100
            y: 290
            visible: false
            antialiasing: true
            PropertyAnimation {
                id: direct_png_anim
                objectName: "direct_png_anim"
                target: direct_png
                property: "rotation"
                from: 180
                to: 360
                duration: 500
                //loops: Animation.Infinite
                easing.type:  Easing.InOutQuad
            }
        }
        Switch {
            id: switch_mA
            objectName: "switch_mA"
            signal signal_switch_mA(int i)
            x: 0
            y: 290
            text: qsTr("")
            checked: false;
            onClicked: {
                if (switch_mA.checked == true) {
                    signal_switch_mA(1);
                    //alternating_png.visible = false;
                    //direct_png.visible = true;
                    //direct_png_anim.running = true

                    //switch_mA.text = qsTr("Постоянный")
                } else {
                    signal_switch_mA(0);                    
                    //direct_png.visible = false;
                    //alternating_png.visible = true;
                    //alternating_png_anim.running = true
                    //switch_mA.text = qsTr("Переменный")
                }
            }            
        }               
    }
//! ________________ Resistor frame ________________________________
    Frame {
        id: frame
        x: 5
        y: 442
        width: 296
        height: 100
        ComboBox {
            id: resComboBox
            objectName: "resComboBox"
            x: 0
            y: 20
            width: 150
            height: 40
            editable: true
            indicator: Canvas {
                id: canvas
                x: 120
                y: 10
                width: 30
                height: 20
                contextType: "2d"
                Connections {
                    target: resComboBox
                    function onPressedChanged() { canvas.requestPaint(); }
                }
                onPaint: {
                    context.reset();
                    context.moveTo(2, 0);
                    context.lineTo(width-2, 0);
                    context.lineTo(width / 2, height);
                    context.closePath();
                    context.fillStyle = resComboBox.pressed ? "#ffffff" : "#491f1f";
                    context.fill();
                }
            }
            contentItem: Text {
                leftPadding: 0
                rightPadding: resComboBox.indicator.width + resComboBox.spacing
                text: resComboBox.displayText
                font.pixelSize: 20
                font.bold: true
                color: resComboBox.pressed ? "#ffffff" : "#491f1f"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideMiddle
            }
            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 40
                border.color: resComboBox.pressed ? "#ffffff" : "#491f1f"
                border.width: resComboBox.visualFocus ? 2 : 1
                radius: 2
            }
            popup: Popup {
                y: resComboBox.height - 1
                width: resComboBox.width
                implicitHeight: contentItem.implicitHeight
                padding: 1
                contentItem: ListView {
                    clip: true
                    implicitHeight: contentHeight
                    model: resComboBox.popup.visible ? resComboBox.delegateModel : null
                    currentIndex: resComboBox.highlightedIndex
                    ScrollIndicator.vertical: ScrollIndicator { }
                }

                background: Rectangle {
                    border.color: "#21be2b"
                    radius: 2
                }
            }
            model: ListModel {
                id: resModel
                ListElement { text: "0.7" }
                ListElement { text: "1.5" }
                ListElement { text: "2" }
                ListElement { text: "6" }
                ListElement { text: "25" }
                ListElement { text: "60" }
            }
            onActivated: {
                switch(resComboBox.currentIndex) {
                    case 0: resButSet.background.color = "#ff0000"; break;
                    case 1: resButSet.background.color = "#f00f00"; break;
                    case 2: resButSet.background.color = "#d01f00"; break;
                    case 3: resButSet.background.color = "#a03f00"; break;
                    case 4: resButSet.background.color = "#995f00"; break;
                    case 5: resButSet.background.color = "#977e00"; break;
                }
            }
        }
        Label {
            id: label
            x: 151
            y: 25
            text: qsTr("МОм")
            font.pixelSize: 30
        }
        Button {
            id: resButSet
            objectName: "resButSet"
            x: 220
            y: 21
            width: 50
            height: 50
            text: "Set"
            signal resBut_clicked(int x)
            background: Rectangle {
               color: resButSet.pressed ? " #1a6938" : "#ff0000"
               implicitWidth: 290
               implicitHeight: 40
               border.color: "#110627"
               border.width: 1
               radius: 4
            }
            onClicked: {
                resButSet.background.color = "#ffffff";
                resButSet.resBut_clicked(resComboBox.currentIndex);
            }
        }
    }
//! ________________ UART CONNECTION FRAME __________________________________
    Frame {
        id: frame1
        x: 314
        y: 442
        width: 270
        height: 100
        ComboBox {
            id: comComboBox
            objectName: "comComboBox"
            x: 0
            y: 35
            width: 140
            height: 32            
            model: listModel
            textRole: "display"
            currentIndex: comIndex
            function setIndex(i) {
                labelLog.text = i;
                comComboBox.currentIndex=i;
            }
                //function addItems (port){
                //    comModel.append(port);
                //}
        }
        Button {
            id: comButConnect
            objectName: "comButConnect"
            signal comSendName(string x)
            x: 158
            y: 35
            width: 75
            height: 30
            text: qsTr("Connect")
            background: Rectangle {
               color: comButConnect.pressed ? " #1a6938" : "#5b8021"
               implicitWidth: 150
               implicitHeight: 30
               border.color: "#110627"
               border.width: 1
               radius: 4
            }
            onClicked: {

            }
        }
    }

//! ________________ MISCELLANEOUS BUTTONS __________________________________
    Button {
        id: butConnect
        objectName: "butConnect"
        x: 0
        y: 550
        width: 200
        height: 50
        //anchors.centerIn: parent
        text: "Connect"
        font.pixelSize: 30
        //anchors.verticalCenterOffset: 161
        //anchors.horizontalCenterOffset: -125
        background: Rectangle {
            color: butConnect.pressed ? " #17a81a" : "#3b0909"
           implicitWidth: 200
           implicitHeight: 50
           border.color: "#110627"
           border.width: 1
           radius: 4
        }
        signal connect_tcp()
        signal disconnect_tcp()
        onClicked: {
            if(Logic.connectedState === false){
                Logic.newConnectState("Disconnect");
                Logic.connectedState = true;
                connect_tcp()
            } else {
                Logic.newConnectState("Connect");
                Logic.connectedState = false;
                disconnect_tcp()
            }
            butConnect.text = Logic.a
        }
    }
    onChangeA: {
        butLab.text = Logic.a;
        //butLab.text: Logic.a
    }
    Button {
        id: buttonStart
        objectName: "buttonStart"
        x: 218
        y: 550
        text: "Start calibration"
        background: Rectangle {
            color: buttonStart.pressed ? " #17a81a" : "#3b0909"
            implicitWidth: 80
            implicitHeight: 40
            border.color: "#26282a"
            border.width: 1
            radius: 4
        }
    }
    Button {
        id: buttonExit
        x: 520
        y: 550
        visible: true
        width: 80
        height: 50
        text: "Exit"
        antialiasing: true
        // for text customization need to set object Text in contentItem
        contentItem: Text {
            text: buttonExit.text
            color: buttonExit.pressed ? "black" : "red"
            font.pixelSize: 30
        }
        background: Rectangle {
           color: buttonExit.pressed ? " #17a81a" : "#3b0909"
           implicitWidth: 80
           implicitHeight: 40
           border.color: "#26282a"
           border.width: 1
           radius: 4
        }
        onClicked: {
            x++
            console.log(x)
            window1.close()
        }
    }    
}
