#include <QFileDialog>

#include "mainwindow.h"
#include "obstacleeditor.h"
#include "obstaclemodel.h"
#include "obstacleview.h"
#include "platformselector.h"
#include "roboteditor.h"
#include "robotmodel.h"
#include "robotview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	// set up UI from forms file
	ui = new Ui::MainWindow;
	ui->setupUi(this);

	// insert platform selector
	ui->layout_lhs->insertWidget(0, new platformSelector(this));

	// build robot selector
	QStringList names, icons;
	names << "Linkbot I" <<  "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	icons << "linkbotI.jpg" << "linkbotL.jpg" << "mobot.jpg" << "mobot.jpg";
	this->build_selector(ui->list_robots, names, icons);
	ui->list_robots->setDragEnabled(true);

	// build preconfig selector
	/*names.clear();
	icons.clear();
	names << "Bow" << "Explorer" << "Four Bot Drive" << "Four Wheel Drive" << "Four Wheel Explorer" << "Group Bow" << "Inchworm" << "Lift" << "Omnidrive" << "Snake" << "Stand";
	icons << "bow.jpg" <<  "explorer.jpg" << "fourbotdrive.jpg" << "fourwheeldrive.jpg" << "fourwheelexplorer.jpg" << "groupbow.jpg" << "inchworm.jpg" << "lift.jpg" << "omnidrive.jpg" << "snake.jpg" << "stand.jpg";
	this->build_selector(ui->list_preconfig, names, icons);
	ui->list_preconfig->setDragEnabled(true);*/

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
	this->build_selector(ui->list_drawings, names, icons);
	ui->list_drawings->setDragEnabled(true);

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

	// set up obstacle model
	obstacleModel *o_model = new obstacleModel(this);

	// set up obstacle view
	obstacleView *o_view = new obstacleView(o_model);
	ui->layout_obstacles->addWidget(o_view);

	// set up obstacle editor
	obstacleEditor *o_editor = new obstacleEditor(o_model);
	ui->layout_obstacles->addWidget(o_editor);

	// set up background view
	ui->backgroundListWidget->addItem(new QListWidgetItem(tr("None")));
	ui->backgroundListWidget->addItem(new QListWidgetItem(tr("Outdoors")));
	ui->backgroundListWidget->addItem(new QListWidgetItem(tr("Board")));
	ui->backgroundListWidget->setCurrentRow(1);

	// set up osg view
	ui->osgWidget->setRobotModel(model);
	ui->osgWidget->setObstacleModel(o_model);

	// set up xml parser
	_xml = new xmlParser(rsXML::getDefaultPath());
	_xml->setRobotModel(model);
	_xml->setObstacleModel(o_model);

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
	QWidget::connect(ui->button_grid_defaults, SIGNAL(clicked()), this, SLOT(grid_defaults()));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->tab_scene, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->toolBox_config, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->tab_scene, SIGNAL(currentChanged(int)), this, SLOT(changeIndices(int)));
	QWidget::connect(ui->backgroundListWidget, SIGNAL(currentRowChanged(int)), ui->osgWidget, SLOT(setCurrentBackground(int)));
	QWidget::connect(ui->tracing, SIGNAL(toggled(bool)), _xml, SLOT(setTrace(bool)));

	// menu actions
	QWidget::connect(ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
	QWidget::connect(ui->action_AboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	QWidget::connect(ui->action_Load, SIGNAL(triggered()), this, SLOT(load()));
	QWidget::connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(save()));
	QWidget::connect(ui->action_SaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
	QWidget::connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

	// connect xml parser to gui elements
	QWidget::connect(_xml, SIGNAL(trace(bool)), ui->tracing, SLOT(setChecked(bool)));
	QWidget::connect(_xml, SIGNAL(units(bool)), ui->si, SLOT(setChecked(bool)));
	QWidget::connect(_xml, SIGNAL(grid(std::vector<double>)), this, SLOT(grid(std::vector<double>)));
	QWidget::connect(_xml, SIGNAL(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, std::string)), model, SLOT(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, std::string)));
	QWidget::connect(_xml, SIGNAL(newWheel(int, int, int, double)), model, SLOT(newWheel(int, int, int, double)));
	QWidget::connect(_xml, SIGNAL(newObstacle(int, int, double*, double*, double*, double*, double)), o_model, SLOT(newObstacle(int, int, double*, double*, double*, double*, double)));
	QWidget::connect(_xml, SIGNAL(newMarker(int, int, double*, double*, double*, int, std::string)), o_model, SLOT(newMarker(int, int, double*, double*, double*, int, std::string)));

	// connect robot pieces together
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(indexChanged(const QModelIndex&)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(indexChanged(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentRobotIndex(const QModelIndex&)));
	QWidget::connect(ui->osgWidget, SIGNAL(robotIndexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(ui->osgWidget, SIGNAL(robotIndexChanged(QModelIndex)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteRobotIndex(QModelIndex, int, int)));
	QWidget::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteRobotIndex(QModelIndex, int, int)));

	// connect obstacle pieces together
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObstacleIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObstacleIndex(const QModelIndex&)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentObstacleIndex(const QModelIndex&)));
	QWidget::connect(ui->osgWidget, SIGNAL(obstacleIndexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(ui->osgWidget, SIGNAL(obstacleIndexChanged(QModelIndex)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), o_editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(obstacleDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(obstacleDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(o_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteObstacleIndex(QModelIndex, int, int)));
	QWidget::connect(o_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteObstacleIndex(QModelIndex, int, int)));

	// parse xml file
	_xml->parse("");

	// if no robots were added, add a token Linkbot-I now
	if (!model->rowCount())
		model->addRobot(rs::LINKBOTI);

	// highlight and show first robot found
	ui->osgWidget->setCurrentIndex(0);
	editor->setCurrentIndex(model->index(0,0));

	// parsing of xml complete
	ui->statusBar->showMessage(tr("Ready"), 2000);
}

MainWindow::~MainWindow(void) {
	delete _xml;
	delete ui;
}

void MainWindow::build_selector(QListWidget *widget, QStringList &names, QStringList &icons) {
	for (int i = 0; i < names.size(); i++) {
		QListWidgetItem *button = new QListWidgetItem(widget);
		button->setIcon(QIcon(icons[i]));
		button->setText(names[i]);
		button->setTextAlignment(Qt::AlignCenter);
		button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
	}
}

void MainWindow::changeIndices(int ind) {
	ui->toolBox_config->setCurrentIndex(ind-2);
	ui->osgWidget->setCurrentIndex(ind-2);
}

void MainWindow::grid_defaults(void) {
	// reset to proper units
	if (ui->si->isDown()) {
		ui->spin_grid_tics->setValue(5);
		ui->spin_grid_hash->setValue(50);
		ui->spin_grid_x_min->setValue(-200);
		ui->spin_grid_x_max->setValue(200);
		ui->spin_grid_y_min->setValue(-200);
		ui->spin_grid_y_max->setValue(200);
	}
	else {
		ui->spin_grid_tics->setValue(1);
		ui->spin_grid_hash->setValue(12);
		ui->spin_grid_x_min->setValue(-48);
		ui->spin_grid_x_max->setValue(48);
		ui->spin_grid_y_min->setValue(-48);
		ui->spin_grid_y_max->setValue(48);
	}

	// change osg grid lines
	ui->osgWidget->gridDefaults();
}

void MainWindow::set_units(bool si) {
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
}

void MainWindow::grid(std::vector<double> v) {
	ui->grid_on->setChecked(static_cast<bool>(v[6]));
	ui->spin_grid_tics->setValue(v[0]);
	ui->spin_grid_hash->setValue(v[1]);
	ui->spin_grid_x_min->setValue(v[2]);
	ui->spin_grid_x_max->setValue(v[3]);
	ui->spin_grid_y_min->setValue(v[4]);
	ui->spin_grid_y_max->setValue(v[5]);
}

void MainWindow::about(void) {
	QMessageBox::about(this, tr("About RoboSim"),
		tr("<h3>About RoboSim</h3><p><b>RoboSim</b> is a robot simulation environment, developed by the UC"
		"Davis Center for Integrated Computing and STEM Education (C-STEM) <a"
		"href=\"%1\"</a>%1</a>, for programming Barobo Mobot and Linkbot.  The same Ch"
		"program can control hardware robots or virtual robots in RoboSim without any"
		"modification.</p>"
		"<p>Copyright (C) %2 The C-STEM Center</p>"
		"<p><b>Authors:</b> Kevin Gucwa</p>"
		).arg(QLatin1String("http://cstem.ucdavis.edu"),
		QLatin1String("2015")
		));
}

void MainWindow::load(void) {
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
		_xml->parse(fileName.toStdString().c_str());
		ui->statusBar->showMessage(tr("Loaded %1").arg(fileName), 2000);
	}
}

void MainWindow::save(void) {
	_xml->save();
}

bool MainWindow::saveAs(void) {
	QFileDialog dialog(this);
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	else
		return false;

	bool retval = _xml->saveFile(files.at(0));
	ui->statusBar->showMessage(tr("Saved %1").arg(files.at(0)), 2000);
	return retval;
}

