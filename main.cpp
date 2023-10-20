#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"
#include "Qrimagedecoder.hpp"
int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("qrcodeblack"), new QRImageProvider("#10141c",1));
    engine.addImageProvider(QLatin1String("wasm"), new WasmImageProvider());
    engine.addImportPath("qrc:/esterVtech.com/imports");
    qDebug()<<engine.importPathList();

    qmlRegisterSingletonType(QUrl(u"qrc:/esterVtech.com/imports/MyDesigns/qml/CustomStyle.qml"_qs), "CustomStyle", 1, 0, "CustomStyle");
    const QUrl url(u"qrc:/esterVtech.com/imports/NftMinter/qml/window.qml"_qs);
	engine.load(url);

	return app.exec();
}
