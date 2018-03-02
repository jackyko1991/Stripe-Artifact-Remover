#include <QApplication>

#include "mainWindow.h"

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Jacky Ko");
	QCoreApplication::setApplicationName("Strip Artifact Remover");
	QCoreApplication::setApplicationVersion("v0.1b");

	// start mainwindow
	//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

	MainWindow mainWin;
	mainWin.show();

	return app.exec();


}