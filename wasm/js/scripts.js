let qtModule = undefined;
async function init() {
	
	const videodiv = document.querySelector('#embedvideo');
	videodiv.innerHTML='<iframe width="560" height="315" src="https://www.youtube.com/embed/UK_493BTI1M?si=Ab3KWGpPy0hE5Af9" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>'
	const rootDiv = document.querySelector('#qtrootDiv');

	rootDiv.innerHTML += '<figure  id="qtspinner"> <center > <img id="logo" crossorigin="anonymous" src="img/esterlogo.png" ></img> <div id="qtstatus"></div> </center> </figure> <div class="qtscreen" id="screen" ></div>';

	const spinner = rootDiv.querySelector('#qtspinner');
	const screen = rootDiv.querySelector('#screen');
	const status = rootDiv.querySelector('#qtstatus');


            const showUi = (ui) => {
                [spinner, screen].forEach(element => element.style.display = 'none');
                if (screen === ui)
                    screen.style.position = 'default';
                ui.style.display = 'block';
            }

	try {
                showUi(spinner);
                status.innerHTML = 'Loading...';

                qtModule = await qtLoad({
                    qt: {
                        onLoaded: () => 
			    {
				    showUi(screen);
			    },
                        onExit: exitData =>
                        {
                            status.innerHTML = 'Application exit';
                            status.innerHTML +=
                                exitData.code !== undefined ? ` with code ` : '';
                            status.innerHTML +=
                                exitData.text !== undefined ? ` ()` : '';
                            showUi(spinner);
                        },
                        entryFunction: window.createQtAppInstance,
                        containerElements: [screen],

                    }
                });
            } catch (e) {
                console.error(e);
                console.error(e.stack);
            }

}
function resizeSplitX(event) {
	var canvas = document.getElementById("screen");;
	qtModule.qtResizeContainerElement(canvas);
}
