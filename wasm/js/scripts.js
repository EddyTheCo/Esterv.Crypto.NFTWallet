function init() {
	qtMint=initQTwasm('.', 'nftminter', '#qtrootDiv', 'img/esterlogo.png');
	qtBuy=initQTwasm('https://eddytheco.github.io/BuyMeACoffe/', 'buymeacoffe', '#buyme', '');
	checkModuleLoad=setInterval(function() {
                  if (qtBuy.module())
                  {
                          qtBuy.module().Monitor.get_monitor().set_properties("https://api.shimmer.network","smr1qq9j9n0wmqu7fhermm6x7lpw35zd8dn24vcty62u06wt7g0fa7fukh98hvc");
                          clearInterval(checkModuleLoad);
                  }
  
                  if( typeof counter == 'undefined' ) {
                          counter = 0;
                  }
                  counter++;
                  if(counter>60)clearInterval(checkModuleLoad);
	document.getElementById('buyme').style.opacity = 1.0;
          }, 1000);

}


function resizeSplitX(event) {
	const canvasMint = document.querySelector('#screennftminter');
	qtMint.resizeCanvasElement(canvasMint);
	const canvasBuy = document.querySelector('#screenbuymeacoffe');
	qtBuy.resizeCanvasElement(canvasBuy);
}
