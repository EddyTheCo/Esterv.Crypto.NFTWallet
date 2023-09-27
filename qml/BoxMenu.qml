import QtQuick
import QtQuick.Layouts
import MyDesigns
import nodeConection
import account
import NftMinter
import QtQuick.Controls
import QtQrGen
import QtQrDec
Rectangle
{
    id:root
    property alias settings:settings_
    property bool incolum: height>300
    signal showSettings()
    color:CustomStyle.backColor2.lighter(1.5)
    radius:3
    ColumnLayout
    {
        anchors.fill: parent
        QrLabel
        {
            id:addr
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            description: qsTr("Account")
            address: (Node_Conection.state)?Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp):''
            color: CustomStyle.frontColor1
        }
        Text
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            font:CustomStyle.h3
            text:qsTr("Available Balance: ")
            horizontalAlignment:Text.AlignHCenter
            color: CustomStyle.frontColor1
            fontSizeMode:Text.Fit
        }
        AmountText
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            font:CustomStyle.h2
            jsob:(Node_Conection.state)?NFTCreator.funds:{}
            horizontalAlignment:Text.AlignHCenter
            fontSizeMode:Text.Fit
        }

        GridLayout
        {
            rowSpacing: 10
            columnSpacing: 15
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.minimumHeight: 45+100*root.incolum
            columns: root.incolum ? 1 : 4
            MySettButton
            {
                id:settings_
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                onReleased: root.showSettings();
                text:qsTr("Settings")
            }
            PlusButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:"Add New"
                onClicked:
                {
                    NFTCreator.model.newBox();
                }
            }
            MyButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:qsTr("Mint")
                enabled:(NFTCreator.model.newBoxes>0&&Node_Conection.state&&NFTCreator.state)
                onClicked:
                {
                    NFTCreator.mint();
                }
            }
            MyButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:qsTr("Send")
                enabled:(Node_Conection.state&&NFTCreator.state&&(NFTCreator.funds.largeValue.value>0))
                onClicked:
                {
                    popup_.open();
                }
            }
        }
    }

    Connections {
        target: Account
        function onSeedChanged() {
            if(Node_Conection.state===Node_Conection.Connected)
            {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
    }
    Connections {
        target: Node_Conection
        function onStateChanged() {
            if(Node_Conection.state===Node_Conection.Connected)
            {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
    }
    QrTextArea
    {
        id:popup_
        visible:false
        closePolicy: Popup.CloseOnPressOutside
        width:300
        height:425
        anchors.centerIn: Overlay.overlay
        description: qsTr("Send all your assests to:")
        placeholder: (Node_Conection.state)?Node_Conection.info().protocol.bech32Hrp+"1":""
        onClicked: (data) => {
                       NFTCreator.recAddress=data;
                       NFTCreator.send();
                   }
    }

}
