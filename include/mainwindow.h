#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListView>
#include <QListWidget>

#include "xmlparser.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
		Q_OBJECT
	public:
		explicit MainWindow(QWidget* = 0);
		~MainWindow(void);

	protected slots:
		void changeIndices(int);
		void grid_defaults(void);
		void set_units(bool);
		void grid(std::vector<double>);
		void load(void);
		void save(void);
		bool saveAs(void);

	private:
		void build_selector(QListWidget*, QStringList&, QStringList&);

	private:
		Ui::MainWindow *ui;
		std::vector<double> _us;
		std::vector<double> _si;
		xmlParser *_xml;
};

#endif // MAINWINDOW_H
