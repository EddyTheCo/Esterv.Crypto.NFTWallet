import QtQuick
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.CustomControls
import QtQuick.Controls
import Esterv.Iota.Wallet

Rectangle
{
    id:root
    required property string issuer;
    required property string address;
    required property string metdata;
    required property string name;
    required property url uri;
    required property int index;

    radius:5

    color:Style.backColor2.darker(1.4)

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: parent.width*0.25
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
            Button
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Layout.maximumHeight: 50
                text: qsTr("Mint")

            }
        }
        ColumnLayout
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 50
                Text
                {
                    id:nfttext
                    text: (root.name)?root.name:"NFT #" + (root.index+1)
                    font: Style.h1
                    color: Style.frontColor2
                    width:parent.width-closebut.width
                    height:parent.height

                }
                CloseButton
                {
                    id:closebut
                    anchors.right: parent.right
                    anchors.top:parent.top
                    visible:!root.address
                    width:40
                    height:40
                    radius:40
                    flat:true
                    onClicked:
                    {
                        root.ListView.view.model.rmBox(root.index);
                    }
                }
            }


            Frame
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100
                visible:root.metdata||!root.address
                Label
                {
                    id:metla
                    text:qsTr("Metadata:")
                }
                ScrollView {
                    id:scrl
                    anchors.top: metla.bottom
                    width:parent.width
                    height:parent.height-metla.height
                    TextArea
                    {
                        id:metadata
                        readOnly:root.address
                        placeholderText: (root.address)?"":qsTr('{\n"standard": "IRC27",\n"type": "image/jpeg",\n"version": "v1.0"\n}')
                        anchors.fill: parent
                        text: (root.address)?root.metdata:"";
                        onEditingFinished: root.ListView.view.model.setProperty(root.index,"metdata",metadata.text);
                    }
                }
            }

            Frame
            {
                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.minimumHeight: 50
                Layout.alignment: Qt.AlignTop
                RowLayout
                {
                    anchors.fill: parent
                    Label
                    {
                        id:issuerla
                        text:qsTr("Issuer:")
                    }
                    ComboBox {
                        id:issuerSelector
                        Layout.fillWidth: true
                        visible:!root.address
                        currentIndex:-1
                        editable: false

                        model: Wallet.addresses
                        textRole:"bech32Address"
                        onActivated: {
                            root.ListView.view.model.setProperty(root.index,"issuer",issuerSelector.currentText);
                        }

                    }

                }



            }


        }
    }


}


