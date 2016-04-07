#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
	// scale for high DPI
	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

	// create main app
	QApplication a(argc, argv);

	// create window and show
	MainWindow w;
	w.showMaximized();

	return a.exec();
}
