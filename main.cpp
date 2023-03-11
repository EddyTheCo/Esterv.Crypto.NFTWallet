#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"



int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.addImageProvider(QLatin1String("qrcodeblack"), new QRImageProvider("black",1));
    engine.addImportPath("qrc:/esterVtech.com/imports");
    engine.addImportPath("_deps/mydesigns-build/");
    engine.addImportPath("_deps/account-build/");
    engine.addImportPath("_deps/nodeconection-build/");
    qDebug()<<engine.importPathList();

    const QUrl url(u"qrc:/esterVtech.com/imports/NFT_Minter/qml/window.qml"_qs);
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			&app, [url](QObject *obj, const QUrl &objUrl) {
			if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
			}, Qt::QueuedConnection);

	engine.load(url);

	return app.exec();
}
