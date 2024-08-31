import QtQuick.Controls
import QtQuick
import QtQuick.Layouts

import Esterv.Styles.Simple
import Esterv.Iota.NodeConnection
import Esterv.Crypto.NFTWallet

import Esterv.Iota.Account

ApplicationWindow {
    visible: true
    id:window

    FontLoader {
        id: nftFont
        source: "qrc:/esterVtech.com/imports/Esterv/Iota/NFTMinter/fonts/Anton/Anton-Regular.ttf"
    }
    FontLoader {
        id: amountFont
        source: "qrc:/esterVtech.com/imports/Esterv/Iota/NFTMinter/fonts/Permanent_Marker/PermanentMarker-Regular.ttf"
    }
    background: Rectangle
    {
        color:Style.backColor1
    }
    Component.onCompleted:
    {
        Style.h1=Qt.font({
                             family: nftFont.font.family,
                             weight: nftFont.font.weight,
                             pixelSize: 28
                         });
        Style.h2=Qt.font({
                             family: amountFont.font.family,
                             weight: amountFont.font.weight,
                             pixelSize: 28
                         });
    }

    ConfDrawer
    {
        id:drawer
        width: Math.max(parent.width*0.2,350)
        height: window.height
    }
    GridLayout
    {
        id:grid
        property bool iscolumn:width < 750
        width:window.width-drawer.width*((window.width>500)?drawer.position:0.0)
        height:drawer.height
        x:drawer.width*drawer.position
        flow: grid.iscolumn ? GridLayout.TopToBottom : GridLayout.LeftToRight

       ListView {
            id:boxes

            Layout.alignment: Qt.AlignTop
            Layout.margins:  15
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: (grid.iscolumn)?150:450
            spacing : 10
            clip:true

            model: BoxModel


            delegate: BoxNFT {
                width: ListView.view.width
                height: 350
            }

        }

        BoxMenu
        {
            id:boxmenu
            onShowSettings: (drawer.position>0)?drawer.close():drawer.open();
            Layout.alignment: (grid.iscolumn)?Qt.AlignBottom|Qt.AlignHCenter:Qt.AlignVCenter|Qt.AlignRight
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: (grid.iscolumn)?15:0
            Layout.leftMargin: (grid.iscolumn)?0:15
            Layout.maximumWidth: 500-((grid.iscolumn)?150:0)
            Layout.maximumHeight: 200+((grid.iscolumn)?0:250)
            Layout.minimumWidth: (grid.iscolumn)?150:250
        }
    }




}
