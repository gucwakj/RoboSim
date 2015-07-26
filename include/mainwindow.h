#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

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

#endif // MAINWINDOW_H_

