#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

#include "robosimwidget.h"
#include "objecteditor.h"
#include "objectview.h"
#include "platformselector.h"
#include "roboteditor.h"
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
	names << "Linkbot I" <<  "Linkbot L" << "Mindstorms EV3/NXT";
	icons << "icons/linkbotI128.png" << "icons/linkbotL128.png" << "icons/mindstormsNXT128.png";
	this->build_selector(ui->list_robots, names, icons);
	ui->list_robots->setDragEnabled(true);

	// build preconfig selector
	names.clear();
	icons.clear();
	names << "Bow" << "Bug Clock" << "Explorer" << "Four Bot Drive" << "Four Wheel Drive" << "Four Wheel Explorer" << "Group Bow" << "Inchworm" << "Lift" << "Omnidrive" << "Snake" << "Stand";
	icons << "icons/bow128.png" << "icons/bugclock128.png" << "icons/explorer128.png" << "icons/fourbotdrive128.png" << "icons/fourwheeldrive128.png" << "icons/fourwheelexplorer128.png" << "icons/groupbow128.png" << "icons/inchworm128.png" << "icons/lift128.png" << "icons/omnidrive128.png" << "icons/snake128.png" << "icons/stand128.png";
	this->build_selector(ui->list_preconfig, names, icons);
	ui->list_preconfig->setDragEnabled(true);

	// build obstacles selector
	names.clear();
	icons.clear();
	names << "Box" << "Competition Border" << "Cylinder" << "Hacky Sack" << "Pullup Bar" << "Sphere" << "Wood Block";
	icons << "icons/box128.png" << "icons/competitionborder128.png" << "icons/cylinder128.png" << "icons/hackysack128.png" << "icons/pullup128.png" << "icons/sphere128.png" << "icons/woodblock128.png";
	this->build_selector(ui->list_obstacles, names, icons);
	ui->list_obstacles->setDragEnabled(true);

	// build drawings selector
	names.clear();
	icons.clear();
	names << "Arc" << "Arc Sector" << "Arc Segment" << "Arrow" << "Circle" << "Ellipse" << "Line" << "Point" <<  "Polygon" << "Rectangle" << "Text" << "Triangle";
	icons << "icons/line128.png" << "icons/line128.png" << "icons/line128.png" << "icons/line128.png" << "icons/line128.png" << "icons/line128.png" << "icons/line128.png" << "icons/point128.png" << "icons/line128.png" << "icons/line128.png" << "icons/text128.png" << "icons/text128.png";
	this->build_selector(ui->list_markers, names, icons);
	ui->list_markers->setDragEnabled(true);

	// set robots as first view in toolbox
	ui->toolBox_config->setCurrentIndex(0);

	// set configuration as first view in scene editor tabwidget
	ui->tab_scene->setCurrentIndex(0);
	QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sp.setVerticalStretch(1);
	ui->tab_scene->setSizePolicy(sp);

	// set up robot model
	_rob_model = new robotModel(this);

	// set up robot view
	robotView *view = new robotView(_rob_model);
	ui->layout_robots->addWidget(view);

	// set up robot editor
	robotEditor *editor = new robotEditor(_rob_model);
	ui->layout_robots->addWidget(editor);

	// set up object model
	_obj_model = new objectModel(this);

	// set up object view
	objectView *o_view = new objectView(_obj_model);
	ui->layout_objects->addWidget(o_view);

	// set up object editor
	objectEditor *o_editor = new objectEditor(_obj_model);
	ui->layout_objects->addWidget(o_editor);

	// set up background view
	int num_def_backgrounds = 6;
	// if defaults aren't all there, delete and restart
	if (!_background.empty() &&
			(	_background[0].toStdString().compare(0, 8, "outdoors") ||
				_background[1].toStdString().compare(0, 6, "barobo") ||
				_background[2].toStdString().compare(0, 4, "2016") ||
				_background[3].toStdString().compare(0, 4, "2015") ||
				_background[4].toStdString().compare(0, 4, "2014") ||
				_background[5].toStdString().compare(0, 4, "none")
			)
		) {
		_background.clear();
	}
	// parse default path
	if (_background.empty()) {
		_background << QString("outdoors");
		_background << QString("baroboactivitymat");
		_background << QString("2016RoboPlay");
		_background << QString("2015RoboPlay");
		_background << QString("2014RoboPlay");
		_background << QString("none");
	}
	// remove any stale custom directories (ignore default ones)
	for (int i = num_def_backgrounds; i < _background.size(); i++) {
		QString file(_background[i]);
		QFileInfo checkFile(file.append("/background.xml"));
		if (!checkFile.exists()) _background.removeAt(i);
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
		if (i >= num_def_backgrounds) _watcher.addPath(_background[i]);
	}
	ui->backgroundListWidget->setDragEnabled(false);

	// set up RoboPlay Challenges list
	QTreeWidgetItem *rc16 = new QTreeWidgetItem(ui->tree_challenges);
	rc16->setText(0, "2016");
	rc16->setText(1, "2016 RoboPlay Challenges");
	QTreeWidgetItem *rc16div1 = this->build_challenge_folder(rc16, "Division 1", "");
	this->build_challenge(rc16div1, "Thruster Control", "20 points");
	this->build_challenge(rc16div1, "X Marks the Spot", "30 points");
	this->build_challenge(rc16div1, "Docking", "35 points");
	this->build_challenge(rc16div1, "Supply Recovery", "45 points");
	this->build_challenge(rc16div1, "Rocket Intercept", "50 points");
	this->build_challenge(rc16div1, "Satellite Tracking", "65 points");
	this->build_challenge(rc16div1, "Rescue Retrieval", "70 points");
	this->build_challenge(rc16div1, "Terrain Excavator", "80 points");
	this->build_challenge(rc16div1, "Go Robobo", "100 points");
	this->build_challenge(rc16div1, "Exhaust Port Targeting", "120 points");
	QTreeWidgetItem *rc16div2 = this->build_challenge_folder(rc16, "Division 2", "");
	this->build_challenge(rc16div2, "Supply Recovery", "20 points");
	this->build_challenge(rc16div2, "Newtons Cradle", "25 points");
	this->build_challenge(rc16div2, "Rocket Intercept", "35 points");
	this->build_challenge(rc16div2, "Collision Simulation", "40 points");
	this->build_challenge(rc16div2, "Satellite Tracking", "55 points");
	this->build_challenge(rc16div2, "Empty Tank Disposal", "60 points");
	this->build_challenge(rc16div2, "Habitat Construction", "70 points");
	this->build_challenge(rc16div2, "Asteroid Orbital", "80 points");
	this->build_challenge(rc16div2, "Supply Run", "90 points");
	this->build_challenge(rc16div2, "Exhaust Port Targeting", "120 points");
	QTreeWidgetItem *rc16div3 = this->build_challenge_folder(rc16, "Division 3", "");
	this->build_challenge(rc16div3, "Cosmic Billiards", "20 points");
	this->build_challenge(rc16div3, "Supply Recovery and Delivery", "35 points");
	this->build_challenge(rc16div3, "Rosetta Comet Survey", "50 points");
	this->build_challenge(rc16div3, "Terrain Excavator", "50 points");
	this->build_challenge(rc16div3, "Orbital Docking", "60 points");
	this->build_challenge(rc16div3, "Earth Moon Orbit", "60 points");
	this->build_challenge(rc16div3, "Contacting the Martian", "80 points");
	this->build_challenge(rc16div3, "Asteroid Defense", "80 points");
	this->build_challenge(rc16div3, "Exhaust Port Targeting", "100 points");
	this->build_challenge(rc16div3, "Orbital Satellite Repair", "120 points");
	QTreeWidgetItem *rc15 = new QTreeWidgetItem(ui->tree_challenges);
	rc15->setText(0, "2015");
	rc15->setText(1, "2015 RoboPlay Challenges");
	QTreeWidgetItem *rc15div1 = this->build_challenge_folder(rc15, "Division 1", "");
	this->build_challenge(rc15div1, "Stop on a Dime", "15 points");
	this->build_challenge(rc15div1, "Straight Forward", "25 points");
	this->build_challenge(rc15div1, "Robotic Slide", "40 points");
	this->build_challenge(rc15div1, "Robot Delivery Service", "40 points");
	this->build_challenge(rc15div1, "Destination Unknown", "60 points");
	this->build_challenge(rc15div1, "Bug Time", "75 points");
	this->build_challenge(rc15div1, "Speed Racer", "80 points");
	this->build_challenge(rc15div1, "Tag", "80 points");
	this->build_challenge(rc15div1, "Robo Pullups", "100 points");
	this->build_challenge(rc15div1, "Stacking Challenge", "120 points");
	QTreeWidgetItem *rc15div2 = this->build_challenge_folder(rc15, "Division 2", "");
	this->build_challenge(rc15div2, "Landing Zone", "40 points");
	this->build_challenge(rc15div2, "Newtons Cradle", "55 points");
	this->build_challenge(rc15div2, "Robo Pullups", "60 points");
	this->build_challenge(rc15div2, "Speed Racer", "60 points");
	this->build_challenge(rc15div2, "Navigate the Oval", "60 points");
	this->build_challenge(rc15div2, "Meet Ya There", "75 points");
	this->build_challenge(rc15div2, "Bug Time", "75 points");
	this->build_challenge(rc15div2, "Stacking Challenge", "90 points");
	this->build_challenge(rc15div2, "Retrieve Supplies", "100 points");
	this->build_challenge(rc15div2, "Synchronized Roboting", "120 points");
	QTreeWidgetItem *rc15div3 = this->build_challenge_folder(rc15, "Division 3", "");
	this->build_challenge(rc15div3, "Newtons Cradle", "40 points");
	this->build_challenge(rc15div3, "Robo Pullups", "40 points");
	this->build_challenge(rc15div3, "Navigate the Figure Eighter", "40 points");
	this->build_challenge(rc15div3, "Stacking Challenge", "60 points");
	this->build_challenge(rc15div3, "Synchronized Roboting", "60 points");
	this->build_challenge(rc15div3, "Flash Mob", "70 points");
	this->build_challenge(rc15div3, "Speed Racer", "65 points");
	this->build_challenge(rc15div3, "Retrieving Supplies from Drop Site", "100 points");
	this->build_challenge(rc15div3, "Bug Clock", "100 points");
	this->build_challenge(rc15div3, "Tired of Wheels", "140 points");
	QTreeWidgetItem *rc14 = new QTreeWidgetItem(ui->tree_challenges);
	rc14->setText(0, "2014");
	rc14->setText(1, "2014 RoboPlay Challenges");
	QTreeWidgetItem *rc14div1 = this->build_challenge_folder(rc14, "Division 1", "Middle School");
	this->build_challenge(rc14div1, "Hit the Wall", "15 points");
	this->build_challenge(rc14div1, "Disco", "30 points");
	this->build_challenge(rc14div1, "Chicken", "30 points");
	this->build_challenge(rc14div1, "Bounce", "40 points");
	this->build_challenge(rc14div1, "Speed Racer", "60 points");
	this->build_challenge(rc14div1, "Racetrack", "60 points");
	this->build_challenge(rc14div1, "High Five Low Five", "60 points");
	this->build_challenge(rc14div1, "Steal the Gold", "80 points");
	this->build_challenge(rc14div1, "The Grid", "95 points");
	this->build_challenge(rc14div1, "Transport", "115 points");
	QTreeWidgetItem *rc14div2 = this->build_challenge_folder(rc14, "Division 2", "High School");
	this->build_challenge(rc14div2, "Bounce", "40 points");
	this->build_challenge(rc14div2, "Steal the Gold", "50 points");
	this->build_challenge(rc14div2, "Racetrack", "60 points");
	this->build_challenge(rc14div2, "C U Soon", "50 points");
	this->build_challenge(rc14div2, "High Five Low Five", "60 points");
	this->build_challenge(rc14div2, "Get'n Dizzy Wit It", "80 points");
	this->build_challenge(rc14div2, "Tortoise and the Hare", "80 points");
	this->build_challenge(rc14div2, "The Grid", "95 points");
	this->build_challenge(rc14div2, "Transport", "115 points");
	this->build_challenge(rc14div2, "Alley Oop", "120 points");
	ui->tree_challenges->expandItem(rc16);
	ui->tree_challenges->expandItem(rc16div1);
	ui->tree_challenges->expandItem(rc16div2);
	ui->tree_challenges->expandItem(rc16div3);
	ui->tree_challenges->expandItem(rc15);
	ui->tree_challenges->expandItem(rc15div1);
	ui->tree_challenges->expandItem(rc15div2);
	ui->tree_challenges->expandItem(rc15div3);
	ui->tree_challenges->expandItem(rc14);
	ui->tree_challenges->expandItem(rc14div1);
	ui->tree_challenges->expandItem(rc14div2);
	ui->tree_challenges->resizeColumnToContents(0);
	ui->tree_challenges->collapseItem(rc16div1);
	ui->tree_challenges->collapseItem(rc16div2);
	ui->tree_challenges->collapseItem(rc16div3);
	ui->tree_challenges->collapseItem(rc15div1);
	ui->tree_challenges->collapseItem(rc15div2);
	ui->tree_challenges->collapseItem(rc15div3);
	ui->tree_challenges->collapseItem(rc14div1);
	ui->tree_challenges->collapseItem(rc14div2);

	// set up osg view
	ui->osgWidget->setRobotModel(_rob_model);
	ui->osgWidget->setObjectModel(_obj_model);

	// set up xml parser
	_xml = new xmlParser(rsXML::getDefaultPath());
	_xml->setRobotModel(_rob_model);
	_xml->setObjectModel(_obj_model);

	// set up default grid units
	_us.push_back(1);
	_us.push_back(12);
	_us.push_back(-48);
	_us.push_back(48);
	_us.push_back(-48);
	_us.push_back(48);
	_si.push_back(5);
	_si.push_back(50);
	_si.push_back(-150);
	_si.push_back(150);
	_si.push_back(-150);
	_si.push_back(150);

	// connect designer elements to slots
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), _rob_model, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), editor, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), _obj_model, SLOT(setUnits(bool)));
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
	QWidget::connect(ui->backgroundListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(setBackground(QListWidgetItem*, QListWidgetItem*)));
	QWidget::connect(ui->add_background, SIGNAL(clicked(void)), this, SLOT(addBackground(void)));
	QWidget::connect(ui->tracing, SIGNAL(toggled(bool)), _xml, SLOT(setTrace(bool)));
	QWidget::connect(ui->tree_challenges, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(setNewChallenge(QTreeWidgetItem*, QTreeWidgetItem*)));
	QWidget::connect(ui->list_robots, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addItem(QListWidgetItem*)));
	QWidget::connect(ui->list_preconfig, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addItem(QListWidgetItem*)));
	QWidget::connect(ui->list_obstacles, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addItem(QListWidgetItem*)));
	QWidget::connect(ui->list_markers, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addItem(QListWidgetItem*)));

	// file watcher of background files
	QWidget::connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateBackgroundList(QString)));

	// connect xml parser to gui elements
	QWidget::connect(_xml, SIGNAL(backgroundImage(int, std::string)), ui->osgWidget, SLOT(setBackgroundImage(int, std::string)));
	QWidget::connect(_xml, SIGNAL(grid(std::vector<float>)), this, SLOT(grid(std::vector<float>)));
	QWidget::connect(_xml, SIGNAL(gridDefaults()), this, SLOT(grid_defaults()));
	QWidget::connect(_xml, SIGNAL(level(int)), ui->osgWidget, SLOT(setCurrentBackground(int)));
	QWidget::connect(_xml, SIGNAL(backgroundName(std::string)), this, SLOT(setCurrentBackground(std::string)));
	QWidget::connect(_xml, SIGNAL(newPreconfig(int, int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, std::string)), _rob_model, SLOT(newPreconfig(int, int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, std::string)));
	QWidget::connect(_xml, SIGNAL(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, const rs::Vec&, int, std::string)), _rob_model, SLOT(newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, const rs::Vec&, int, std::string)));
	QWidget::connect(_xml, SIGNAL(newObstacle(int, int, rs::Pos, rs::Quat, rs::Vec, rs::Vec, double)), _obj_model, SLOT(newObstacle(int, int, rs::Pos, rs::Quat, rs::Vec, rs::Vec, double)));
	QWidget::connect(_xml, SIGNAL(newMarker(int, int, rs::Pos, rs::Pos, rs::Vec, int, std::string)), _obj_model, SLOT(newMarker(int, int, rs::Pos, rs::Pos, rs::Vec, int, std::string)));
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
	QWidget::connect(_rob_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), view, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_rob_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_rob_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_rob_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(robotDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_rob_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteRobotIndex(QModelIndex, int, int)));
	QWidget::connect(_rob_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteRobotIndex(QModelIndex, int, int)));
	QWidget::connect(_rob_model, SIGNAL(reid(QModelIndex)), _xml, SLOT(reidRobot(QModelIndex)));
	QWidget::connect(_rob_model, SIGNAL(reid(QModelIndex)), ui->osgWidget, SLOT(reidRobot(QModelIndex)));

	// connect object pieces together
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(o_view, SIGNAL(indexChanged(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(o_editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentObjectIndex(const QModelIndex&)));
	QWidget::connect(ui->osgWidget, SIGNAL(objectIndexChanged(QModelIndex)), o_view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(ui->osgWidget, SIGNAL(objectIndexChanged(QModelIndex)), o_editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(_obj_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), o_view, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_obj_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), o_editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_obj_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(objectDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_obj_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), _xml, SLOT(objectDataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(_obj_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), ui->osgWidget, SLOT(deleteObjectIndex(QModelIndex, int, int)));
	QWidget::connect(_obj_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), _xml, SLOT(deleteObjectIndex(QModelIndex, int, int)));

	// parse xml file
	_xml->parse();

	// if no robots were added, add a token Linkbot-I now
	if (!_rob_model->rowCount()) _rob_model->addRobot(rs::LinkbotI, 2, 2, 0);

	// highlight and show first robot found
	ui->osgWidget->setCurrentIndex(0);
	editor->setCurrentIndex(_rob_model->index(0,0));

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
		// add to list
		_background << dir;
		// parse rsXML::BackgroundReader
		rsXML::BackgroundReader background(dir.toStdString());
		// make item
		QListWidgetItem *item = new QListWidgetItem(ui->backgroundListWidget);
		item->setIcon(QIcon(background.getScreenshot().c_str()));
		item->setText(background.getName().c_str());
		item->setData(Qt::UserRole, dir);
		// add to watched paths
		_watcher.addPath(dir);
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

void roboSimWidget::build_challenge(QTreeWidgetItem *parent, QString name, QString description) {
	QTreeWidgetItem *treeItem = new QTreeWidgetItem();
	treeItem->setText(0, name);
	treeItem->setText(1, description);
	parent->addChild(treeItem);
}

QTreeWidgetItem* roboSimWidget::build_challenge_folder(QTreeWidgetItem *parent, QString name, QString description) {
	QTreeWidgetItem *treeItem = new QTreeWidgetItem();
	treeItem->setText(0, name);
	treeItem->setText(1, description);
	parent->addChild(treeItem);
	return treeItem;
}

void roboSimWidget::changeIndices(int ind) {
	ui->toolBox_config->setCurrentIndex(ind-2);
	ui->osgWidget->setCurrentIndex(ind-2);
}

void roboSimWidget::grid_defaults(void) {
	// reset to proper units
	if (ui->si->isChecked()) {
		ui->grid_on->setChecked(true);
		ui->spin_grid_tics->setValue(5);
		ui->spin_grid_hash->setValue(50);
		ui->spin_grid_x_min->setValue(-150);
		ui->spin_grid_x_max->setValue(150);
		ui->spin_grid_y_min->setValue(-150);
		ui->spin_grid_y_max->setValue(150);
		ui->osgWidget->setUnits(true);
	}
	else {
		ui->grid_on->setChecked(true);
		ui->spin_grid_tics->setValue(1);
		ui->spin_grid_hash->setValue(12);
		ui->spin_grid_x_min->setValue(-48);
		ui->spin_grid_x_max->setValue(48);
		ui->spin_grid_y_min->setValue(-48);
		ui->spin_grid_y_max->setValue(48);
		ui->osgWidget->setUnits(false);
	}
	this->grid_labels(ui->si->isChecked());

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

void roboSimWidget::grid(std::vector<float> v) {
	ui->grid_off->setChecked(!(static_cast<bool>(v[6])));
	ui->spin_grid_tics->setValue(v[0]);
	ui->spin_grid_hash->setValue(v[1]);
	ui->spin_grid_x_min->setValue(v[2]);
	ui->spin_grid_x_max->setValue(v[3]);
	ui->spin_grid_y_min->setValue(v[4]);
	ui->spin_grid_y_max->setValue(v[5]);
	this->grid_labels(ui->si->isChecked());
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
	// count variable
	int i = 0;

	// highlight clicked item
	for (i = 0; i < ui->backgroundListWidget->count(); i++) {
		QListWidgetItem *item = ui->backgroundListWidget->item(i);
		if ( !item->text().compare(name.c_str()) ) {
			ui->backgroundListWidget->setCurrentRow(i);
			ui->osgWidget->setNewBackground(item, item);
			break;
		}
	}

	// default to outdoors if selected one disappeared
	if (i == ui->backgroundListWidget->count()) {
		QListWidgetItem *item = ui->backgroundListWidget->item(0);
		ui->backgroundListWidget->setCurrentRow(0);
		ui->osgWidget->setNewBackground(item, item);
	}

	// disable all grid options when not using outdoors
	/*QObjectList list = ui->group_grid->children();
	QWidget *item;
	for (int i = 0; i < list.size(); i++) {
		if ( (item = dynamic_cast<QWidget*>(list[i])) )
			item->setEnabled(!name.compare("Outdoors"));
	}*/
}

void roboSimWidget::setBackground(QListWidgetItem *current, QListWidgetItem *previous) {
	// get name of new item
	QString name = current->data(Qt::UserRole).toString();

	// disable all grid options when not using outdoors
	/*QObjectList list = ui->group_grid->children();
	QWidget *item;
	for (int i = 0; i < list.size(); i++) {
		if ( (item = dynamic_cast<QWidget*>(list[i])) )
			item->setEnabled(name.contains("outdoors"));
	}*/
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

void roboSimWidget::setNewChallenge(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
	QString challenge = current->data(0, Qt::EditRole).toString();
	QString bread;

	// if challenge is selected
	if (current->parent()) {
		QString division = current->parent()->data(0, Qt::EditRole).toString();
		if (current->parent()->parent()) {
			QString year = current->parent()->parent()->data(0, Qt::EditRole).toString();
			bread.append(year).append("->").append(division).append("->").append(challenge);
			ui->label_current_item->setText(bread);
			// create filepath to load
			QString fileName = rsXML::getDefaultChallengePath().c_str();
			division = division.toLower();
			division.replace(QString(" "), QString(""));
			challenge = challenge.toLower();
			challenge.replace(QString(" "), QString("_"));
			fileName.append(year).append("/").append(division).append("/").append(challenge).append(".xml");
			QFileInfo checkFile(fileName);
			if (checkFile.exists() && checkFile.isFile()) {
				_rob_model->clear();
				_obj_model->clear();
				_xml->parse(fileName.toStdString().c_str());
				emit statusMessage(tr("Loaded %1").arg(fileName), 2000);
			}
		}
		else {
			// none, go back to old one
			ui->label_current_item->setText("None");
			emit statusMessage(tr("No Challenge Loaded"), 2000);
		}
	}
}

void roboSimWidget::about(void) {
	QMessageBox::about(this, tr("About RoboSim"),
		tr("<h3>About RoboSim</h3><p><b>RoboSim</b> is a robot simulation environment, developed by the UC"
		"Davis Center for Integrated Computing and STEM Education (C-STEM) <a"
		"href=\"%1\"</a>%1</a>, for programming Barobo Linkbot and Lego Mindstorms EV3/NXT.  The same Ch"
		"program can control hardware robots or virtual robots in RoboSim without any"
		"modification.</p>"
		"<p>Copyright (C) %2 The C-STEM Center</p>"
		"<p><b>Authors:</b> Kevin Gucwa</p>"
		).arg(QLatin1String("http://cstem.ucdavis.edu"), QLatin1String("2013-2016"))
	);
}

void roboSimWidget::load(void) {
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
		_rob_model->clear();
		_obj_model->clear();
		_xml->parse(fileName.toStdString().c_str());
		emit statusMessage(tr("Loaded %1").arg(fileName), 2000);
	}
}

void roboSimWidget::save(void) {
	_xml->save();
}

bool roboSimWidget::saveAs(void) {
	QFileDialog dialog(this);
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilter("RoboSim XML Files (*.xml)");
	dialog.selectFile("untitled.xml");
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

void roboSimWidget::addItem(QListWidgetItem *item) {
	if (!item->text().compare("Linkbot I"))
		_rob_model->addRobot(rs::LinkbotI, 2, 2, 0);
	else if (!item->text().compare("Linkbot L"))
		_rob_model->addRobot(rs::LinkbotL);
	else if (!item->text().compare("Mindstorms EV3/NXT"))
		_rob_model->addRobot(rs::NXT);
	else if (!item->text().compare("Bow"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Bow);
	else if (!item->text().compare("Bug Clock"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::BugClock);
	else if (!item->text().compare("Explorer"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Explorer);
	else if (!item->text().compare("Four Bot Drive"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::FourBotDrive);
	else if (!item->text().compare("Four Wheel Drive"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::FourWheelDrive);
	else if (!item->text().compare("Four Wheel Explorer"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::FourWheelExplorer);
	else if (!item->text().compare("Group Bow"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::GroupBow);
	else if (!item->text().compare("Inchworm"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Inchworm);
	else if (!item->text().compare("Lift"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Lift);
	else if (!item->text().compare("Omnidrive"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Omnidrive);
	else if (!item->text().compare("Snake"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Snake);
	else if (!item->text().compare("Stand"))
		_rob_model->addPreconfig(rsLinkbot::Preconfigs::Stand);
	else if (!item->text().compare("Arc"))
		_obj_model->addObject(rs::Arc);
	else if (!item->text().compare("Arc Sector"))
		_obj_model->addObject(rs::ArcSector);
	else if (!item->text().compare("Arc Segment"))
		_obj_model->addObject(rs::ArcSegment);
	else if (!item->text().compare("Arrow"))
		_obj_model->addObject(rs::Arrow);
	else if (!item->text().compare("Box"))
		_obj_model->addObject(rs::Box);
	else if (!item->text().compare("Circle"))
		_obj_model->addObject(rs::Circle);
	else if (!item->text().compare("Competition Border"))
		_obj_model->addObject(rs::CompetitionBorder);
	else if (!item->text().compare("Cylinder"))
		_obj_model->addObject(rs::Cylinder);
	else if (!item->text().compare("Point"))
		_obj_model->addObject(rs::Dot);
	else if (!item->text().compare("Ellipse"))
		_obj_model->addObject(rs::Ellipse);
	else if (!item->text().compare("Hacky Sack"))
		_obj_model->addObject(rs::HackySack);
	else if (!item->text().compare("Line"))
		_obj_model->addObject(rs::Line);
	else if (!item->text().compare("Polygon"))
		_obj_model->addObject(rs::Polygon);
	else if (!item->text().compare("Pullup Bar"))
		_obj_model->addObject(rs::PullupBar);
	else if (!item->text().compare("Rectangle"))
		_obj_model->addObject(rs::Rectangle);
	else if (!item->text().compare("Sphere"))
		_obj_model->addObject(rs::Sphere);
	else if (!item->text().compare("Text"))
		_obj_model->addObject(rs::Text);
	else if (!item->text().compare("Triangle"))
		_obj_model->addObject(rs::Triangle);
	else if (!item->text().compare("Wood Block"))
		_obj_model->addObject(rs::WoodBlock);
}

