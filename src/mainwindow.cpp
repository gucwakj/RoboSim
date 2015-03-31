#include "mainwindow.h"
#include "obstacleeditor.h"
#include "obstaclemodel.h"
#include "obstacleview.h"
#include "roboteditor.h"
#include "robotmodel.h"
#include "robotview.h"
#include "xmlparser.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	// set up UI from forms file
	ui = new Ui::MainWindow;
	ui->setupUi(this);

	// parse xml
	//xmlParser *xml = new xmlParser("/home/kgucwa/.robosimrc");

	// build robot selector
	QStringList names, icons;
	names << "Linkbot I" <<  "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	icons << "linkbotI.jpg" << "linkbotL.jpg" << "mobot.jpg" << "mobot.jpg";
	this->build_selector(ui->list_robots, names, icons);
	ui->list_robots->setDragEnabled(true);

	// build preconfig selector
	names.clear();
	icons.clear();
	names << "Bow" << "Explorer" << "Four Bot Drive" << "Four Wheel Drive" << "Four Wheel Explorer" << "Group Bow" << "Inchworm" << "Lift" << "Omnidrive" << "Snake" << "Stand";
	icons << "bow.jpg" <<  "explorer.jpg" << "fourbotdrive.jpg" << "fourwheeldrive.jpg" << "fourwheelexplorer.jpg" << "groupbow.jpg" << "inchworm.jpg" << "lift.jpg" << "omnidrive.jpg" << "snake.jpg" << "stand.jpg";
	this->build_selector(ui->list_preconfig, names, icons);
	ui->list_preconfig->setDragEnabled(true);

	// build obstacles selector
	names.clear();
	icons.clear();
	names << "Box" <<  "Cylinder" << "Sphere";
	icons << "box.jpg" << "cylinder.jpg" << "sphere.jpg";
	this->build_selector(ui->list_obstacles, names, icons);
	ui->list_obstacles->setDragEnabled(true);

	// build drawings selector
	names.clear();
	icons.clear();
	names << "Line" <<  "Point" << "Text";
	icons << "line.jpg" << "point.jpg" << "text.jpg";
	this->build_selector(ui->list_drawings, names, icons);
	ui->list_drawings->setDragEnabled(true);

	// build scenes selector
	names.clear();
	icons.clear();
	names << "Outdoors" <<  "Board";
	icons << "line.jpg" << "point.jpg";
	this->build_selector(ui->list_scenes, names, icons);

	// set robots as first view in toolbox
	ui->toolBox_config->setCurrentIndex(0);

	// set robots as first view in scene editor tabwidget
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

	// set up robot view
	obstacleView *o_view = new obstacleView(o_model);
	ui->layout_obstacles->addWidget(o_view);

	// set up obstacle editor
	obstacleEditor *o_editor = new obstacleEditor(o_model);
	ui->layout_obstacles->addWidget(o_editor);

	// set up osg view
	ui->osgWidget->setRobotModel(model);
	ui->osgWidget->setObstacleModel(o_model);

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
	QWidget::connect(ui->spin_grid_tics, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridTics(double)));
	QWidget::connect(ui->spin_grid_hash, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridHash(double)));
	QWidget::connect(ui->spin_grid_x_min, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMinX(double)));
	QWidget::connect(ui->spin_grid_x_max, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMaxX(double)));
	QWidget::connect(ui->spin_grid_y_min, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMinY(double)));
	QWidget::connect(ui->spin_grid_y_max, SIGNAL(valueChanged(double)), ui->osgWidget, SLOT(gridMaxY(double)));
	QWidget::connect(ui->grid_on, SIGNAL(toggled(bool)), ui->osgWidget, SLOT(gridEnabled(bool)));
	QWidget::connect(ui->button_grid_defaults, SIGNAL(clicked()), this, SLOT(grid_defaults()));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->tab_scene, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->osgWidget, SIGNAL(currentTab(int)), ui->toolBox_config, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->tab_scene, SIGNAL(currentChanged(int)), ui->toolBox_config, SLOT(setCurrentIndex(int)));
	QWidget::connect(ui->tab_scene, SIGNAL(currentChanged(int)), ui->osgWidget, SLOT(setCurrentIndex(int)));

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
	QWidget::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteRobotIndex(QModelIndex, int, int)));

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
	QWidget::connect(o_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteObstacleIndex(QModelIndex, int, int)));

	// parsing of xml complete
	//ui->statusBar->showMessage(tr("Loaded %1").arg(fileName), 2000);
}

MainWindow::~MainWindow(void) {
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

