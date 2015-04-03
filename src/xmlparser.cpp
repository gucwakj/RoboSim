#include <QDebug>

#include <rsXML/Reader>

#include "xmlparser.h"

xmlParser::xmlParser(char *filename) : rsXML::Writer(filename, filename) { }

void xmlParser::parse(char *name) {
	rsXML::Reader reader(name);
	emit trace(reader.getTrace());
	_units = reader.getUnits();
	emit units(_units);

	// grid
	_grid = reader.getGrid();
	for (int i = 0; i < 6; i++)
		_grid[i] = ((_units) ? _grid[i]*100 : _grid[i]*39.37);
	emit grid(_grid);

	// add all robots
	rsXML::Robot *xmlbot = reader.getNextRobot(-1);
	while (xmlbot) {
		emit newRobot(xmlbot->getID(), xmlbot->getForm(), xmlbot->getPosition(), xmlbot->getQuaternion(), xmlbot->getJoints(), xmlbot->getLED(), xmlbot->getName());
		xmlbot = reader.getNextRobot(-1);
	}

	// add all obstacles
	rsXML::Ground *xmlob = reader.getNextObstacle();
	while (xmlob) {
		emit newObstacle(xmlob->getID(), xmlob->getForm(), xmlob->getPosition(), xmlob->getQuaternion(), xmlob->getColor(), xmlob->getDimensions(), xmlob->getMass());
		xmlob = reader.getNextObstacle();
	}
}

void xmlParser::setObstacleModel(obstacleModel *model) {
	// set model
	_o_model = model;
}

void xmlParser::setRobotModel(robotModel *model) {
	// set model
	_r_model = model;
}

void xmlParser::robotDataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form and id
		int form = _r_model->data(_r_model->index(i, rsRobotModel::FORM)).toInt();
		int id = _r_model->data(_r_model->index(i, rsRobotModel::ID), Qt::EditRole).toInt();

		// create robot element
		tinyxml2::XMLElement *robot = Writer::getOrCreateRobot(form, id);

		// get name
		std::string name = _r_model->data(_r_model->index(i, rsRobotModel::NAME)).toString().toStdString();

		// get position
		double p[3] = {_r_model->data(_r_model->index(i, rsRobotModel::P_X)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Y)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Z)).toDouble()};

		// get euler angles
		double r[3] = {DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_PHI)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_THETA)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_PSI)).toDouble())};

		// calculate quaternion
		rs::Quat q(sin(0.5*r[0]), 0, 0, cos(0.5*r[0]));
		q.multiply(0, sin(0.5*r[1]), 0, cos(0.5*r[1]));
		q.multiply(0, 0, sin(0.5*r[2]), cos(0.5*r[2]));
		double Q[4] = {q[0], q[1], q[2], q[3]};

		// get led color
		QColor color(_r_model->data(_r_model->index(i, rsRobotModel::COLOR)).toString());
		double c[4] = {color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0};

		// joints
		double j[3] = {0};

		// set new robot data
		Writer::setRobot(robot, name, p, Q, j, c);
	}

	// save
	Writer::save();
}

void xmlParser::obstacleDataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		// get form, id, and mass
		int form = _o_model->data(_o_model->index(i, rsObstacleModel::FORM)).toInt();
		int id = _o_model->data(_o_model->index(i, rsObstacleModel::ID), Qt::EditRole).toInt();
		int mass = _o_model->data(_o_model->index(i, rsObstacleModel::MASS), Qt::EditRole).toInt();

		// create obstacle element
		tinyxml2::XMLElement *obstacle = Writer::getOrCreateObstacle(form, id);

		// get name
		std::string name = _o_model->data(_o_model->index(i, rsObstacleModel::TEXT)).toString().toStdString();

		// get position
		double pos[3] = {_o_model->data(_o_model->index(i, rsObstacleModel::P_X)).toDouble(),
						 _o_model->data(_o_model->index(i, rsObstacleModel::P_Y)).toDouble(),
						 _o_model->data(_o_model->index(i, rsObstacleModel::P_Z)).toDouble()};

		// get dimensions
		double dims[3] = {_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
						  _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
						  _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble()};

		// quaternion
		int axis = _o_model->data(_o_model->index(i, rsObstacleModel::AXIS)).toInt();
		double quat[4] = {0, 0, 0, 1};
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
		double led[4] = {color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0};

		// save obstacle
		switch (form) {
			case rs::BOX:
			case rs::CYLINDER:
			case rs::SPHERE:
				Writer::setObstacle(obstacle, name, pos, quat, dims, led, mass);
				break;
			case rs::DOT:
			case rs::LINE:
			case rs::TEXT:
				//Writer::setMarker(obstacle, id, form, name, pos, led, dims, quat);
				break;
		}
	}

	// save
	Writer::save();
}

void xmlParser::setGridEnabled(bool enabled) {
	_grid[6] = static_cast<double>(enabled);
	Writer::setGrid(_grid);
}

void xmlParser::setGridHash(double value) {
	_grid[1] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setGridMaxX(double value) {
	_grid[3] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setGridMaxY(double value) {
	_grid[5] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setGridMinX(double value) {
	_grid[2] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setGridMinY(double value) {
	_grid[4] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setGridTics(double value) {
	_grid[0] = value;
	Writer::setGrid(_grid);
}

void xmlParser::setTrace(bool state) {
	Writer::setTrace(state);
}

void xmlParser::setUnits(bool state) {
	_units = state;
	Writer::setUnits(state);
}

