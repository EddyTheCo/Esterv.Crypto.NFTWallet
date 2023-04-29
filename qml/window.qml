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



    Notification
    {
        id:noti
        width:360
        height:150
        x:(window.width-width)*0.5
        y: window.height*(1-0.05)-height
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
            if(Node_Conection.state==Node_Conection.Connected)
            {
                noti.show({"message":"Conected to "+ Node_Conection.nodeaddr });
            }

            NFTCreator.restart();
        }
    }
    GridLayout
    {
        id:grid
        anchors.fill: parent
        columns: parent.width > 700 ? 2 : 1
        rows : parent.width > 700 ? 1 : 2

        ListView {
            id:boxes

            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing : 10
            clip:true

            model: NFTCreator.model

            delegate: BoxNFT {
                width: ListView.view.width
            }

        }
        BoxMenu
        {
            id:boxmenu
            Layout.alignment: (grid.columns===1)?Qt.AlignBottom|Qt.AlignHCenter:Qt.AlignTop|Qt.AlignRight
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 340
            Layout.maximumHeight: 150
            settings.animate:drawer.visible
            z:1000
            onShowSettings: drawer.open();
        }


    }
    ConfDrawer
    {
        id:drawer
        width: 300
        height: window.height
    }





}
