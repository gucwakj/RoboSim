#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QListView>
#include <QListWidget>

#include "xmldom.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(QWidget* = 0);
		~MainWindow();

	private slots:
		void on_pushButton_clicked();

	private:
		void build_selector(QListWidget*, QStringList&, QStringList&);

	private:
		Ui::MainWindow *ui;
		int _version;
};

#endif // MAINWINDOW_H
