#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

#include "robosimwidget.h"
#include "objecteditor.h"
#include "objectmodel.h"
#include "objectview.h"
#include "platformselector.h"
#include "roboteditor.h"
#include "robotmodel.h"
#include "robotview.h"
#include "ui_robosimwidget.h"

#include <rsXML/BackgroundReader>

roboSimWidget::roboSimWidget(QWidget *parent) : QWidget(parent) {
	// set up UI from forms file
	ui = new Ui::RoboSimWidget;
	ui->setupUi(this);

	// load settings
	this->load_settings();

	// insert platform selector
	ui->layout_lhs->insertWidget(0, new platformSelector(this));

	// build robot selector
	QStringList names, icons;
	names << "Linkbot I" <<  "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	icons << "icons/linkbotI128.png" << "icons/linkbotL128.png" << "icons/mindstormsEV3128.png" << "icons/mindstormsNXT128.png";
	this->build_selector(ui->list_robots, names, icons);
	ui->list_robots->setDragEnabled(true);

	// build preconfig selector
	//names.clear();
	//icons.clear();
	//names << "Bow" << "Explorer" << "Four Bot Drive" << "Four Wheel Drive" << "Four Wheel Explorer" << "Group Bow" << "Inchworm" << "Lift" << "Omnidrive" << "Snake" << "Stand";
	//icons << "bow.jpg" <<  "explorer.jpg" << "fourbotdrive.jpg" << "fourwheeldrive.jpg" << "fourwheelexplorer.jpg" << "groupbow.jpg" << "inchworm.jpg" << "lift.jpg" << "omnidrive.jpg" << "snake.jpg" << "stand.jpg";
	//this->build_selector(ui->list_preconfig, names, icons);
	//ui->list_preconfig->setDragEnabled(true);

	// build obstacles selector
	names.clear();
	icons.clear();
	names << "Box" <<  "Cylinder" << "Sphere";
	icons << "icons/box128.png" << "icons/cylinder128.png" << "icons/sphere128.png";
	this->build_selector(ui->list_obstacles, names, icons);
	ui->list_obstacles->setDragEnabled(true);

	// build drawings selector
	names.clear();
	icons.clear();
	names << "Line" <<  "Point" << "Text";
	icons << "icons/line128.png" << "icons/point128.png" << "icons/text128.png";
	this->build_selector(ui->list_markers, names, icons);
	ui->list_markers->setDragEnabled(true);

	// set robots as first view in toolbox
	ui->toolBox_config->setCurrentIndex(0);

	// set configuration as first view in scene editor tabwidget
	ui->tab_scene->setCurrentIndex(0);

	// set up robot model
	robotModel *model = new robotModel(this);

	// set up robot view
	robotView *view = new robotView(model);
	ui->layout_robots->addWidget(view);

	// set up robot editor
	robotEditor *editor = new robotEditor(model);
	ui->layout_robots->addWidget(editor);

	// set up object model
	objectModel *o_model = new objectModel(this);

	// set up object view
	objectView *o_view = new objectView(o_model);
	ui->layout_objects->addWidget(o_view);

	// set up object editor
	objectEditor *o_editor = new objectEditor(o_model);
	ui->layout_objects->addWidget(o_editor);

	// set up background view
	// parse default path
	if (_background.empty()) {
		QString parent(rsXML::getDefaultBackgroundPath().c_str());
		QStringList dirs;
		QDirIterator directories(parent, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		while (directories.hasNext()) {
			directories.next();
			dirs << directories.filePath();
		}
		for (int i = 0; i < dirs.size(); i++) {
			QString file(dirs[i]);
			QFileInfo checkFile(file.append("/background.xml"));
			if (checkFile.exists() && checkFile.isFile())
				_background << dirs[i];
		}
	}
	// remove any stale directories
	for (int i = 0; i < _background.size(); i++) {
		QString file(_background[i]);
		QFileInfo checkFile(file.append("/background.xml"));
		if (!checkFile.exists()) _background.removeAt(1);
	}
	// create view list
	for (int i = 0; i < _background.size(); i++) {
		// parse rsXML::BackgroundReader
		rsXML::BackgroundReader background(_background[i].toStdString());
		// make item
		QListWidgetItem *item = new QListWidgetItem(ui->backgroundListWidget);
		item->setIcon(QIcon(background.getScreenshot().c_str()));
		item->setText(background.getName().c_str());
		item->setData(Qt::UserRole, _background[i]);
		// add to watched paths
		_watcher.addPath(_background[i]);
	}

	// set up osg view
	ui->osgWidget->setRobotModel(model);
	ui->osgWidget->setObjectModel(o_model);

	// set up xml parser
	_xml = new xmlParser(rsXML::getDefaultPath());
	_xml->setRobotModel(model);
	_xml->setObjectModel(o_model);

	// set up default grid units
	_us.push_back(1);
	_us.push_back(12);
	_us.push_back(-48);
	_us.push_back(48);
	_us.push_back(-48);
	_us.push_back(48);
	_si.push_back(5);
	_si.push_back(50);
	_si.push_back(-200);
	_si.push_back(200);
	_si.push_back(-200);
	_si.push_back(200);

	// connect designer elements to slots
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), model, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), editor, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), o_model, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), o_editor, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), ui->osgWidget, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), this, SLOT(set_units(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), _xml, SLOT(setUnits(bool)));
	QWidget::connect(ui->spin_grid_tics, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridTics(double)));
	QWidget::connect(ui->spin_grid_tics, SIGNAL(valueChanged(double)), _xml, SLOT(setGridTics(double)));
	QWidget::connect(ui->spin_grid_hash, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridHash(double)));
	QWidget::connect(ui->spin_grid_hash, SIGNAL(valueChanged(double)), _xml, SLOT(setGridHash(double)));
	QWidget::connect(ui->spin_grid_x_min, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMinX(double)));
	QWidget::connect(ui->spin_grid_x_min, SIGNAL(valueChanged(double)), _xml, SLOT(setGridMinX(double)));
	QWidget::connect(ui->spin_grid_x_max, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMaxX(double)));
	QWidget::connect(ui->spin_grid_x_max, SIGNAL(valueChanged(double)), _xml, SLOT(setGridMaxX(double)));
	QWidget::connect(ui->spin_grid_y_min, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMinY(double)));
	QWidget::connect(ui->spin_grid_y_min, SIGNAL(valueChanged(double)), _xml, SLOT(setGridMinY(double)));
	QWidget::connect(ui->spin_grid_y_max, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMaxY(double)));
	QWidget::connect(ui->spin_grid_y_max, SIGNAL(valueChanged(double)), _xml, SLOT(setGridMaxY(double)));
	QWidget::connect(ui->grid_on, SIGNAL(toggled(bool)), ui->osgWidget, SLOT(gridEnabled(bool)));
	QWidget::connect(ui->grid_on, SIGNAL(toggled(bool)), _xml, SLOT(setGridEnabled(bool)));
	QWidget::connect(ui->button_grid_defaults, SIGNAL(clicked()), this, SLOT(grid_defaults()));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->tab_scene, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->toolBox_config, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->tab_scene, SIGNAL(currentChanged(int)), this, SLOT(changeIndices(int)));
	QWidget::connect(ui->backgroundListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), ui->osgWidget, SLOT(setNewBackground(QListWidgetItem*, QListWidgetItem*)));
	QWidget::connect(ui->backgroundListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), _xml, SLOT(setBackground(QListWidgetItem*, QListWidgetItem*)));
	QWidget::connect(ui->add_background, SIGNAL(clicked(void)), this, SLOT(addBackground(void)));
	QWidget::connect(ui->tracing, SIGNAL(toggled(bool)), _xml, SLOT(setTrace(bool)));

	// file watcher of background files
	QWidget::connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateBackgroundList(QString)));

	// connect xml parser to gui elements
	QWidget::connect(_xml, SIGNAL(backgroundImage(int, std::string)), ui->osgWidget, SLOT(setBackgroundImage(int, std::string)));
	QWidget::connect(_xml, SIGNAL(grid(std::vector<double>)), this, SLOT(grid(std::vector<double>)));
	QWidget::connect(_xml, SIGNAL(gridDefaults()), this, SLOT(grid_defaults()));
	QWidget::connect(_xml, SIGNAL(level(int)), ui->osgWidget, SLOT(setCurrentBackground(int)));
	QWidget::connect(_xml, SIGNAL(backgroundName(std::string)), this, SLOT(setCurrentBackground(std::string)));
	QWidget::connect(_xml, SIGNAL(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, const rs::Vec&, std::string)), model, SLOT(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, const rs::Vec&, std::string)));
	QWidget::connect(_xml, SIGNAL(newObstacle(int, int, rs::Pos, rs::Quat, rs::Vec, rs::Vec, double)), o_model, SLOT(newObstacle(int, int, rs::Pos, rs::Quat, rs::Vec, rs::Vec, double)));
	QWidget::connect(_xml, SIGNAL(newMarker(int, int, rs::Pos, rs::Pos, rs::Vec, int, std::string)), o_model, SLOT(newMarker(int, int, rs::Pos, rs::Pos, rs::Vec, int, std::string)));
	QWidget::connect(_xml, SIGNAL(trace(bool)), ui->tracing, SLOT(setChecked(bool)));
	QWidget::connect(_xml, SIGNAL(units(bool)), ui->si, SLOT(setChecked(bool)));

	// connect robot pieces together
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(indexChanged(const QModelIndex&)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(indexChanged(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(ui->osgWidget, SIGNAL(robotIndexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(ui->osgWidget, SIGNAL(robotIndexChanged(QModelIndex)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), view, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteRobotIndex(QModelIndex, int, int)));
	QWidget::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteRobotIndex(QModelIndex, int, int)));

	// connect object pieces together
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(ui->osgWidget, SIGNAL(objectIndexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(ui->osgWidget, SIGNAL(objectIndexChanged(QModelIndex)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), o_view, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), o_editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(objectDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(objectDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteObjectIndex(QModelIndex, int, int)));
	QWidget::connect(o_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteObjectIndex(QModelIndex, int, int)));

	// parse xml file
	_xml->parse("");

	// if no robots were added, add a token Linkbot-I now
	if (!model->rowCount()) model->addRobot(rs::LINKBOTI, 2, 2);

	// highlight and show first robot found
	ui->osgWidget->setCurrentIndex(0);
	editor->setCurrentIndex(model->index(0,0));

	// parsing of xml complete
	emit statusMessage(tr("RoboSim: Ready"), 2000);
}

roboSimWidget::~roboSimWidget(void) {
	this->save_settings();
	delete _xml;
	delete ui;
}

void roboSimWidget::addBackground(void) {
	// get directory from user
	QString dir = QFileDialog::getExistingDirectory(this, tr("Add Background Directory"),
					"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	// check for existance of xml file
	QString xml(dir);
	QFileInfo checkFile(xml.append("/background.xml"));
	if (checkFile.exists()) {
		_background << dir;
	}
	else {
		QMessageBox msgBox;
		msgBox.setText(QString("%1 does not contain a valid RoboSim Background XML File.").arg(dir));
		msgBox.exec();
	}
}

void roboSimWidget::build_selector(QListWidget *widget, QStringList &names, QStringList &icons) {
	for (int i = 0; i < names.size(); i++) {
		QListWidgetItem *button = new QListWidgetItem(widget);
		button->setIcon(QIcon(icons[i]));
		button->setText(names[i]);
		button->setTextAlignment(Qt::AlignCenter);
		button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
	}
}

void roboSimWidget::changeIndices(int ind) {
	ui->toolBox_config->setCurrentIndex(ind-2);
	ui->osgWidget->setCurrentIndex(ind-2);
}

void roboSimWidget::grid_defaults(void) {
	// reset to proper units
	if (ui->si->isDown()) {
		ui->grid_on->setChecked(true);
		ui->spin_grid_tics->setValue(5);
		ui->spin_grid_hash->setValue(50);
		ui->spin_grid_x_min->setValue(-200);
		ui->spin_grid_x_max->setValue(200);
		ui->spin_grid_y_min->setValue(-200);
		ui->spin_grid_y_max->setValue(200);
	}
	else {
		ui->grid_on->setChecked(true);
		ui->spin_grid_tics->setValue(1);
		ui->spin_grid_hash->setValue(12);
		ui->spin_grid_x_min->setValue(-48);
		ui->spin_grid_x_max->setValue(48);
		ui->spin_grid_y_min->setValue(-48);
		ui->spin_grid_y_max->setValue(48);
	}
	this->grid_labels(ui->si->isDown());

	// change osg grid lines
	ui->osgWidget->gridDefaults();
}

void roboSimWidget::set_units(bool si) {
	if ( si &&
		 static_cast<int>(ui->spin_grid_tics->value()) == _us[0] &&
		 static_cast<int>(ui->spin_grid_hash->value()) == _us[1] &&
		 static_cast<int>(ui->spin_grid_x_min->value()) == _us[2] &&
		 static_cast<int>(ui->spin_grid_x_max->value()) == _us[3] &&
		 static_cast<int>(ui->spin_grid_y_min->value()) == _us[4] &&
		 static_cast<int>(ui->spin_grid_y_max->value()) == _us[5] ) {
			ui->spin_grid_tics->setValue(_si[0]);
			ui->spin_grid_hash->setValue(_si[1]);
			ui->spin_grid_x_min->setValue(_si[2]);
			ui->spin_grid_x_max->setValue(_si[3]);
			ui->spin_grid_y_min->setValue(_si[4]);
			ui->spin_grid_y_max->setValue(_si[5]);
	}
	else if ( !si &&
		 static_cast<int>(ui->spin_grid_tics->value()) == _si[0] &&
		 static_cast<int>(ui->spin_grid_hash->value()) == _si[1] &&
		 static_cast<int>(ui->spin_grid_x_min->value()) == _si[2] &&
		 static_cast<int>(ui->spin_grid_x_max->value()) == _si[3] &&
		 static_cast<int>(ui->spin_grid_y_min->value()) == _si[4] &&
		 static_cast<int>(ui->spin_grid_y_max->value()) == _si[5] ) {
			ui->spin_grid_tics->setValue(_us[0]);
			ui->spin_grid_hash->setValue(_us[1]);
			ui->spin_grid_x_min->setValue(_us[2]);
			ui->spin_grid_x_max->setValue(_us[3]);
			ui->spin_grid_y_min->setValue(_us[4]);
			ui->spin_grid_y_max->setValue(_us[5]);
	}
	this->grid_labels(si);
}

void roboSimWidget::grid(std::vector<double> v) {
	ui->grid_off->setChecked(!(static_cast<bool>(v[6])));
	ui->spin_grid_tics->setValue(v[0]);
	ui->spin_grid_hash->setValue(v[1]);
	ui->spin_grid_x_min->setValue(v[2]);
	ui->spin_grid_x_max->setValue(v[3]);
	ui->spin_grid_y_min->setValue(v[4]);
	ui->spin_grid_y_max->setValue(v[5]);
	this->grid_labels(ui->si->isDown());
}

void roboSimWidget::grid_labels(bool si) {
	if (si) {
		ui->units_grid_x_min->setText(tr("cm"));
		ui->units_grid_x_max->setText(tr("cm"));
		ui->units_grid_y_min->setText(tr("cm"));
		ui->units_grid_y_max->setText(tr("cm"));
	}
	else {
		ui->units_grid_x_min->setText(tr("in"));
		ui->units_grid_x_max->setText(tr("in"));
		ui->units_grid_y_min->setText(tr("in"));
		ui->units_grid_y_max->setText(tr("in"));
	}
}

void roboSimWidget::setCurrentBackground(std::string name) {
	for (int i = 0; i < ui->backgroundListWidget->count(); i++) {
		QListWidgetItem *item = ui->backgroundListWidget->item(i);
		if ( !item->text().compare(name.c_str()) ) {
			ui->backgroundListWidget->setCurrentRow(i);
			break;
		}
	}
}

void roboSimWidget::updateBackgroundList(const QString &str) {
	for (int i = 0; i < ui->backgroundListWidget->count(); i++) {
		QListWidgetItem *item = ui->backgroundListWidget->item(i);
		if ( !item->data(Qt::UserRole).toString().compare(str) ) {
			rsXML::BackgroundReader background(str.toStdString());
			item->setIcon(QIcon(background.getScreenshot().c_str()));
			item->setText(background.getName().c_str());
			if (i == ui->backgroundListWidget->currentRow()) {
				ui->osgWidget->setNewBackground(item, item);
			}
			break;
		}
	}
}

void roboSimWidget::about(void) {
	QMessageBox::about(this, tr("About RoboSim"),
		tr("<h3>About RoboSim</h3><p><b>RoboSim</b> is a robot simulation environment, developed by the UC"
		"Davis Center for Integrated Computing and STEM Education (C-STEM) <a"
		"href=\"%1\"</a>%1</a>, for programming Barobo Mobot and Linkbot.  The same Ch"
		"program can control hardware robots or virtual robots in RoboSim without any"
		"modification.</p>"
		"<p>Copyright (C) %2 The C-STEM Center</p>"
		"<p><b>Authors:</b> Kevin Gucwa</p>"
		).arg(QLatin1String("http://cstem.ucdavis.edu"), QLatin1String("2015"))
	);
}

void roboSimWidget::load(void) {
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
		_xml->parse(fileName.toStdString().c_str());
		emit statusMessage(tr("RoboSim: Loaded %1").arg(fileName), 2000);
	}
}

void roboSimWidget::save(void) {
	_xml->save();
}

bool roboSimWidget::saveAs(void) {
	QFileDialog dialog(this);
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	else
		return false;

	bool retval = _xml->saveFile(files.at(0));
	emit statusMessage(tr("RoboSim: Saved %1").arg(files.at(0)), 2000);
	return retval;
}

void roboSimWidget::usersGuide(void) {
	QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("../docs/robosim.pdf").absoluteFilePath()));
}

void roboSimWidget::load_settings(void) {
	QSettings settings(tr("UC Davis C-STEM Center"), tr("RoboSim"));

	//settings.beginGroup("mainwindow");
	//this->resize(settings.value("size", QSize(400, 400)).toSize());
	//this->move(settings.value("pos", QPoint(200, 200)).toPoint());
	//settings.endGroup();

	settings.beginGroup("background");
	int size = settings.beginReadArray("directories");
	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		_background << settings.value("path").toString();
	}
	settings.endArray();
	settings.endGroup();
}

void roboSimWidget::save_settings(void) {
	QSettings settings(tr("UC Davis C-STEM Center"), tr("RoboSim"));

	//settings.beginGroup("mainwindow");
	//settings.setValue("size", size());
	//settings.setValue("pos", pos());
	//settings.endGroup();

	settings.beginGroup("background");
	settings.beginWriteArray("directories");
	for (int i = 0; i < _background.size(); ++i) {
		settings.setArrayIndex(i);
		settings.setValue("path", _background[i]);
	}
	settings.endArray();
	settings.endGroup();
}

void roboSimWidget::closeEvent(QCloseEvent *event) {
	save_settings();
	event->accept();
}

