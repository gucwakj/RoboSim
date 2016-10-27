#ifndef ROBOSIMWIDGET_H_
#define ROBOSIMWIDGET_H_

#include <QFileSystemWatcher>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QTreeWidgetItem>

#include "objectmodel.h"
#include "robotmodel.h"
#include "xmlparser.h"

#if defined(ROBOSIMWIDGET_DLL) && defined(_WIN32)
	#define ROBOSIMWIDGET_EXPORT Q_DECL_EXPORT
#elif defined(_WIN32)
	#define ROBOSIMWIDGET_EXPORT Q_DECL_IMPORT
#else
	#define ROBOSIMWIDGET_EXPORT
#endif

namespace Ui {
	class RoboSimWidget;
}

class ROBOSIMWIDGET_EXPORT roboSimWidget : public QWidget {
		Q_OBJECT
	public:
		explicit roboSimWidget(QWidget* = 0);
		~roboSimWidget(void);

	signals:
		void statusMessage(const QString&, int);

	protected slots:
		void addBackground(void);
		void addItem(QListWidgetItem*);
		void changeIndices(int);
		void closeEvent(QCloseEvent*);
		void grid_defaults(void);
		void set_units(bool);
		void grid(std::vector<float>);
		void setCurrentBackground(std::string);
		void setGridEnabled(bool);
		void setBackground(QListWidgetItem*, QListWidgetItem*);
		void setNewChallenge(QTreeWidgetItem*, QTreeWidgetItem*);
		void updateBackgroundList(const QString&);
		void about(void);
		void load(void);
		void save(void);
		bool saveAs(void);
		void usersGuide(void);

	private:
		void build_challenge(QTreeWidgetItem*, QString, QString);
		QTreeWidgetItem* build_challenge_folder(QTreeWidgetItem*, QString, QString);
		void build_selector(QListWidget*, QStringList&, QStringList&);
		void load_settings(void);
		void save_settings(void);
		void grid_labels(bool);

	private:
		Ui::RoboSimWidget *ui;
		QString _settings;
		std::vector<double> _us;
		std::vector<double> _si;
		xmlParser *_xml;
		QStringList _background;
		QFileSystemWatcher _watcher;
		robotModel *_rob_model;
		objectModel *_obj_model;
};

#endif // ROBOSIMWIDGET_H_

