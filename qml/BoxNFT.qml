import QtQuick
import QtQuick.Layouts
import MyDesigns
import QtQuick.Controls


Rectangle
{
    id:root
    required property string issuer;
    required property string address;
    required property string metdata;
    required property int index;

    radius:5

    height:((root.address)?((root.issuer)?255:240):220) + 90*metframe.collapsed
    color:CustomStyle.backColor1.lighter(1.4)

    ColumnLayout
    {
        id:dat
        width:parent.width*0.95
        height:parent.height*0.95
        anchors.centerIn: parent

        RowLayout
        {
            Layout.alignment: (Qt.AlignTop)
            Layout.fillWidth: true
            Layout.fillHeight:  true
            Layout.maximumHeight: 60
            Layout.minimumHeight: 20
            Text
            {
                id:nfttext
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft|Qt.AlignTop
                text: "NFT #" + (root.index+1)
                fontSizeMode:Text.VerticalFit

                font: CustomStyle.h1
                color: CustomStyle.frontColor2
            }

            MyCheckBox
            {
                id: isissuer
                Layout.alignment: Qt.AlignRight|Qt.AlignTop
                checked: root.address===root.ListView.view.model.cissuer
                visible: root.address

                text:qsTr("Use as issuer")
                nextCheckState: function() {

                    if(isissuer.checked)
                    {
                        root.ListView.view.model.unsetCissuer();
                        return Qt.Unchecked
                    }
                    else
                    {
                        root.ListView.view.model.cissuer=root.address;
                        return Qt.Checked
                    }
                }
            }



        }

        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: 60
            Layout.minimumHeight: ((root.address)?18:0)+((root.issuer)?18:0)+5
            Layout.alignment: Qt.AlignTop
            Column
            {
                id:datcolumn
                anchors.fill: parent
                TextAddress
                {
                    id:taddr
                    visible:root.address
                    width:Math.min(parent.width,implicitWidth)
                    height:implicitHeight
                    description:"Address"
                    address:root.address
                    color:"white"
                }
                TextAddress
                {
                    id:tissuer
                    visible:root.issuer
                    width:Math.min(parent.width,implicitWidth)
                    height:implicitHeight
                    description:"Issuer"
                    address:root.issuer
                    color:"white"
                }
            }
        }


        MyFrame{
            id:metframe
            visible:root.metdata||!root.address
            description: qsTr("Metadata")
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.alignment: Qt.AlignTop
            ScrollView {
                id:scrl
                visible:metframe.collapsed
                anchors.fill: parent
                TextArea
                {
                    id:metadata
                    visible:metframe.collapsed
                    readOnly:root.address
                    placeholderText: (root.address)?"":qsTr('{\n"standard": "IRC27",\n"type": "image/jpeg",\n"version": "v1.0"\n}')
                    anchors.fill: parent
                    text: (root.address)?root.metdata:"";
                    onEditingFinished: root.ListView.view.model.setProperty(root.index,"metdata",metadata.text);
                    color:CustomStyle.frontColor1
                    placeholderTextColor:CustomStyle.midColor1
                }
            }
        }
    }
    CloseButton
    {
        id: rmv
        anchors.right: root.right
        anchors.top: root.top
        anchors.margins: 10
        width:14
        height:14
        visible: !root.address
        onClose:
        {
            root.ListView.view.model.rmBox(root.index);
        }

    }
}


