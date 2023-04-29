import QtQuick.Controls
import QtQuick
import MyDesigns



Text
{
    id:root
    property string description:"";
    required property string address;

    MyPayPopUp
    {
        id:popup_
        address:root.address
        description:root.description
        visible:false
        closePolicy: Popup.CloseOnPressOutside
        anchors.centerIn: Overlay.overlay
    }

    text:"<b>"+ root.description +":   </b> <font color=\"#1998ff\">"+ root.address +"</font>"
    fontSizeMode:Text.Fit
    elide:Text.ElideRight
    horizontalAlignment: TextEdit.AlignLeft


    MouseArea {
        anchors.fill: parent
        onClicked: popup_.visible=true;
    }
}
