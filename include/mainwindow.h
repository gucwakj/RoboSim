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
		void addBackground(void);
		void changeIndices(int);
		void closeEvent(QCloseEvent*);
		void grid_defaults(void);
		void set_units(bool);
		void grid(std::vector<double>);
		void setCurrentBackground(std::string);
		void about(void);
		void load(void);
		void save(void);
		bool saveAs(void);

	private:
		void build_selector(QListWidget*, QStringList&, QStringList&);
		void load_settings(void);
		void save_settings(void);

	private:
		Ui::MainWindow *ui;
		QString _settings;
		std::vector<double> _us;
		std::vector<double> _si;
		xmlParser *_xml;
		QStringList _background;
};

#endif // MAINWINDOW_H
