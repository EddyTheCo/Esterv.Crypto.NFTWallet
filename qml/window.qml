import QtQuick.Controls
import QtQuick
import QtQuick.Layouts

import account
import NftMinter
import MyDesigns
import nodeConection
import midlePay

ApplicationWindow {
    visible: true
    id:window

    FontLoader {
        id: nftFont
        source: "qrc:/esterVtech.com/imports/NftMinter/fonts/Anton/Anton-Regular.ttf"
    }
    FontLoader {
        id: amountFont
        source: "qrc:/esterVtech.com/imports/NftMinter/fonts/Permanent_Marker/PermanentMarker-Regular.ttf"
    }
    background: Rectangle
    {
        color:CustomStyle.backColor1
    }
    Component.onCompleted:
    {
        if(LocalConf.nodeaddr) Node_Conection.nodeaddr=LocalConf.nodeaddr;
        if(LocalConf.jwt) Node_Conection.jwt=LocalConf.jwt;
        CustomStyle.h1=Qt.font({
                                   family: nftFont.font.family,
                                   weight: nftFont.font.weight,
                                   pixelSize: 28
                               });
        CustomStyle.h2=Qt.font({
                                   family: amountFont.font.family,
                                   weight: amountFont.font.weight,
                                   pixelSize: 28
                               });
    }


    Connections {
        target: Account
        function onSeedChanged() {
            NFTCreator.restart();
        }
    }

    Connections {
        target: NFTCreator
        function onNotEnought(amount) {
            noti.show({"message":"Not enough funds\n "+ "lack of "+ amount.largeValue.value + " "+ amount.largeValue.unit });
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
    MidlePayPop
    {
        initTag:"NFTCreator"
        width:300
        height:350
        anchors.centerIn: Overlay.overlay
    }

    GridLayout
    {
        id:grid
        width:window.width-drawer.width*((window.width>500)?drawer.position:0.0)
        height:drawer.height
        x:drawer.width*drawer.position
        columns: width > 750 ? 2 : 1
        rows : width > 750 ? 1 : 2
        rowSpacing: 10


        ListView {
            id:boxes

            Layout.alignment: Qt.AlignTop
            Layout.margins:  15
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: (grid.columns===1)?150:450
            spacing : 10
            clip:true

            model: NFTCreator.model

            BusyIndicator {
                anchors.top: boxes.top
                anchors.horizontalCenter: boxes.horizontalCenter
                anchors.margins: 20
                running: NFTCreator.state===NFTCreator.Null
            }


            delegate: BoxNFT {
                width: ListView.view.width

            }

        }
        BoxMenu
        {
            id:boxmenu
            Layout.alignment: (grid.columns===1)?Qt.AlignBottom|Qt.AlignHCenter:Qt.AlignVCenter|Qt.AlignRight
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: (grid.columns===1)?15:0
            Layout.leftMargin: (grid.columns===1)?0:15
            Layout.maximumWidth: 500-150*(grid.columns-1)
            Layout.maximumHeight: 200+250*(grid.columns-1)
            Layout.minimumWidth: (grid.columns===1)?150:250
            settings.animate:drawer.visible
            onShowSettings:(drawer.position>0)?drawer.close():drawer.open();

        }


    }
    ConfDrawer
    {
        id:drawer
        width: 300
        height: window.height
    }



    Notification
    {
        id:noti
        width:360
        height:100
        x:(window.width-width)*0.5
        y: window.height*(1-0.05)-height
    }


}
