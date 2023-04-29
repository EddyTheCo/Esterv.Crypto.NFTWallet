import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import nodeConection
import account

Drawer
{
    id: drawer
    closePolicy: Popup.CloseOnPressOutside

    ColumnLayout  //is better to use a ScrollView here
    {
        width:parent.width
        height:100
        Node_Connections
        {
            id:conn_
            Layout.fillWidth: true
            Layout.minimumHeight: 30
        }
        AccountQml
        {
            id:acc_
            Layout.fillWidth: true
            Layout.minimumHeight: 30
        }

    }

}
