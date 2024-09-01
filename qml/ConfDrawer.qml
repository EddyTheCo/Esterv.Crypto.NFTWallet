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


            ThemeSwitch
            {
                Layout.alignment:  Qt.AlignLeft
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
        Label
        {
            id:aboutLabel
            text:AboutThis.name + " " + AboutThis.version
            Layout.alignment: Qt.AlignBottom|Qt.AlignHCenter

            MouseArea
            {
                anchors.fill: parent
                hoverEnabled:true

                Popup
                {
                    id:aboutpop
                    visible:parent.containsMouse
                    y: -height
                    width:300
                    height:400
                    About
                    {
                        anchors.fill: parent
                        anchors.margins: 10
                        description:"Made with \u2764 by Esterv"
                        logo:"https://eddytheco.github.io/Esterv.Crypto.NFTWallet/img/esterlogo.png"
                    }
                }

            }
        }
    }

}
