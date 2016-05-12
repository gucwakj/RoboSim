#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
	// windows specific settings
	#ifdef Q_OS_WIN64
	// scale for high DPI
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// opengl implementation - force it because of surface pro
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	#endif

	// create main app
	QApplication a(argc, argv);

	// create window and show
	MainWindow w;
	w.showMaximized();

	// run app
	return a.exec();
}
