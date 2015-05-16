#ifndef ROBOSIMWIDGET_H
#define ROBOSIMWIDGET_H

#include <QFileSystemWatcher>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>

#include "xmlparser.h"

namespace Ui {
	class RoboSimWidget;
}

class roboSimWidget : public QWidget {
		Q_OBJECT
	public:
		explicit roboSimWidget(QWidget* = 0);
		~roboSimWidget(void);

	signals:
		void statusMessage(const QString&, int);

	protected slots:
		void addBackground(void);
		void changeIndices(int);
		void closeEvent(QCloseEvent*);
		void grid_defaults(void);
		void set_units(bool);
		void grid(std::vector<double>);
		void setCurrentBackground(std::string);
		void updateBackgroundList(const QString&);
		void about(void);
		void load(void);
		void save(void);
		bool saveAs(void);

	private:
		void build_selector(QListWidget*, QStringList&, QStringList&);
		void load_settings(void);
		void save_settings(void);

	private:
		Ui::RoboSimWidget *ui;
		QString _settings;
		std::vector<double> _us;
		std::vector<double> _si;
		xmlParser *_xml;
		QStringList _background;
		QFileSystemWatcher _watcher;
};

#endif // ROBOSIMWIDGET_H

