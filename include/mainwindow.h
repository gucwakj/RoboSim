#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
		Q_OBJECT
	public:
		explicit MainWindow(QWidget* = 0);
		~MainWindow(void);

	private:
		Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

