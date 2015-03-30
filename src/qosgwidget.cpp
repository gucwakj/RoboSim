#include "qosgwidget.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

QOsgWidget::QOsgWidget(QWidget *parent) : osgQt::GLWidget(parent) {
	// create new scene
	_scene = new rsScene::Scene();

	// privide reference count
	this->ref();

	// handle event inputs
	this->installEventFilter(this);

	// set display settings
	osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);
	traits->x = this->x();
	traits->y = this->y();
	traits->width = this->width();
	traits->height = this->height();
	traits->doubleBuffer = false;
	traits->inheritedWindowData = new osgQt::GraphicsWindowQt::WindowData(this);
	osg::ref_ptr<osgQt::GraphicsWindowQt> gw = new osgQt::GraphicsWindowQt(traits.get());

	// create viewer
	_scene->setupViewer(dynamic_cast<osgViewer::Viewer*>(this));
	_scene->setupCamera(gw, traits->width, traits->height);
	_scene->setupScene(traits->width, traits->height, false);
	QMouseHandler *mh = new QMouseHandler(_scene);
	_scene->setMouseHandler(mh);
	QWidget::connect(mh, SIGNAL(clickedIndex(int)), this, SLOT(clickedIndex(int)));

	// set grid
	_units = 0;
	_grid.push_back(1/39.37);
	_grid.push_back(12/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(-48/39.37);
	_grid.push_back(48/39.37);
	_grid.push_back(1);
	_scene->setGrid(_units, _grid);

	// set highlighting of click
	_scene->setHighlight(true);
	// set global hud
	_scene->setHUD(false);
	// set level to display
	_level = 0;
	_scene->setLevel(_level);
	_scene->setLabel(false);

	// draw viewer
	osgQt::setViewer(this);
}

QOsgWidget::~QOsgWidget(void) {
	//this->unref();
	delete _scene;
}

void QOsgWidget::setRobotModel(robotModel *model) {
	// set model
	_r_model = model;

	// update view with default model
	this->dataChanged(_r_model->index(0, 0), _r_model->index(_r_model->rowCount()-1, 0));
}

void QOsgWidget::setObstacleModel(obstacleModel *model) {
	// set model
	_o_model = model;

	// update view with default model
	this->dataChanged(_o_model->index(0, 0), _o_model->index(_o_model->rowCount()-1, 0));
}

void QOsgWidget::setUnits(bool si) {
	_units = si;
}

void QOsgWidget::dataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new robots
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form and id
		int form = _r_model->data(_r_model->index(i, rsModel::FORM)).toInt();
		int id = _r_model->data(_r_model->index(i, rsModel::ID), Qt::EditRole).toInt();
		std::string name = _r_model->data(_r_model->index(i, rsModel::NAME)).toString().toStdString();

		// get position
		rs::Pos p(_r_model->data(_r_model->index(i, rsModel::P_X)).toDouble(),
				  _r_model->data(_r_model->index(i, rsModel::P_Y)).toDouble(),
				  _r_model->data(_r_model->index(i, rsModel::P_Z)).toDouble() + 0.04445);

		// get euler angles
		double r[3] = {DEG2RAD(_r_model->data(_r_model->index(i, rsModel::R_PHI)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsModel::R_THETA)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsModel::R_PSI)).toDouble())};

		// calculate quaternion
		rs::Quat q(sin(0.5*r[0]), 0, 0, cos(0.5*r[0]));
		q.multiply(0, sin(0.5*r[1]), 0, cos(0.5*r[1]));
		q.multiply(0, 0, sin(0.5*r[2]), cos(0.5*r[2]));

		// get led color
		QColor color(_r_model->data(_r_model->index(i, rsModel::COLOR)).toString());
		rs::Vec c(color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0);

		// delete old robot
		_scene->deleteChild(id);

		// draw new robot
		switch (form) {
			case rs::LINKBOTI: {
				rsRobots::Linkbot *robot = new rsRobots::Linkbot(rs::LINKBOTI);
				robot->setID(id);
				robot->setName(name);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0, 0), c, 0);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 2, rsLinkbot::CASTER);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				break;
			}
			case rs::LINKBOTL: {
				rsRobots::Linkbot *robot = new rsRobots::Linkbot(rs::LINKBOTL);
				robot->setID(id);
				robot->setName(name);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0, 0), c, 0);
				break;
			}
			case rs::EV3: {
				rsRobots::Mindstorms *robot = new rsRobots::Mindstorms(rs::EV3);
				robot->setID(id);
				robot->setName(name);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0), c, 0);
				break;
			}
			case rs::NXT: {
				rsRobots::Mindstorms *robot = new rsRobots::Mindstorms(rs::NXT);
				robot->setID(id);
				robot->setName(name);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, p, q, rs::Vec(0, 0), c, 0);
				break;
			}
		}
		// add new robot
		_scene->addChild();
	}
	// set current robot
	this->setCurrentIndex(bottomRight);
}

void QOsgWidget::dataChanged2(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new robots
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form and id
		int form = _o_model->data(_o_model->index(i, rsObstacleModel::FORM)).toInt();
		int id = _o_model->data(_o_model->index(i, rsObstacleModel::ID), Qt::EditRole).toInt();

		// get position
		double pos[3] = {_o_model->data(_o_model->index(i, rsObstacleModel::P_X)).toDouble(),
						 _o_model->data(_o_model->index(i, rsObstacleModel::P_Y)).toDouble(),
						 _o_model->data(_o_model->index(i, rsObstacleModel::P_Z)).toDouble()};

		// get dimensions
		double dims[3] = {_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
						  _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
						  _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble()};

		// quaternion
		double quat[4] = {0, 0, 0, 1};

		// get led color
		QColor color(_o_model->data(_o_model->index(i, rsObstacleModel::COLOR)).toString());
		double led[4] = {color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0};

		// delete old robot
		_scene->deleteObstacle(id);

		// draw new obstacle
		_scene->drawGround(id, form, pos, led, dims, quat);

		// add new obstacle to scene 
		_scene->addChild();
	}
	// set current robot
	this->setCurrentIndex2(bottomRight);
}

void QOsgWidget::setCurrentIndex(const QModelIndex &index) {
	// get new robot ID
	int newRobot = _r_model->data(_r_model->index(index.row(), rsModel::ID), Qt::EditRole).toInt();

	// set new current robot
	_current = newRobot;

	// update view
	_scene->addHighlight(_current);
}

void QOsgWidget::setCurrentIndex2(const QModelIndex &index) {
	// get new obstacle ID
	int new_obstacle = _o_model->data(_o_model->index(index.row(), rsObstacleModel::ID), Qt::EditRole).toInt();

	// set new current robot
	_current = new_obstacle;

	// update view
	_scene->addHighlight(_current);
}

void QOsgWidget::changeLevel(void) {
	_level = (_level) ? 0 : 1;
	_scene->setLevel(_level);
}

void QOsgWidget::clickedIndex(int id) {
	QModelIndex index;
	for (int i = 0; i < _r_model->rowCount(); i++) {
		index = _r_model->index(i, rsModel::ID);
		if (_r_model->data(index, Qt::EditRole).toInt() == id) {
			// highlight new item
			if (id != _current) {
				_current = id;
				_scene->addHighlight(_current);
			}
			// deselect current item
			else {
				_current = -1;
				index = _r_model->index(-1, -1);
			}
			emit indexChanged(index);
			return;
		}
	}
}

void QOsgWidget::deleteIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _r_model->data(_r_model->index(first, rsModel::ID), Qt::EditRole).toInt();
	_scene->deleteChild(id);
}

void QOsgWidget::deleteIndex2(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _o_model->data(_o_model->index(first, rsObstacleModel::ID), Qt::EditRole).toInt();
	_scene->deleteObstacle(id);
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
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridTics(double value) {
	_grid[0] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridHash(double value) {
	_grid[1] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridMinX(double value) {
	_grid[2] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridMaxX(double value) {
	_grid[3] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridMinY(double value) {
	_grid[4] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridMaxY(double value) {
	_grid[5] = convert(value);
	_scene->setGrid(_units, _grid);
}

void QOsgWidget::gridEnabled(bool enabled) {
	_grid[6] = static_cast<double>(enabled);
	_scene->setGrid(_units, _grid);
}

bool QOsgWidget::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		switch (keyEvent->key()) {
			case Qt::Key_Backspace:
			case Qt::Key_Delete: {
				// remove current robot from model
				QModelIndex index = _r_model->index(_current, rsModel::ID);
				_r_model->removeRows(index.row(), 1);

				// new index is same row as last one
				int row = _r_model->index(_current, rsModel::ID).row();

				// if it is invalid, then set the last row in the model
				if (row == -1) {
					this->setCurrentIndex(_r_model->index(_r_model->rowCount()-1, rsModel::ID));
					emit indexChanged(_r_model->index(_r_model->rowCount()-1, rsModel::ID));
				}
				else {
					this->setCurrentIndex(_r_model->index(row, rsModel::ID));
					emit indexChanged(_r_model->index(row, rsModel::ID));
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

double QOsgWidget::convert(double value) {
	return ((_units) ? value/100 : value/39.37);
}

QMouseHandler::QMouseHandler(rsScene::Scene *scene) : rsScene::MouseHandler(scene) {
}

int QMouseHandler::pick(const osgGA::GUIEventAdapter &ea, osgViewer::Viewer *viewer) {
	int id = rsScene::MouseHandler::pick(ea, viewer);
	if (id != -1)
		emit clickedIndex(id);
}

