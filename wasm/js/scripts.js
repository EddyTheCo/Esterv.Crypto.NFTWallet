function init() {
	initQTwasm('.', 'nftminter', '#qtrootDiv', 'img/qtlogo.svg');

}


function resizeSplitX(event) {
	const canvas = document.querySelector('#screen');
	qtLoader.resizeCanvasElement(canvas);
}
