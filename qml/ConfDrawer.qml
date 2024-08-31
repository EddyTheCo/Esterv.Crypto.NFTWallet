import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Iota.Account
import Esterv.Styles.Simple
import Esterv.Iota.NodeConnection
import Esterv.CustomControls
import Esterv.Crypto.NFTWallet
import Esterv.AboutEstervNFTWallet

Drawer
{
    id: drawer
    closePolicy: Popup.CloseOnPressOutside
    focus:true
    modal:true
    background: Rectangle
    {
        color:Style.backColor2
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout
        {
            ThemeSwitch
            {
                Layout.alignment: Qt.AlignTop || Qt.AlignLeft
            }
            Button
            {
                text:"About"
                onClicked:aboutpop.open()
            }
        }

        Popup
        {
            id:aboutpop
            visible:false
            anchors.centerIn: Overlay.overlay
            closePolicy: Popup.CloseOnPressOutside
            modal:true
            About
            {
                anchors.fill: parent
            }
        }


        NodeConnectionSettings
        {
            id:conn_
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }
        AccountSettings
        {
            id:acc_
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }
        UpdateFrame
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
        }
    }

}
