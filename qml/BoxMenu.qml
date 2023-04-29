import QtQuick
import QtQuick.Layouts
import MyDesigns
import nodeConection
import account
import NFT_Minter


Rectangle
{
    id:root
    property alias settings:settings_
    signal showSettings()
    ColumnLayout
    {
        anchors.fill: parent
        TextAddress
        {
            id:addr
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            description:"Account"
            address:Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp)
            color:"white"
        }

        RowLayout
        {
            spacing:20
            Layout.fillHeight: true
            Layout.fillWidth: true
            MySettButton
            {
                id:settings_
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 75
                Layout.preferredHeight: width

                onClicked: root.showSettings();
            }
        }
    }

    Connections {
            target: Account
            function onSeedChanged() {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
        Connections {
            target: Node_Conection
            function onStateChanged() {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
}
