#include "DiskInfoMgr.h"
#include <QFont>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QResource>
#include <QDir>
#include <QIcon>
#include <QQmlContext>
#include <QQuickView>
#include <QWidget>
#include <QSettings>
#include "ui\DiskHealthMainWin.h"

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT

#define _WIN32_WINNT 0x0A00
#include <WinBase.h>
#endif
Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap &p);

extern HANDLE s_hMutex;
extern bool isConsole;
extern bool CheckHasAnotherAppInstance();

void SetQtPath(QString strCurrentPath)
{
	do {
		if (strCurrentPath.isEmpty()) break;

		QByteArray baPath = strCurrentPath.toLocal8Bit();

		qputenv("QT_PLUGIN_PATH", baPath + "/plugins");
		qputenv("QML2_IMPORT_PATH", baPath + "/qml");

		qputenv("QML_XHR_ALLOW_FILE_READ", QByteArray("1"));
	} while (0);
}

void setPreApplicationEnv()
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

int main(int argc, char *argv[])
{
	setPreApplicationEnv();
	//a.setWindowIcon(QIcon(QString(":/res/disksmart.ico")));
	SetQtPath(QDir::currentPath());

	QApplication a(argc, argv);

	isConsole = qApp->arguments().contains(_EPM_CONSOLE);
	if (CheckHasAnotherAppInstance())
		return 1;

	if (!isConsole)
	{
		QString file1 = QDir::cleanPath(qApp->applicationDirPath() + "/../bin") + "/AliyunConfig.ini";
		int n = GetPrivateProfileInt(L"Config", L"bAllowSendInfo", 0, file1.toStdWString().c_str());
		QString file2 = QDir::cleanPath(qApp->applicationDirPath())+ "/AliyunConfig.ini";
		WritePrivateProfileString(L"Config", L"bAllowSendInfo", QString::number(n).toStdWString().c_str(), file2.toStdWString().c_str());
	}

	QFont font("Segoe UI");
	qApp->setFont(font);

	QString path = QDir(qApp->applicationDirPath()).absolutePath() + "/DiskHealth.rcc";
	QResource::registerResource(path);

	//QQmlApplicationEngine engine;
	CDiskHealthMainWin mainWin;

	DiskInfoMgr dim;
	//dim.setEngine(&engine);
	dim.setMaminWin(&mainWin);
	dim.initMultiLanguage();
	dim.setCurrrentLanguage(dim.getCurrentLanguage());
	//engine.rootContext()->setContextProperty("diskInfoMgr", &dim);

	mainWin.show();
	if ( !isConsole)
		//engine.load(QUrl(QString("qrc:/qml/main.qml")));

    return a.exec();
}
