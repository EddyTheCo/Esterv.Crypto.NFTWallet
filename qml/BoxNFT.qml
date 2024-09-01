import QtQuick
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.CustomControls
import QtQuick.Controls
import Esterv.Iota.Wallet
import Esterv.Crypto.NFTWallet
import Esterv.Iota.NodeConnection
import Esterv.CustomControls.QrGen

Rectangle {
    id: root
    required property string issuer
    required property string address
    required property string metdata
    required property string name
    required property url uri
    required property int index

    required property int state

    radius: 5

    color: Style.backColor2.darker(1.4)

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: parent.width * 0.25
            Image {
                id: pic
                source: root.uri
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 75
                Layout.minimumHeight: 75
                Layout.alignment: Qt.AlignCenter
                fillMode: Image.PreserveAspectFit
            }
            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Layout.maximumHeight: 50
                text: qsTr((root.state === NftBox.Minting) ? "Minting" : ((root.state === NftBox.Sending) ? "Sending" : "Mint"))
                onClicked: root.ListView.view.model.mint(root.index)
                enabled: root.state === NftBox.Ready && (NodeConnection.state && ((Object.keys(Wallet.amount.json).length != 0) && Wallet.amount.json.largeValue.value > 0))
                ToolTip.text: text
                ToolTip.visible: hovered
                visible: !root.address
            }
            DelayButton {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Layout.maximumHeight: 50
                text: qsTr((root.state === NftBox.Ready) ? "Burn" : ((root.state === NftBox.Sending) ? "Sending" : "Burning"))
                onActivated: root.ListView.view.model.burn(root.index)
                enabled: root.state === NftBox.Ready
                ToolTip.text: qsTr("Hold to burn")
                ToolTip.visible: hovered && enabled
                delay: 2000
                visible: root.address
            }
        }
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 50
                Text {
                    id: nfttext
                    text: (root.name) ? root.name : "NFT #" + (root.index + 1)
                    font: Style.h1
                    color: Style.frontColor2
                    width: parent.width - closebut.width
                    height: parent.height
                }
                CloseButton {
                    id: closebut
                    anchors.right: parent.right
                    anchors.top: parent.top
                    visible: !root.address && (root.state === NftBox.Ready)
                    width: 32
                    height: 32
                    radius: 32
                    flat: true
                    onClicked: {
                        root.ListView.view.model.rmBox(root.index);
                    }
                }
                CheckBox {
                    id: select
                    anchors.right: parent.right
                    anchors.top: parent.top
                    visible: root.address && (root.state === NftBox.Ready)
                    width: 32
                    height: 32
                    onCheckedChanged: {
                        root.ListView.view.model.setProperty(root.index, "selected", select.checked);
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100
                visible: root.metdata || !root.address
                Label {
                    id: metla
                    text: qsTr("Metadata:")
                }
                ScrollView {
                    id: scrl
                    anchors.top: metla.bottom
                    width: parent.width
                    height: parent.height - metla.height
                    TextArea {
                        id: metadata
                        readOnly: root.address || root.state !== NftBox.Ready
                        placeholderText: (root.address) ? "" : qsTr('{\n"standard": "IRC27",\n"type": "image/jpeg",\n"version": "v1.0"\n}')
                        anchors.fill: parent
                        text: (root.address) ? root.metdata : ""
                        onEditingFinished: (metadata.readOnly) ? console.log("Fix this") : root.ListView.view.model.setProperty(root.index, "metdata", metadata.text)
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.minimumHeight: 50
                Layout.alignment: Qt.AlignTop
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    Label {
                        id: addrla
                        visible: root.address
                        text: qsTr("Address:")
                    }
                    QrText {
                        text: root.address
                        visible: root.address
                        Layout.fillWidth: true
                        Layout.maximumWidth: implicitWidth
                    }
                    Label {
                        id: issuerla
                        text: qsTr("Issuer:")
                        visible: (!root.address || root.issuer)
                    }
                    QrText {
                        text: root.issuer
                        visible: root.address && root.issuer
                        Layout.fillWidth: true
                        Layout.maximumWidth: implicitWidth
                    }
                    ComboBox {
                        id: issuerSelector
                        Layout.fillWidth: true
                        visible: !root.address
                        currentIndex: -1
                        editable: false
                        enabled: root.state === NftBox.Ready
                        model: Wallet.addresses
                        textRole: "bech32Address"
                        onActivated: {
                            root.ListView.view.model.setProperty(root.index, "issuer", issuerSelector.currentText);
                        }
                    }
                }
            }
        }
    }
}
