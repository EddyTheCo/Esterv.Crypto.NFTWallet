import QtQuick
import QtQuick.Layouts
import MyDesigns



Rectangle
{
    id:root
    required property string issuer
    required property string address
    //required property string data

    GridLayout
    {
        id:grid
        anchors.fill: parent
        columns: parent.width > 350 ? 2 : 1
        rows : parent.width > 350 ? 1 : 2

        MyRadioButton
        {
            id: isissuer
            Layout.minimumWidth: (grid.columns===2)?50:25
            Layout.maximumWidth: 75
            Layout.maximumHeight: width
            Layout.minimumHeight: width
            Layout.alignment: (grid.columns===2)?(Qt.AlignTop|Qt.AlignRight):(Qt.AlignBottom|Qt.AlignHCenter)
            Layout.fillHeight: true
            Layout.fillWidth: true
            checked: root.address===root.ListView.view.model.cissuer
            visible: root.address!=""
            onPressed: {
                root.ListView.view.model.cissuer=root.address
            }
        }
        ColumnLayout
        {
            id:dat
            Layout.minimumWidth: 250
            Layout.minimumHeight: 100
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: (grid.columns===2)?(Qt.AlignTop|Qt.AlignLeft):(Qt.AlignTop|Qt.AlignHCenter)

            TextAddress
            {
                id:taddr
                visible:root.address!=""
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                description:"Address"
                address:root.address
                color:"white"
            }
            TextAddress
            {
                id:tissuer
                visible:root.issuer!=""
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                description:"Issuer"
                address:root.issuer
                color:"white"
            }
            MyTextArea
            {
                visible:root.address===""||root.data!==""
                label.visible:false
                textarea.readOnly:root.address!==""
            }

        }
    }

}
