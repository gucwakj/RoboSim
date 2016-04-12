#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
	// scale for high DPI
	#ifdef Q_OS_WIN
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	#endif

	// create main app
	QApplication a(argc, argv);

	// create window and show
	MainWindow w;
	w.showMaximized();

	return a.exec();
}
