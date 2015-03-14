#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QListView>
#include <QListWidget>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
		Q_OBJECT
	public:
		explicit MainWindow(QWidget* = 0);
		~MainWindow(void);

	private:
		void build_selector(QListWidget*, QStringList&, QStringList&);

	private:
		Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
