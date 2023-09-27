#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"
#include "Qrimagedecoder.hpp"
#include "mydesigns.hpp"
#include "midlePay.hpp"
int main(int argc, char *argv[])
{
    auto foo=fooDesign::fooPrint(); //https://forum.qt.io/post/762513
    foo=fooQtQrGen::fooPrint();
    foo=fooQtQrDec::fooPrint();
    foo=fooMidlePay::fooPrint();


    QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("qrcodeblack"), new QRImageProvider("#10141c",1));
    engine.addImageProvider(QLatin1String("wasm"), new WasmImageProvider());
    engine.addImportPath("qrc:/esterVtech.com/imports");
    qDebug()<<engine.importPathList();

    qmlRegisterSingletonType(QUrl(u"qrc:/esterVtech.com/imports/MyDesigns/qml/CustomStyle.qml"_qs), "CustomStyle", 1, 0, "CustomStyle");
    const QUrl url(u"qrc:/esterVtech.com/imports/NftMinter/qml/window.qml"_qs);
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			&app, [url](QObject *obj, const QUrl &objUrl) {
			if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
			}, Qt::QueuedConnection);

	engine.load(url);

	return app.exec();
}
