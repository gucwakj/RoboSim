#include "qosgwidget.h"

#include <QListWidgetItem>

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <rs/Macros>
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

	// handle event inputs
	this->installEventFilter(this);

	// set display settings
	osg::ref_ptr<osg::DisplaySettings> ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);
	traits->x = this->x();
	traits->y = this->y();
	traits->width = 640;
	traits->height = 360;
	traits->doubleBuffer = true;
	traits->vsync = true;
	traits->inheritedWindowData = new osgQt::GraphicsWindowQt::WindowData(this);
	osg::ref_ptr<osgQt::GraphicsWindowQt> gw = new osgQt::GraphicsWindowQt(traits.get());

	// create viewer
	_scene->setupViewer(dynamic_cast<osgViewer::Viewer*>(this));
	_scene->setupCamera(gw, traits->width, traits->height);
	_scene->setupScene(traits->width, traits->height, false);
	QMouseHandler *mh = new QMouseHandler(_scene);
	_scene->setMouseHandler(mh);
	QWidget::connect(mh, SIGNAL(clickedObstacleIndex(int)), this, SLOT(clickedObstacleIndex(int)));
	QWidget::connect(mh, SIGNAL(clickedRobotIndex(int)), this, SLOT(clickedRobotIndex(int)));

	// set grid
	_units = 0;
	_grid.push_back(1/39.37);
	_grid.push_back(12/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(1);
	_scene->setUnits(_units);
	_scene->setGrid(_grid, true);

	// initialize current indices
	_current[0] = -1;
	_current[1] = -1;
	_current[2] = -1;

	// set highlight on click
	_scene->setHighlight(true);
	// hide global hud
	_scene->setHUD(false);
	// set level to display
	_level = 1;
	_scene->setLevel(_level);
	// hide robot popup labels
	_scene->setLabel(false);

	// draw viewer
	this->setRunMaxFrameRate(30);
	osgQt::setViewer(this);
}

QOsgWidget::~QOsgWidget(void) {
	_scene->setMouseHandler(NULL);
}

void QOsgWidget::clickedObstacleIndex(int id) {
	QModelIndex index;
	for (int i = 0; i < _o_model->rowCount(); i++) {
		index = _o_model->index(i, rsObstacleModel::ID);
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
			// obstacle is valid index
			_current[2] = 1;
			emit obstacleIndexChanged(index);
			return;
		}
	}
}

void QOsgWidget::clickedRobotIndex(int id) {
	QModelIndex index;
	for (int i = 0; i < _r_model->rowCount(); i++) {
		index = _r_model->index(i, rsRobotModel::ID);
		if (_r_model->data(index, Qt::EditRole).toInt() == id) {
			// highlight new item
			if (id != _current[0] || _current[2] == 1) {
				_current[0] = id;
				this->highlight_robots(index);
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

void QOsgWidget::deleteObstacleIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _o_model->data(_o_model->index(first, rsObstacleModel::ID), Qt::EditRole).toInt();
	_scene->deleteObstacle(id);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::deleteRobotIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _r_model->data(_r_model->index(first, rsRobotModel::ID), Qt::EditRole).toInt();
	_scene->deleteRobot(id);
	_scene->addAndRemoveChildren();
}

void QOsgWidget::gridDefaults(void) {
	// reset to defaults
	if (_units) {
		_grid[0] = 5/100;
		_grid[1] = 50/100;
		_grid[2] = -200/100;
		_grid[3] = 200/100;
		_grid[4] = -200/100;
		_grid[5] = 200/100;
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
}

void QOsgWidget::gridTics(double value) {
	_grid[0] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridHash(double value) {
	_grid[1] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridMinX(double value) {
	_grid[2] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridMaxX(double value) {
	_grid[3] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridMinY(double value) {
	_grid[4] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridMaxY(double value) {
	_grid[5] = convert(value);
	_scene->setGrid(_grid, true);
}

void QOsgWidget::gridEnabled(bool enabled) {
	_grid[6] = static_cast<double>(enabled);
	_scene->setGrid(_grid, true);
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

		// draw new robot
		switch (form) {
			case rs::LINKBOTI: {
				switch (preconfig) {
					case rsLinkbot::BOW: {
						// delete old robots
						_scene->deleteRobot(id);
						_scene->deleteRobot(id + 1);

						// draw base robot
						rsRobots::Linkbot *robot0 = new rsRobots::Linkbot(rs::LINKBOTL);
						robot0->setID(id);
						robot0->setName(name);
						rsScene::Robot *sceneRobot0 = _scene->drawRobot(robot0, p, q, rs::Vec(0, 0, 0), c, 0);
						_scene->drawConnector(robot0, sceneRobot0, rsLinkbot::BRIDGE, rsLinkbot::FACE1, rs::LEFT, 0, 1, -1);
						_scene->drawConnector(robot0, sceneRobot0, rsLinkbot::FACEPLATE, rsLinkbot::FACE2, rs::RIGHT, 0, 1, -1);

						// draw second robot
						rsRobots::Linkbot *robot1 = new rsRobots::Linkbot(rs::LINKBOTL);
						robot1->setID(id + 1);
						robot1->setName(name);
						rs::Pos P = robot0->getRobotFacePosition(rsLinkbot::FACE1, p, q);
						rs::Quat Q = robot0->getRobotBodyQuaternion(rsLinkbot::FACE1, 0, q);
						P = robot0->getConnFacePosition(rsLinkbot::DOUBLEBRIDGE, rsLinkbot::SIDE2, rs::LEFT, P, Q);
						Q = robot0->getConnFaceQuaternion(rsLinkbot::DOUBLEBRIDGE, rsLinkbot::SIDE2, rs::LEFT, Q);
						P = robot1->getRobotCenterPosition(rsLinkbot::FACE3, P, Q);
						Q = robot1->getRobotCenterQuaternion(rsLinkbot::FACE3, rs::LEFT, 0, Q);
						rsScene::Robot *sceneRobot1 = _scene->drawRobot(robot1, P, Q, rs::Vec(0, 0, 0), c, 0);
						_scene->drawConnector(robot1, sceneRobot1, rsLinkbot::FACEPLATE, rsLinkbot::FACE2, rs::RIGHT, 0, 1, -1);

						// end
						delete robot0;
						delete robot1;
						break;
					}
					default: {
						// remove old robot
						_scene->deleteRobot(id);
						// create new one
						rsRobots::Linkbot *robot = new rsRobots::Linkbot(rs::LINKBOTI);
						robot->setID(id);
						robot->setName(name);
						// get wheels
						for (int i = 0; i < 2; i++) {
							if (wheelID[i] == 0)
								wheel[i] = -1;
							else if (wheelID[i] == 1)
								wheel[i] = rsLinkbot::TINYWHEEL;
							else if (wheelID[i] == 2)
								wheel[i] = rsLinkbot::SMALLWHEEL;
							else if (wheelID[i] == 3)
								wheel[i] = rsLinkbot::BIGWHEEL;
							else if (wheelID[i] == 4)
								wheel[i] = rsLinkbot::WHEEL;
						}
						// tilt for wheels
						double p2;
						q.multiply(robot->tiltForWheels(wheel[0], 1, p2));
						p[2] += p2;
						q.multiply(robot->tiltForWheels(wheel[1], 3, p2));
						p[2] += p2;
						// adjust height to be above zero
						if (fabs(p[2]) < (robot->getBodyHeight() - rs::EPSILON)) {
							p.add(0, 0, robot->getBodyHeight()/2);
						}
						// draw linkbot
						rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0, 0), c, 0);
						// left wheel
						if (wheelID[0]) {
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, rs::RIGHT, 0, 1, -1);
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, rs::RIGHT, radius, 2, wheel[0]);
						}
						// caster
						if (wheelID[0] || wheelID[1]) {
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, rs::RIGHT, 0, 1, -1);
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, rs::RIGHT, 0, 2, rsLinkbot::CASTER);
						}
						// right wheel
						if (wheelID[1]) {
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, rs::RIGHT, 0, 1, -1);
							_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, rs::RIGHT, radius, 2, wheel[1]);
						}
						// end
						delete robot;
						break;
					}
				}
				break;
			}
			case rs::LINKBOTL: {
				// remove old robot
				_scene->deleteRobot(id);
				// create new one
				rsRobots::Linkbot *robot = new rsRobots::Linkbot(rs::LINKBOTL);
				robot->setID(id);
				robot->setName(name);
				// adjust height to be above zero
				if (fabs(p[2]) < (robot->getBodyHeight() - rs::EPSILON)) {
					p.add(q.multiply(0, 0, robot->getBodyHeight()/2));
				}
				// draw linkbot
				_scene->drawRobot(robot, p, q, rs::Vec(0, 0, 0), c, 0);
				// end
				delete robot;
				break;
			}
			case rs::EV3:
			case rs::NXT: {
				// remove old robot
				_scene->deleteRobot(id);
				// create new one
				rsRobots::Mindstorms *robot;
				if (form == rs::EV3)
					robot = new rsRobots::Mindstorms(rs::EV3);
				else if (form == rs::NXT)
					robot = new rsRobots::Mindstorms(rs::NXT);
				robot->setID(id);
				robot->setName(name);
				// get wheels
				for (int i = 0; i < 2; i++) {
					if (wheelID[i] == 0)
						wheel[i] = rsMindstorms::SMALL;
					else if (wheelID[i] == 1)
						wheel[i] = rsMindstorms::BIG;
				}
				// tilt for wheels
				double p2;
				q.multiply(robot->tiltForWheels(wheel[0], wheel[1], p2));
				p[2] += p2;
				// draw mindstorms
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0), c, 0);
				// draw wheels
				if (wheel[0]) _scene->drawWheel(robot, sceneRobot, wheel[0], rsMindstorms::WHEEL1);
				if (wheel[1]) _scene->drawWheel(robot, sceneRobot, wheel[1], rsMindstorms::WHEEL2);
				// end
				delete robot;
				break;
			}
		}
		// add new robot
		_scene->addAndRemoveChildren();
	}
	// set current robot
	this->setCurrentRobotIndex(bottomRight);
}

void QOsgWidget::obstacleDataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new obstacles
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form, id, and mass
		int form = _o_model->data(_o_model->index(i, rsObstacleModel::FORM)).toInt();
		int id = _o_model->data(_o_model->index(i, rsObstacleModel::ID), Qt::EditRole).toInt();
		int mass = _o_model->data(_o_model->index(i, rsObstacleModel::MASS), Qt::EditRole).toInt();

		// get name
		std::string name = _o_model->data(_o_model->index(i, rsObstacleModel::TEXT)).toString().toStdString();

		// get position
		rs::Pos p(_o_model->data(_o_model->index(i, rsObstacleModel::P_X)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObstacleModel::P_Y)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObstacleModel::P_Z)).toDouble());

		// quaternion
		int axis = _o_model->data(_o_model->index(i, rsObstacleModel::AXIS)).toInt();
		rs::Quat quat;
		switch (axis) {
			case 0: // x
				quat[1] = sin(0.785398);
				quat[3] = cos(0.785398);
				break;
			case 1: // y
				quat[0] = sin(0.785398);
				quat[3] = cos(0.785398);
				break;
		}

		// get led color
		QColor color(_o_model->data(_o_model->index(i, rsObstacleModel::COLOR)).toString());
		rs::Vec led(color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0);

		// delete old obstacle
		_scene->deleteObstacle(id);
		_scene->deleteMarker(id);

		// draw new obstacle
		switch (form) {
			case rs::BOX:
			case rs::CYLINDER:
			case rs::SPHERE: {
				rs::Vec dims(_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble());
				_scene->drawObstacle(id, form, p, led, dims, quat);
				break;
			}
			case rs::DOT:
			case rs::LINE:
			case rs::TEXT: {
				rs::Pos dims(_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble());
				_scene->drawMarker(id, form, p, dims, led, mass, name);
				break;
			}
		}

		// add new obstacle to scene
		_scene->addAndRemoveChildren();
	}
	// set current robot
	this->setCurrentObstacleIndex(bottomRight);
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
		this->setCurrentObstacleIndex(_o_model->index(_o_model->findByID(_current[1]).toInt(), rsObstacleModel::ID));
}

void QOsgWidget::setCurrentObstacleIndex(const QModelIndex &index) {
	// invalid index: remove all highlight and return
	if ( !index.isValid() ) {
		_scene->addHighlight(-1, false);
		return;
	}

	// set new obstacle ID
	_current[1] = _o_model->data(_o_model->index(index.row(), rsObstacleModel::ID), Qt::EditRole).toInt();

	// obstacle is valid ID
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

	// draw obstacle objects
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

void QOsgWidget::setObstacleModel(obstacleModel *model) {
	// set model
	_o_model = model;
}

void QOsgWidget::setRobotModel(robotModel *model) {
	// set model
	_r_model = model;
}

void QOsgWidget::setUnits(bool si) {
	_units = si;
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
					// remove current obstacle from model
					QModelIndex index = _o_model->index(_current[1], rsObstacleModel::ID);
					_o_model->removeRows(index.row(), 1);

					// new index is same row as last one
					int row = _o_model->index(_current[1], rsObstacleModel::ID).row();

					// if it is invalid, then set the last row in the model
					if (row == -1) {
						this->setCurrentObstacleIndex(_o_model->index(_o_model->rowCount()-1, rsObstacleModel::ID));
						emit obstacleIndexChanged(_o_model->index(_o_model->rowCount()-1, rsObstacleModel::ID));
					}
					else {
						this->setCurrentObstacleIndex(_o_model->index(row, rsObstacleModel::ID));
						emit obstacleIndexChanged(_o_model->index(row, rsObstacleModel::ID));
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
	// update view
	_scene->addHighlight(_current[0]);

	// highlight rest of preconfig robots
	int preconfig = _r_model->data(_r_model->index(index.row(), rsRobotModel::PRECONFIG), Qt::EditRole).toInt();
	switch (preconfig) {
		case rsLinkbot::BOW:
			_scene->addHighlight(_current[0] + 1, true, false);
			break;
		case rsLinkbot::EXPLORER:
			break;
		case rsLinkbot::FOURBOTDRIVE:
			break;
		case rsLinkbot::FOURWHEELDRIVE:
			break;
		case rsLinkbot::FOURWHEELEXPLORER:
			break;
		case rsLinkbot::GROUPBOW:
			break;
		case rsLinkbot::INCHWORM:
			break;
		case rsLinkbot::LIFT:
			break;
		case rsLinkbot::OMNIDRIVE:
			break;
		case rsLinkbot::SNAKE:
			break;
		case rsLinkbot::STAND:
			break;
	}
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
		emit clickedObstacleIndex(id-1000);
	else if (id >= 0)
		emit clickedRobotIndex(id);

	return id;
}

