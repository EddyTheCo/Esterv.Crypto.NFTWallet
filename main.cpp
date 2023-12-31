#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"
#if defined(USE_QRDEC)
#include "Qrimagedecoder.hpp"
#endif
#if defined(FORCE_STYLE)
#include <QQuickStyle>
#endif
int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);
#if defined(FORCE_STYLE)
    QQuickStyle::setStyle(FORCE_STYLE);
#endif

	QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("qrcode"), new QRImageProvider(1));
#if defined(USE_QRDEC)
    engine.addImageProvider(QLatin1String("wasm"), new WasmImageProvider());
#endif
    engine.addImportPath("qrc:/esterVtech.com/imports");

    const QUrl url(u"qrc:/esterVtech.com/imports/Esterv/Iota/NFTMinter/qml/window.qml"_qs);
	engine.load(url);

	return app.exec();
}
