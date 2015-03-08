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

	// set highlighting of click
	_scene->setHighlight(true);
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
    this->unref();
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
		int form = _model->data(_model->index(i, rsModel::FORM)).toInt();
		int id = _model->data(_model->index(i, rsModel::ID), Qt::EditRole).toInt();
		double pos[3] = {_model->data(_model->index(i, rsModel::P_X)).toDouble(),
						 _model->data(_model->index(i, rsModel::P_Y)).toDouble(),
						 _model->data(_model->index(i, rsModel::P_Z)).toDouble() + 0.04445};
		double quat[4] = {0, 0, 0, 1};
		double led[4] = {0, 0, 0, 1};

		switch (form) {
			case rs::LINKBOTI: {
				_scene->deleteChild(id);
				rsRobots::LinkbotI *robot = new rsRobots::LinkbotI();
				robot->setID(id);
				rsScene::Robot *sceneRobot = _scene->drawRobot(robot, pos, quat, led, 0);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE1, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE2, 1, 0, 2, rsLinkbot::CASTER);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 1, -1);
				_scene->drawConnector(robot, sceneRobot, rsLinkbot::SIMPLE, rsLinkbot::FACE3, 1, 0, 2, rsLinkbot::SMALLWHEEL);
				break;
			}
			case rs::LINKBOTL:
				_scene->drawRobot(new rsRobots::LinkbotL(), pos, quat, led, 0);
				break;
			case rs::LINKBOTT:
				_scene->drawRobot(new rsRobots::LinkbotT(), pos, quat, led, 0);
				break;
			default:
				break;
		}
		_scene->addChild();
	}
	// set current robot
	this->setCurrentIndex(bottomRight);
}

void QOsgWidget::setCurrentIndex(const QModelIndex &index) {
	// get new robot ID
	int newRobot = _model->data(_model->index(index.row(), rsModel::ID), Qt::EditRole).toInt();

	// do nothing when indices are the same
	if (_current == newRobot) return;

	// set new current robot
	_current = newRobot;

	// update view
	_scene->addHighlight(_current);
}
