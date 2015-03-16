#include "qosgwidget.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
//#include <rsScene/mouseHandler.hpp>

QOsgWidget::QOsgWidget(QWidget *parent) : osgQt::GLWidget(parent) {
	// create new scene
	_scene = new rsScene::Scene();

	// privide reference count
	this->ref();

	// set grid
	std::vector<double> grid;
	grid.push_back(1/39.37);
	grid.push_back(12/39.37);
	grid.push_back(-48/39.37);
	grid.push_back(48/39.37);
	grid.push_back(-48/39.37);
	grid.push_back(48/39.37);
	grid.push_back(1);
	_scene->setGrid(0, grid);

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

	double pos[3] = {0.2, 0.2, 0};
	double color[4] = {0, 0, 1, 1};
	double dims[3] = {0.1, 0.1, 0.1};
	double quat[4] = {0, 0, 0, 1};
	_scene->drawGround(rs::BOX, pos, color, dims, quat);
	_scene->addChild();
}

QOsgWidget::~QOsgWidget(void) {
	//this->unref();
	delete _scene;
}

void QOsgWidget::setModel(robotModel *model) {
	// set model
	_model = model;

	// update view with default model
	this->dataChanged(_model->index(0, 0), _model->index(_model->rowCount()-1, 0));
}

void QOsgWidget::dataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	// draw all new robots
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form and id
		int form = _model->data(_model->index(i, rsModel::FORM)).toInt();
		int id = _model->data(_model->index(i, rsModel::ID), Qt::EditRole).toInt();
		std::string name = _model->data(_model->index(i, rsModel::NAME)).toString().toStdString();

		// get position
		double pos[3] = {_model->data(_model->index(i, rsModel::P_X)).toDouble(),
						 _model->data(_model->index(i, rsModel::P_Y)).toDouble(),
						 _model->data(_model->index(i, rsModel::P_Z)).toDouble() + 0.04445};

		// get euler angles
		double r[3] = {DEG2RAD(_model->data(_model->index(i, rsModel::R_PHI)).toDouble()),
					   DEG2RAD(_model->data(_model->index(i, rsModel::R_THETA)).toDouble()),
					   DEG2RAD(_model->data(_model->index(i, rsModel::R_PSI)).toDouble())};

		// calculate quaternion
		double quat[4] = {0, 0, 0, 1}, o1[4];
		double q1[4] = {sin(0.5*r[0]), 0, 0, cos(0.5*r[0])};
		double q2[4] = {0, sin(0.5*r[1]), 0, cos(0.5*r[1])};
		double q3[4] = {0, 0, sin(0.5*r[2]), cos(0.5*r[2])};

		// get led color
		QColor color(_model->data(_model->index(i, rsModel::COLOR)).toString());
		double led[4] = {color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0};

		// delete old robot
		_scene->deleteChild(id);

		// draw new robot
		switch (form) {
			case rs::LINKBOTI: {
				rsRobots::LinkbotI *robot = new rsRobots::LinkbotI();
				robot->setID(id);
				robot->setName(name);
				robot->multiplyQbyQ(q2, q1, o1);
				robot->multiplyQbyQ(q3, o1, quat);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, pos, quat, led, 0);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 2, rsLinkbot::CASTER);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				break;
			}
			case rs::LINKBOTL: {
				rsRobots::LinkbotL *robot = new rsRobots::LinkbotL();
				robot->setID(id);
				robot->setName(name);
				robot->multiplyQbyQ(q2, q1, o1);
				robot->multiplyQbyQ(q3, o1, quat);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, pos, quat, led, 0);
				break;
			}
			case rs::MINDSTORMS: {
				rsRobots::Mindstorms *robot = new rsRobots::Mindstorms();
				robot->setID(id);
				robot->setName(name);
				robot->multiplyQbyQ(q2, q1, o1);
				robot->multiplyQbyQ(q3, o1, quat);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, pos, quat, led, 0);
				break;
			}
		}
		// add new robot
		_scene->addChild();
	}
	// set current robot
	this->setCurrentIndex(bottomRight);
}

void QOsgWidget::setCurrentIndex(const QModelIndex &index) {
	// get new robot ID
	int newRobot = _model->data(_model->index(index.row(), rsModel::ID), Qt::EditRole).toInt();

	// set new current robot
	_current = newRobot;

	// update view
	_scene->addHighlight(_current);
}

void QOsgWidget::changeLevel(void) {
	_level = (_level) ? 0 : 1;
	_scene->setLevel(_level);
}

void QOsgWidget::clickedIndex(int id) {
	QModelIndex index;
	for (int i = 0; i < _model->rowCount(); i++) {
		index = _model->index(i, rsModel::ID);
		if (_model->data(index, Qt::EditRole).toInt() == id) {
			// highlight new item
			if (id != _current) {
				_current = id;
				_scene->addHighlight(_current);
			}
			// deselect current item
			else {
				_current = -1;
				index = _model->index(-1, -1);
			}
			emit indexChanged(index);
			return;
		}
	}
}

void QOsgWidget::deleteIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _model->data(_model->index(first, rsModel::ID), Qt::EditRole).toInt();
	_scene->deleteChild(id);
}

QMouseHandler::QMouseHandler(rsScene::Scene *scene) : rsScene::MouseHandler(scene) {
}

int QMouseHandler::pick(const osgGA::GUIEventAdapter &ea, osgViewer::Viewer *viewer) {
	int id = rsScene::MouseHandler::pick(ea, viewer);
	if (id != -1)
		emit clickedIndex(id);
}

