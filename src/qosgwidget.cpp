#include "qosgwidget.h"

#include <QListWidgetItem>

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <rs/Macros>
#include <rsScene/Linkbot>
#include <rsScene/Mindstorms>
#include <rsXML/BackgroundReader>
#include <rsXML/Marker>
#include <rsXML/Obstacle>

/*!
 *
 *
 *	OSG Widget
 *
 *
 */

QOsgWidget::QOsgWidget(QWidget *parent) : osgQt::GLWidget(parent) {
	// create new scene
	_scene = new rsScene::Scene();

	// provide reference count
	this->ref();

	// set size properties
	QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sp.setVerticalStretch(2);
	sp.setHorizontalStretch(3);
	this->setSizePolicy(sp);

	// handle event inputs
	this->installEventFilter(this);

	// set display settings
	osg::ref_ptr<osg::DisplaySettings> ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds.get());
	traits->x = this->x();
	traits->y = this->y();
	traits->width = 640;
	traits->height = 360;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();
	traits->inheritedWindowData = new osgQt::GraphicsWindowQt::WindowData(this);
	_gw = new osgQt::GraphicsWindowQt(traits.get());

	// create viewer
	_scene->setupViewer(dynamic_cast<osgViewer::Viewer*>(this));
	_scene->setupCamera(_gw, traits->width, traits->height);
	_scene->setupScene(traits->width, traits->height, false);
	QMouseHandler *mh = new QMouseHandler(_scene);
	_scene->setMouseHandler(mh);
	QWidget::connect(mh, SIGNAL(clickedObjectIndex(int)), this, SLOT(clickedObjectIndex(int)));
	QWidget::connect(mh, SIGNAL(clickedRobotIndex(int, bool)), this, SLOT(clickedRobotIndex(int, bool)));

	// initialize current indices
	_current[0] = -1;
	_current[1] = -1;
	_current[2] = -1;

	// set highlight on click
	_scene->setHighlight(true);
	// hide global hud
	_scene->setHUD(false);
	// hide global clock
	_scene->setClock(false);
	// set level to display
	_level = rs::Level::Outdoors;
	_scene->setLevel(_level);
	// hide robot popup labels
	_scene->setLabel(false);

	// set grid
	_units = 0;
	_grid.push_back(1/39.37);
	_grid.push_back(12/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(1.0);
	_scene->setUnits(_units);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();

	// create timer to manage redraws
	QWidget::connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	this->_timer.start(25);		// 40 fps
}

QOsgWidget::~QOsgWidget(void) {
	_scene->setMouseHandler(NULL);
}

void QOsgWidget::clickedObjectIndex(int id) {
	QModelIndex index;
	for (int i = 0; i < _o_model->rowCount(); i++) {
		index = _o_model->index(i, rsObjectModel::ID);
		if (_o_model->data(index, Qt::EditRole).toInt() == id) {
			// highlight new item
			if (id != _current[1] || _current[2] == 0) {
				_current[1] = id;
				_scene->addHighlight(_current[1], false);
				emit currentTab(3);
			}
			// deselect current item
			else {
				_current[1] = -1;
				index = _o_model->index(-1, -1);
			}
			// object is valid index
			_current[2] = 1;
			emit objectIndexChanged(index);
			return;
		}
	}
}

void QOsgWidget::clickedRobotIndex(int id, bool highlight) {
	QModelIndex index;
	for (int i = 0; i < _r_model->rowCount(); i++) {
		index = _r_model->index(i, rsRobotModel::ID);
		if (_r_model->data(index, Qt::EditRole).toInt() == id) {
			// highlight new item
			if (id != _current[0] || _current[2] == 1) {
				_current[0] = id;
				if (highlight) this->highlight_robots(index);
				emit currentTab(2);
			}
			// deselect current item
			else {
				_current[0] = -1;
				index = _r_model->index(-1, -1);
			}
			// undo robot index
			_current[2] = 0;
			emit robotIndexChanged(index);
			return;
		}
	}
}

void QOsgWidget::deleteObjectIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _o_model->data(_o_model->index(first, rsObjectModel::ID), Qt::EditRole).toInt();
	_scene->deleteObstacle(id);
	_scene->deleteMarker(id);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::deleteRobotIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _r_model->data(_r_model->index(first, rsRobotModel::ID), Qt::EditRole).toInt();
	int preconfig = _r_model->data(_r_model->index(first, rsRobotModel::PRECONFIG), Qt::EditRole).toInt();
	if (preconfig)
		_scene->deletePreconfig(id);
	else
		_scene->deleteRobot(id);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridDefaults(void) {
	// reset to defaults
	if (_units) {
		_grid[0] = 5.0/100;
		_grid[1] = 50.0/100;
		_grid[2] = -150.0/100;
		_grid[3] = 150.0/100;
		_grid[4] = -150.0/100;
		_grid[5] = 150.0/100;
		_grid[6] = 1;
	}
	else {
		_grid[0] = 1/39.37;
		_grid[1] = 12/39.37;
		_grid[2] = -48/39.37;
		_grid[3] = 48/39.37;
		_grid[4] = -48/39.37;
		_grid[5] = 48/39.37;
		_grid[6] = 1;
	}

	// draw new grid
	_scene->setUnits(_units);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridTics(double value) {
	_grid[0] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridHash(double value) {
	_grid[1] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridMinX(double value) {
	_grid[2] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridMaxX(double value) {
	_grid[3] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridMinY(double value) {
	_grid[4] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridMaxY(double value) {
	_grid[5] = convert(value);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridEnabled(bool enabled) {
	_grid[6] = static_cast<double>(enabled);
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::reidRobot(QModelIndex index) {
	_scene->reidRobot(_r_model->data(_r_model->index(index.row(), rsRobotModel::ID), Qt::EditRole).toInt());
}

void QOsgWidget::robotDataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new robots
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form and id
		int form = _r_model->data(_r_model->index(i, rsRobotModel::FORM)).toInt();
		int id = _r_model->data(_r_model->index(i, rsRobotModel::ID), Qt::EditRole).toInt();
		std::string name = _r_model->data(_r_model->index(i, rsRobotModel::NAME)).toString().toStdString();
		int preconfig = _r_model->data(_r_model->index(i, rsRobotModel::PRECONFIG)).toInt();

		// get position
		rs::Pos p(_r_model->data(_r_model->index(i, rsRobotModel::P_X)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Y)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Z)).toDouble());

		// get euler angles
		double r[3] = {rs::D2R(_r_model->data(_r_model->index(i, rsRobotModel::R_PHI)).toDouble()),
					   rs::D2R(_r_model->data(_r_model->index(i, rsRobotModel::R_THETA)).toDouble()),
					   rs::D2R(_r_model->data(_r_model->index(i, rsRobotModel::R_PSI)).toDouble())};

		// calculate quaternion
		rs::Quat q(sin(0.5*r[0]), 0, 0, cos(0.5*r[0]));
		q.multiply(0, sin(0.5*r[1]), 0, cos(0.5*r[1]));
		q.multiply(0, 0, sin(0.5*r[2]), cos(0.5*r[2]));

		// get led color
		QColor color(_r_model->data(_r_model->index(i, rsRobotModel::COLOR)).toString());
		rs::Vec c(color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0);

		// get wheels
		int wheelID[2] = {_r_model->data(_r_model->index(i, rsRobotModel::WHEELLEFT)).toInt(),
						  _r_model->data(_r_model->index(i, rsRobotModel::WHEELRIGHT)).toInt()};
		double radius = _r_model->data(_r_model->index(i, rsRobotModel::RADIUS)).toDouble();
		int wheel[2] = {0};
		int casterID = _r_model->data(_r_model->index(i, rsRobotModel::CASTER)).toInt();
		int caster = 0;

		// draw new robot
		switch (preconfig) {
			case rsLinkbot::Preconfigs::Bow: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotL);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotL);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				break;
			}
			case rsLinkbot::Preconfigs::BugClock: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, rsLinkbot::Connectors::SmallWheel);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face3, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face3, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Right, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Right, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot1->getRobotFacePosition(rsLinkbot::Bodies::Face3, P, Q);
				rs::Quat Q2 = robot1->getRobotBodyQuaternion(rsLinkbot::Bodies::Face3, 0, Q);
				P2 = robot1->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Right, P2, Q2);
				Q2 = robot1->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Right, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(0, 0, 0), c, 0);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				break;
			}
			case rsLinkbot::Preconfigs::Explorer: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Cube, rsLinkbot::Bodies::Face1, rs::Up, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Cube, rsLinkbot::Bodies::Face1, rs::Up, 0, 2, rsLinkbot::Connectors::Caster, rs::Right);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face3, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face3, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q2 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P2 = robot0->getConnFacePosition(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side4, rs::Right, P2, Q2);
				Q2 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side4, rs::Right, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Left, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(-12, 0, 12), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotI);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot2->getRobotFacePosition(rsLinkbot::Bodies::Face1, P2, Q2);
				rs::Quat Q3 = robot2->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, rs::D2R(-12), Q2);
				P3 = robot2->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P3, Q3);
				Q3 = robot2->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, rs::D2R(-90), Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(-90, 0, 90), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw fifth robot
				rsScene::Linkbot *robot4 = new rsScene::Linkbot(rs::LinkbotL);
				robot4->setID(id + 4);
				robot4->setName(name);
				rs::Pos P4 = robot3->getRobotFacePosition(rsLinkbot::Bodies::Face2, P3, Q3);
				rs::Quat Q4 = robot3->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, Q3);
				P4 = robot3->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, P4, Q4);
				Q4 = robot3->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, Q4);
				P4 = robot4->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P4, Q4);
				Q4 = robot4->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q4);
				rsScene::Group *sceneRobot4 = _scene->createRobot(robot4);
				robot4->draw(sceneRobot4, P4, Q4, rs::Vec(0, 0, 0), c, 0);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				group->addChild(sceneRobot4);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				delete robot4;
				break;
			}
			case rsLinkbot::Preconfigs::FourBotDrive: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Omniplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P1 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q1 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P1 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side2, rs::Right, P1, Q1);
				Q1 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side2, rs::Right, Q1);
				P1 = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P1, Q1);
				Q1 = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q1);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P1, Q1, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 1);
				robot2->setName(name);
				rs::Pos P2 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q2 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P2 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side3, rs::Right, P2, Q2);
				Q2 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side3, rs::Right, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(0, 0, 0), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotI);
				robot3->setID(id + 1);
				robot3->setName(name);
				rs::Pos P3 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q3 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P3 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side4, rs::Right, P3, Q3);
				Q3 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side4, rs::Right, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(0, 0, 0), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				break;
			}
			case rsLinkbot::Preconfigs::FourWheelDrive: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, rsLinkbot::Connectors::SmallWheel);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Cube, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, rsLinkbot::Connectors::SmallWheel);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P1 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q1 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P1 = robot0->getConnFacePosition(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, P1, Q1);
				Q1 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, Q1);
				P1 = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P1, Q1);
				Q1 = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q1);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P1, Q1, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, rsLinkbot::Connectors::SmallWheel);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, rsLinkbot::Connectors::SmallWheel);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				break;
			}
			case rsLinkbot::Preconfigs::FourWheelExplorer: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Cube, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Cube, rsLinkbot::Bodies::Face2, rs::Right, 0, 4, rsLinkbot::Connectors::Caster);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side3, rs::Right, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 2, rsLinkbot::Connectors::BigWheel);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q2 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P2 = robot0->getConnFacePosition(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side5, rs::Right, P2, Q2);
				Q2 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Cube, rsLinkbot::Connectors::Side5, rs::Right, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Left, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(-12, 0, 12), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotI);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot2->getRobotFacePosition(rsLinkbot::Bodies::Face1, P2, Q2);
				rs::Quat Q3 = robot2->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, rs::D2R(-12), Q2);
				P3 = robot2->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P3, Q3);
				Q3 = robot2->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, rs::D2R(-90), Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(-90, 0, 90), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw fifth robot
				rsScene::Linkbot *robot4 = new rsScene::Linkbot(rs::LinkbotL);
				robot4->setID(id + 4);
				robot4->setName(name);
				rs::Pos P4 = robot3->getRobotFacePosition(rsLinkbot::Bodies::Face2, P3, Q3);
				rs::Quat Q4 = robot3->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, Q3);
				P4 = robot3->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, P4, Q4);
				Q4 = robot3->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, Q4);
				P4 = robot4->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P4, Q4);
				Q4 = robot4->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q4);
				rsScene::Group *sceneRobot4 = _scene->createRobot(robot4);
				robot4->draw(sceneRobot4, P4, Q4, rs::Vec(0, 0, 0), c, 0);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				group->addChild(sceneRobot4);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				delete robot4;
				break;
			}
			case rsLinkbot::Preconfigs::GroupBow: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotL);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotL);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw base robot 2
				rs::Pos p2(p); p2.add(0.1524, 0, 0);
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotL);
				robot2->setID(id + 2);
				robot2->setName(name);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, p2, q, rs::Vec(0, 0, 0), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw second robot 2
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotL);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot1->getRobotFacePosition(rsLinkbot::Bodies::Face1, p2, q);
				rs::Quat Q3 = robot1->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P3 = robot2->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P3, Q3);
				Q3 = robot2->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(0, 0, 0), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				break;
			}
			case rsLinkbot::Preconfigs::Inchworm: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotL);
				robot0->setID(id);
				robot0->setName(name);
				p.add(0, 0, robot0->getBodyHeight()/2);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotL);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				break;
			}
			case rsLinkbot::Preconfigs::Lift: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				p.add(0, 0, robot0->getBodyHeight()/2);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P1 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q1 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P1 = robot0->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P1, Q1);
				Q1 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q1);
				P1 = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P1, Q1);
				Q1 = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q1);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P1, Q1, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot1->getRobotFacePosition(rsLinkbot::Bodies::Face2, P1, Q1);
				rs::Quat Q2 = robot1->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, Q1);
				P2 = robot1->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, P2, Q2);
				Q2 = robot1->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(0, 0, 0), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotI);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot2->getRobotFacePosition(rsLinkbot::Bodies::Face1, P2, Q2);
				rs::Quat Q3 = robot2->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, Q2);
				P3 = robot2->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P3, Q3);
				Q3 = robot2->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(0, 0, 0), c, 0);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				break;
			}
			case rsLinkbot::Preconfigs::Omnidrive: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotL);
				robot0->setID(id);
				robot0->setName(name);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Omniplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotL);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P1 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q1 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P1 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side2, rs::Right, P1, Q1);
				Q1 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side2, rs::Right, Q1);
				P1 = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P1, Q1);
				Q1 = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q1);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P1, Q1, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotL);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q2 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P2 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side3, rs::Right, P2, Q2);
				Q2 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side3, rs::Right, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Left, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(0, 0, 0), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotL);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q3 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P3 = robot0->getConnFacePosition(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side4, rs::Right, P3, Q3);
				Q3 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Omniplate, rsLinkbot::Connectors::Side4, rs::Right, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Left, 0, Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(0, 0, 0), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 2, rsLinkbot::Connectors::SmallWheel);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				break;
			}
			case rsLinkbot::Preconfigs::Snake: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotI);
				robot0->setID(id);
				robot0->setName(name);
				p.add(0, 0, robot0->getBodyHeight()/2);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Gripper, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Left, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotI);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P1 = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face2, p, q);
				rs::Quat Q1 = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, q);
				P1 = robot0->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, P1, Q1);
				Q1 = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, Q1);
				P1 = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P1, Q1);
				Q1 = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q1);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P1, Q1, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw third robot
				rsScene::Linkbot *robot2 = new rsScene::Linkbot(rs::LinkbotI);
				robot2->setID(id + 2);
				robot2->setName(name);
				rs::Pos P2 = robot1->getRobotFacePosition(rsLinkbot::Bodies::Face1, P1, Q1);
				rs::Quat Q2 = robot1->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, Q1);
				P2 = robot1->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P2, Q2);
				Q2 = robot1->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q2);
				P2 = robot2->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P2, Q2);
				Q2 = robot2->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q2);
				rsScene::Group *sceneRobot2 = _scene->createRobot(robot2);
				robot2->draw(sceneRobot2, P2, Q2, rs::Vec(0, 0, 0), c, 0);
				robot2->drawConnector(sceneRobot2, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw fourth robot
				rsScene::Linkbot *robot3 = new rsScene::Linkbot(rs::LinkbotI);
				robot3->setID(id + 3);
				robot3->setName(name);
				rs::Pos P3 = robot2->getRobotFacePosition(rsLinkbot::Bodies::Face2, P2, Q2);
				rs::Quat Q3 = robot2->getRobotBodyQuaternion(rsLinkbot::Bodies::Face2, 0, Q2);
				P3 = robot2->getConnFacePosition(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, P3, Q3);
				Q3 = robot2->getConnFaceQuaternion(rsLinkbot::Connectors::Simple, rsLinkbot::Connectors::Side2, rs::Left, Q3);
				P3 = robot3->getRobotCenterPosition(rsLinkbot::Bodies::Face2, P3, Q3);
				Q3 = robot3->getRobotCenterQuaternion(rsLinkbot::Bodies::Face2, rs::Right, 0, Q3);
				rsScene::Group *sceneRobot3 = _scene->createRobot(robot3);
				robot3->draw(sceneRobot3, P3, Q3, rs::Vec(0, 0, 0), c, 0);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot3->drawConnector(sceneRobot3, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);

				// draw fifth robot
				rsScene::Linkbot *robot4 = new rsScene::Linkbot(rs::LinkbotI);
				robot4->setID(id + 4);
				robot4->setName(name);
				rs::Pos P4 = robot3->getRobotFacePosition(rsLinkbot::Bodies::Face1, P3, Q3);
				rs::Quat Q4 = robot3->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, Q3);
				P4 = robot3->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P4, Q4);
				Q4 = robot3->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q4);
				P4 = robot4->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P4, Q4);
				Q4 = robot4->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, rs::D2R(-10), Q4);
				rsScene::Group *sceneRobot4 = _scene->createRobot(robot4);
				robot4->draw(sceneRobot4, P4, Q4, rs::Vec(-10, 0, 10), c, 0);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
				robot4->drawConnector(sceneRobot4, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 2, rsLinkbot::Connectors::Caster);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				group->addChild(sceneRobot2);
				group->addChild(sceneRobot3);
				group->addChild(sceneRobot4);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				delete robot2;
				delete robot3;
				delete robot4;
				break;
			}
			case rsLinkbot::Preconfigs::Stand: {
				// delete old preconfig
				_scene->deletePreconfig(id);

				// create preconfig group
				rsScene::Group *group = _scene->createPreconfig(id);

				// draw base robot
				rsScene::Linkbot *robot0 = new rsScene::Linkbot(rs::LinkbotL);
				robot0->setID(id);
				robot0->setName(name);
				p.add(0, 0, robot0->getBodyHeight()/2);
				rsScene::Group *sceneRobot0 = _scene->createRobot(robot0);
				robot0->draw(sceneRobot0, p, q, rs::Vec(0, 0, 0), c, 0);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Bridge, rsLinkbot::Bodies::Face1, rs::Left, 0, 1, -1);
				robot0->drawConnector(sceneRobot0, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// draw second robot
				rsScene::Linkbot *robot1 = new rsScene::Linkbot(rs::LinkbotL);
				robot1->setID(id + 1);
				robot1->setName(name);
				rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::Bodies::Face1, p, q);
				rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::Bodies::Face1, 0, q);
				P = robot0->getConnFacePosition(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, P, Q);
				Q = robot0->getConnFaceQuaternion(rsLinkbot::Connectors::Bridge, rsLinkbot::Connectors::Side2, rs::Left, Q);
				P = robot1->getRobotCenterPosition(rsLinkbot::Bodies::Face1, P, Q);
				Q = robot1->getRobotCenterQuaternion(rsLinkbot::Bodies::Face1, rs::Right, 0, Q);
				rsScene::Group *sceneRobot1 = _scene->createRobot(robot1);
				robot1->draw(sceneRobot1, P, Q, rs::Vec(0, 0, 0), c, 0);
				robot1->drawConnector(sceneRobot1, rsLinkbot::Connectors::Faceplate, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);

				// add robots to group
				group->addChild(sceneRobot0);
				group->addChild(sceneRobot1);
				_scene->stageChild(group);

				// end
				delete robot0;
				delete robot1;
				break;
			}
			default: {
				switch (form) {
					case rs::LinkbotI: {
						// remove old robot
						_scene->deleteRobot(id);
						// create new one
						rsScene::Linkbot *robot = new rsScene::Linkbot(rs::LinkbotI);
						robot->setID(id);
						robot->setName(name);
						// adjust height to be above zero
						if (fabs(p[2]) < (robot->getBodyHeight() - rs::Epsilon)) {
							p.add(0, 0, robot->getBodyHeight() / 2);
						}
						// get wheels
						for (int i = 0; i < 2; i++) {
							if (wheelID[i] == 1)		wheel[i] = rsLinkbot::Connectors::TinyWheel;
							else if (wheelID[i] == 2)	wheel[i] = rsLinkbot::Connectors::SmallWheel;
							else if (wheelID[i] == 3)	wheel[i] = rsLinkbot::Connectors::BigWheel;
							else if (wheelID[i] == 4)	wheel[i] = rsLinkbot::Connectors::Wheel;
							else						wheel[i] = rsLinkbot::Connectors::None;
						}
						// get caster
						if (casterID == 0) caster = rsLinkbot::Connectors::Caster;
						else if (casterID == 1) caster = rsLinkbot::Connectors::SoccerScoop;
						// tilt for wheels
						float p2;
						q.multiply(robot->tiltForWheels(wheel[0], wheel[1], p2));
						p[2] += p2;
						// draw linkbot
						rsScene::Group *sceneRobot = _scene->createRobot(robot);
						robot->draw(sceneRobot, p, q, rs::Vec(0, 0, 0), c, 0);
						// left wheel
						if (wheelID[0] > 0) {
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, 0, 1, -1);
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face1, rs::Right, radius, 2, wheel[0]);
						}
						// caster
						if (wheelID[0] > 0 || wheelID[1] > 0) {
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 1, -1);
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face2, rs::Right, 0, 2, caster);
						}
						// right wheel
						if (wheelID[1] > 0) {
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, 0, 1, -1);
							robot->drawConnector(sceneRobot, rsLinkbot::Connectors::Simple, rsLinkbot::Bodies::Face3, rs::Right, radius, 2, wheel[1]);
						}
						// end
						_scene->stageChild(sceneRobot);
						delete robot;
						break;
					}
					case rs::LinkbotL: {
						// remove old robot
						_scene->deleteRobot(id);
						// create new one
						rsScene::Linkbot *robot = new rsScene::Linkbot(rs::LinkbotL);
						robot->setID(id);
						robot->setName(name);
						// adjust height to be above zero
						if (fabs(p[2]) < (robot->getBodyHeight() - rs::Epsilon)) {
							p.add(q.multiply(0, 0, robot->getBodyHeight()/2));
						}
						// draw linkbot
						rsScene::Group *sceneRobot = _scene->createRobot(robot);
						robot->draw(sceneRobot, p, q, rs::Vec(0, 0, 0), c, 0);
						_scene->stageChild(sceneRobot);
						// end
						delete robot;
						break;
					}
					case rs::EV3:
					case rs::NXT: {
						// remove old robot
						_scene->deleteRobot(id);
						// create new one
						rsScene::Mindstorms *robot = new rsScene::Mindstorms(rs::NXT);
						robot->setID(id);
						robot->setName(name);
						// get wheels
						for (int i = 0; i < 2; i++) {
							if (wheelID[i] == 0)		wheel[i] = rsMindstorms::Connectors::Small;
							else if (wheelID[i] == 1)	wheel[i] = rsMindstorms::Connectors::Big;
						}
						// tilt for wheels
						float p2;
						q.multiply(robot->tiltForWheels(wheel[0], wheel[1], p2));
						p[2] += p2;
						// draw mindstorms
						rsScene::Group *sceneRobot = _scene->createRobot(robot);
						robot->draw(sceneRobot, p, q, rs::Vec(0, 0), c, 0);
						// draw wheels
						if (wheel[0]) robot->drawWheel(sceneRobot, wheel[0], rsMindstorms::Bodies::Wheel1);
						if (wheel[1]) robot->drawWheel(sceneRobot, wheel[1], rsMindstorms::Bodies::Wheel2);
						// end
						_scene->stageChild(sceneRobot);
						delete robot;
						break;
					}
				}
				break;
			}
		}
		// add new robot
		_scene->addAndRemoveChildren();
	}
	// set current robot
	this->setCurrentRobotIndex(bottomRight);
}

void QOsgWidget::objectDataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new objects
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form, id, and mass
		int form = _o_model->data(_o_model->index(i, rsObjectModel::FORM)).toInt();
		int id = _o_model->data(_o_model->index(i, rsObjectModel::ID), Qt::EditRole).toInt();
		int size = _o_model->data(_o_model->index(i, rsObjectModel::SIZE)).toInt();

		// get name
		std::string name = _o_model->data(_o_model->index(i, rsObjectModel::TEXT)).toString().toStdString();

		// get position
		rs::Pos p(_o_model->data(_o_model->index(i, rsObjectModel::P_X)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObjectModel::P_Y)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObjectModel::P_Z)).toDouble());

		// get euler angles
		double r[3] = {rs::D2R(_o_model->data(_o_model->index(i, rsObjectModel::R_PHI)).toDouble()),
					   rs::D2R(_o_model->data(_o_model->index(i, rsObjectModel::R_THETA)).toDouble()),
					   rs::D2R(_o_model->data(_o_model->index(i, rsObjectModel::R_PSI)).toDouble())};

		// calculate quaternion
		rs::Quat quat(sin(0.5*r[0]), 0, 0, cos(0.5*r[0]));
		quat.multiply(0, sin(0.5*r[1]), 0, cos(0.5*r[1]));
		quat.multiply(0, 0, sin(0.5*r[2]), cos(0.5*r[2]));

		// get led color
		QColor color(_o_model->data(_o_model->index(i, rsObjectModel::COLOR)).toString());
		rs::Vec led(color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0);

		// delete old object
		_scene->deleteObstacle(id);
		_scene->deleteMarker(id);

		// draw new object
		switch (form) {
			case rs::Box:
			case rs::CompetitionBorder:
			case rs::Cylinder:
			case rs::HackySack:
			case rs::PullupBar:
			case rs::Sphere:
			case rs::WoodBlock: {
				rs::Vec dims(_o_model->data(_o_model->index(i, rsObjectModel::L_1)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObjectModel::L_2)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObjectModel::L_3)).toDouble());
				_scene->drawObstacle(id, form, p, led, dims, quat);
				break;
			}
			case rs::Dot:
			case rs::Line:
			case rs::Rectangle:
			case rs::Text:
			case rs::Triangle: {
				rs::Pos dims(_o_model->data(_o_model->index(i, rsObjectModel::L_1)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObjectModel::L_2)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObjectModel::L_3)).toDouble());
				_scene->drawMarker(id, form, p, dims, led, size, name);
				break;
			}
		}

		// add new object to scene
		_scene->addAndRemoveChildren();
	}
	// set current robot
	this->setCurrentObjectIndex(bottomRight);
}

void QOsgWidget::setBackgroundImage(int pos, std::string name) {
	_scene->setBackgroundImage(pos, name);
}

void QOsgWidget::setCurrentBackground(int ind) {
	if (_level == ind) return;

	// set new level
	_level = ind;

	// draw new level
	_scene->setLevel(_level);
}

void QOsgWidget::setCurrentIndex(int ind) {
	if (ind == 0) {
		this->setCurrentRobotIndex(_r_model->index(_r_model->findByID(_current[0]).toInt(), rsRobotModel::ID)); }
	else if (ind == 1)
		this->setCurrentObjectIndex(_o_model->index(_o_model->findByID(_current[1]).toInt(), rsObjectModel::ID));
}

void QOsgWidget::setCurrentObjectIndex(const QModelIndex &index) {
	// invalid index: remove all highlight and return
	if ( !index.isValid() ) {
		_scene->addHighlight(-1, false);
		return;
	}

	// set new object ID
	_current[1] = _o_model->data(_o_model->index(index.row(), rsObjectModel::ID), Qt::EditRole).toInt();

	// object is valid ID
	_current[2] = 1;

	// update view
	_scene->addHighlight(_current[1], false);
}

void QOsgWidget::setCurrentRobotIndex(const QModelIndex &index) {
	// invalid index: remove all highlight and return
	if ( !index.isValid() ) {
		_scene->addHighlight(-1, false);
		return;
	}

	// set new robot ID
	_current[0] = _r_model->data(_r_model->index(index.row(), rsRobotModel::ID), Qt::EditRole).toInt();

	// robot is valid ID
	_current[2] = 0;

	// highlight robots
	this->highlight_robots(index);
}

void QOsgWidget::setNewBackground(QListWidgetItem *current, QListWidgetItem *previous) {
	// read background xml file
	rsXML::BackgroundReader background(current->data(Qt::UserRole).toString().toStdString());

	// load background images
	for (int i = 0; i < 7; i++) {
		_scene->setBackgroundImage(i, background.getBackgroundImage(i));
	}

	// set level after images are set
	_scene->setLevel(background.getLevel());
	_scene->setGrid(_grid, true);
	_scene->addAndRemoveChildren(true);

	// draw object objects
	for (int i = 0; i < background.getNumObstacles(); i++) {
		rsXML::Obstacle *obstacle = background.getObstacle(i);
		_scene->drawObstacle(0, obstacle->getForm(), obstacle->getPosition(), obstacle->getColor(), obstacle->getDimensions(), obstacle->getQuaternion());
	}
	// draw marker objects
	for (int i = 0; i < background.getNumMarkers(); i++) {
		rsXML::Marker *marker = background.getMarker(i);
		_scene->drawMarker(0, marker->getForm(), marker->getStart(), marker->getEnd(), marker->getColor(), marker->getSize(), marker->getLabel());
	}

	// add children to background after level is set
	_scene->addChildrenToBackground();
}

void QOsgWidget::setObjectModel(objectModel *model) {
	// set model
	_o_model = model;
}

void QOsgWidget::setRobotModel(robotModel *model) {
	// set model
	_r_model = model;
}

void QOsgWidget::setUnits(bool si) {
	_units = si;
	_scene->setUnits(si);
}

double QOsgWidget::convert(double value) {
	return ((_units) ? value/100 : value/39.37);
}

bool QOsgWidget::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		switch (keyEvent->key()) {
			case Qt::Key_Backspace:
			case Qt::Key_Delete: {
				if (_current[0] != -1 && _current[2] == 0) {
					// remove current robot from model
					QModelIndex index = _r_model->index(_current[0], rsRobotModel::ID);
					_r_model->removeRows(index.row(), 1);

					// new index is same row as last one
					int row = _r_model->index(_current[0], rsRobotModel::ID).row();

					// if it is invalid, then set the last row in the model
					if (row == -1) {
						this->setCurrentRobotIndex(_r_model->index(_r_model->rowCount()-1, rsRobotModel::ID));
						emit robotIndexChanged(_r_model->index(_r_model->rowCount()-1, rsRobotModel::ID));
					}
					else {
						this->setCurrentRobotIndex(_r_model->index(row, rsRobotModel::ID));
						emit robotIndexChanged(_r_model->index(row, rsRobotModel::ID));
					}
				}
				else if (_current[1] != -1 && _current[2] == 1) {
					// remove current object from model
					QModelIndex index = _o_model->index(_current[1], rsObjectModel::ID);
					_o_model->removeRows(index.row(), 1);

					// new index is same row as last one
					int row = _o_model->index(_current[1], rsObjectModel::ID).row();

					// if it is invalid, then set the last row in the model
					if (row == -1) {
						this->setCurrentObjectIndex(_o_model->index(_o_model->rowCount()-1, rsObjectModel::ID));
						emit objectIndexChanged(_o_model->index(_o_model->rowCount()-1, rsObjectModel::ID));
					}
					else {
						this->setCurrentObjectIndex(_o_model->index(row, rsObjectModel::ID));
						emit objectIndexChanged(_o_model->index(row, rsObjectModel::ID));
					}
				}
				break;
			}
			default:
				return QObject::eventFilter(obj, event);
				break;
		}
		return true;
	}
	else
		return QObject::eventFilter(obj, event);
}

void QOsgWidget::highlight_robots(const QModelIndex &index) {
	if (_r_model->data(_r_model->index(index.row(), rsRobotModel::PRECONFIG), Qt::EditRole).toInt())
		_scene->addHighlight(_current[0], true, true, true);
	else
		_scene->addHighlight(_current[0], true, false, true);
}

/*!
 *
 *
 *	Mouse Handler
 *
 *
 */

QMouseHandler::QMouseHandler(rsScene::Scene *scene) : rsScene::MouseHandler(scene) { }

int QMouseHandler::pick(const osgGA::GUIEventAdapter &ea, osgViewer::Viewer *viewer) {
	int id = rsScene::MouseHandler::pick(ea, viewer);
	if (id >= 1000)
		emit clickedObjectIndex(id-1000);
	else if (id >= 0)
		emit clickedRobotIndex(id, false);

	return id;
}

