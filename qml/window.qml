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
            paypopup.addr_=Account.addr([0,0,0]);
            paypopup.descr_="Pay at least "+ NFTCreator.Lfunds +" to: \n" +paypopup.addr_ ;
            paypopup.visible=NFTCreator.Lfunds;
        }
    }

    Drawer{
        id: drawer
        width: (window.width<1100)?0.4 * window.width:440
        height: window.height
        dragMargin :0
        position:1.0
        visible:true
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
            Layout.maximumWidth: 120
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            color:"#1f2937"

            MyButton
            {
                anchors.centerIn: parent
                width:parent.width*0.95
                height:width/2
                id:seetb
                text:qsTr("Settings")
                onClicked: drawer.open();
            }
        }


        MyFrame
        {

            Layout.minimumHeight: 400
            Layout.fillHeight:  true
            Layout.preferredWidth: 400
            Layout.minimumWidth: 300
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            description:"Mint and send NFT"
            backColor:"#0f171e"


            ColumnLayout
            {
                spacing:20
                width:parent.width
                height:parent.height-create_.height

                MyTextField
                {
                    id:raddres_

                    Layout.maximumHeight: 300
                    Layout.minimumHeight: 45
                    Layout.fillHeight:  true
                    Layout.minimumWidth: 75
                    Layout.maximumWidth: 500
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    desc:"Recieving address"
                    placeholderText: "rms1... (mandatory)"
                }


                MyTextArea
                {
                    id:immetadata_
                    Layout.maximumHeight: 500
                    Layout.minimumHeight: 150
                    Layout.fillHeight:  true
                    Layout.minimumWidth: 75
                    Layout.maximumWidth: 600
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter

                    desc:"Immutable metadata"
                    placeholderText: qsTr('{\n"standard": "IRC27",\n"type": "image/jpeg",\n"version": "v1.0"\n}')
                }



                MyTextArea
                {
                    Layout.maximumHeight: 500
                    Layout.minimumHeight: 150
                    Layout.fillHeight:  true
                    Layout.minimumWidth: 75
                    Layout.maximumWidth: 600
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    id:metadata_
                    desc:"Metadata"
                    placeholderText: qsTr('Testing NFTs on shimmer')
                }

            }


            Rectangle
            {
                id:create_
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width:parent.width
                height:75
                color:"#1f2937"
                MyButton
                {
                    text:qsTr("Create")
                    anchors.centerIn: parent
                    width:100
                    height:50
                    enabled: NFTCreator.status

                    onClicked:{
                        NFTCreator.recaddr=raddres_.tfield.text;
                        NFTCreator.immetadata=immetadata_.tfield.text;
                        NFTCreator.metadata=metadata_.tfield.text;
                        NFTCreator.tryToCreate();
                    }
                }
            }


        }
        MyFrame
        {
            Layout.minimumHeight: 400
            Layout.fillHeight:  true
            Layout.preferredWidth: 400
            Layout.minimumWidth: 300
            Layout.maximumWidth: 500
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
                    width: listview.width
                    height:listview.height/10

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


    Component.onCompleted:
    {
        NFTCreator.account=Account
        NFTCreator.connection=Node_Conection
    }

}
