import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import MyDesigns
import nodeConection
import account
import NFT_Minter


ApplicationWindow {
    visible: true
    id:window
    MyPayPopUp
    {
        id:paypopup
        addr_:""
        descr_:""
        visible:false
        closePolicy: Popup.CloseOnPressOutside
        anchors.centerIn: Overlay.overlay
    }
    Connections {
        target: NFTCreator
        function onLfundsChanged() {
            paypopup.addr_=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            paypopup.descr_="Pay at least "+ NFTCreator.Lfunds +" "+ Node_Conection.info().baseToken.subunit+" to: \n" +paypopup.addr_ ;
	paypopup.url_="firefly:v1/wallet/send?recipient="+Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp)+"&amount="+NFTCreator.Lfunds;	
            paypopup.visible=NFTCreator.Lfunds;
        }
    }
    Connections {
        target: Account
        function onSeedChanged() {
            NFTCreator.restart();
        }
    }
    Connections {
        target: Node_Conection
        function onStateChanged() {
            NFTCreator.restart();
        }
    }

    Drawer
    {
        id: drawer
        width: 360
        height: window.height

        closePolicy: Popup.CloseOnPressOutside
        background:Rectangle
        {
            color:"#0f171e"
            border.width: 2
            border.color: "white"
            radius:4
        }


        ColumnLayout
        {
            anchors.fill: parent
            spacing: 5
            Node_Connections
            {
                id:conn_
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 200
                Layout.maximumWidth:  350
                Layout.maximumHeight: 250
                Layout.minimumWidth: 75
                Layout.alignment: Qt.AlignCenter

            }
            AccountQml
            {
                id:acc_

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 350
                Layout.maximumWidth:  350
                Layout.maximumHeight: 400
                Layout.minimumWidth: 75
                Layout.alignment: Qt.AlignCenter
            }

        }



    }
    RowLayout
    {
        anchors.fill:parent
        Rectangle
        {

            Layout.minimumHeight: 50
            Layout.fillHeight:  true
            Layout.preferredWidth: 100
            Layout.minimumWidth: 50
            Layout.maximumWidth: 90
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            color:"#1f2937"

            MySettButton
            {
                anchors.centerIn: parent
                width:parent.width*0.95
                height:width/2
                id:seetb
                onClicked: drawer.open();
                rect_:Rectangle {
                    radius:10
                    border.color:"#0f79af"
                    border.width:1
                    color:"#0d1117"
                }
            }
        }


        MyFrame
        {

            Layout.minimumHeight: 500
            Layout.fillHeight:  true
            Layout.minimumWidth: 300
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            description:"Mint and send NFT"
            backColor:"#0f171e"


            ColumnLayout
            {
                spacing:20
                anchors.fill:parent

                MyTextField
                {
                    id:raddres_

                    Layout.maximumHeight: 100
                    Layout.minimumHeight: 50
                    Layout.fillHeight:  true
                    Layout.maximumWidth: 500
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    desc:"Receiving address"
                    placeholderText: (Node_Conection.state)?Node_Conection.info().protocol.bech32Hrp+"1... (mandatory)":"Waiting for the node"
                }


                MyTextArea
                {
                    id:immetadata_

                    Layout.minimumHeight: 150
                    Layout.fillHeight:  true
                    Layout.maximumWidth: 600
                    Layout.maximumHeight: width*0.5
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter

                    desc:"Immutable metadata"
                    placeholderText: qsTr('{\n"standard": "IRC27",\n"type": "image/jpeg",\n"version": "v1.0"\n}')
                }



                MyTextArea
                {

                    Layout.minimumHeight: 150
                    Layout.fillHeight:  true
                    Layout.maximumWidth: 600
                    Layout.maximumHeight: width*0.5
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    id:metadata_
                    desc:"Metadata"
                    placeholderText: qsTr('Testing NFTs on shimmer')
                }
                Rectangle
                {
                    id:create_
                    color:"#1f2937"
                    Layout.maximumHeight: 100
                    Layout.minimumHeight: 50
                    Layout.minimumWidth: 100
                    Layout.fillHeight:  true
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignBottom
                    MyButton
                    {
                        text:qsTr("Create")
                        anchors.centerIn: parent
                        width:120
                        height:parent.height*0.7
                        enabled: Node_Conection.state

                        onClicked:{
                            NFTCreator.recaddr=raddres_.tfield.text;
                            NFTCreator.immetadata=immetadata_.tfield.text;
                            NFTCreator.metadata=metadata_.tfield.text;
                            NFTCreator.tryToCreate();
                        }
                    }
                }

            }





        }
        MyFrame
        {
            Layout.minimumHeight: 400
            Layout.fillHeight:  true
            Layout.maximumWidth: 300
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            description:"Blocks produced"
            backColor:"#0f171e"

            ListView {
                id:listview
                anchors.fill: parent
                model: blockidModel
                delegate: Rectangle{
                    id:rootdelegate
                    required property string blockid
                    color:"transparent"
                    width: ListView.view.width
                    height:80

                    Rectangle
                    {
                        anchors.centerIn: parent
                        color:"#1f2937"
                        border.width: 2
                        border.color: "white"
                        width: parent.width*0.95
                        height:parent.height*0.9
                        radius:5
                        Text
                        {
                            id:test
                            width:parent.width*0.90
                            anchors.centerIn: parent
                            color:"white"
                            text: "block id:"+rootdelegate.blockid;
                            elide:Text.ElideRight
                            fontSizeMode:Text.Fit
                        }

                        ToolTip
                        {
                            id:tooltip
                            visible: false
                            text:qsTr("Copy")
                        }
                        TextEdit{
                            id: textEdit
                            visible: false
                        }
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled :true
                            onEntered: tooltip.visible=!tooltip.visible
                            onExited: tooltip.visible=!tooltip.visible
                            onClicked:
                            {
                                textEdit.text = rootdelegate.blockid;
                                textEdit.selectAll();
                                textEdit.copy();
				Qt.openUrlExternally("https://explorer.shimmer.network/testnet/block/"+rootdelegate.blockid);
                            }
                        }
                    }
                }
            }

            ListModel {
                id: blockidModel


            }
            Connections {
                target: NFTCreator
                function onNewBlock(id) {
                    blockidModel.insert(0,{"blockid":id});
                }
            }
        }

    }



}
