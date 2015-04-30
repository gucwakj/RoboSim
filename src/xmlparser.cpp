#include <QDebug>

#include <rsXML/Reader>

#include "xmlparser.h"

xmlParser::xmlParser(const std::string filename) : rsXML::Writer(filename, filename) { }

void xmlParser::deleteObstacleIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _o_model->data(_o_model->index(first, rsObstacleModel::ID), Qt::EditRole).toInt();
	int form = _o_model->data(_o_model->index(first, rsObstacleModel::FORM)).toInt();
	// save obstacle
	switch (form) {
		case rs::BOX: case rs::CYLINDER: case rs::SPHERE:
			Writer::deleteObstacle(id);
			break;
		case rs::DOT: case rs::LINE: case rs::TEXT:
			Writer::deleteMarker(id);
			break;
	}
}

void xmlParser::deleteRobotIndex(QModelIndex index, int first, int last) {
	// delete child with id from index
	int id = _r_model->data(_r_model->index(first, rsRobotModel::ID), Qt::EditRole).toInt();
	Writer::deleteRobot(id);
}

void xmlParser::parse(const char *name) {
	rsXML::Reader reader(name, false);
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
		rs::Vec xmlwheels = xmlbot->getWheels();
		rs::Vec wheels;
		wheels.allocate(2);
		for (int i = 0; i < 2; i++) {
			switch (xmlbot->getForm()) {
				case rs::LINKBOTI:
				case rs::LINKBOTL: {
					if (xmlwheels[i] == rsLinkbot::TINYWHEEL)
						wheels[i] = 1;
					else if (xmlwheels[i] == rsLinkbot::SMALLWHEEL)
						wheels[i] = 2;
					else if (xmlwheels[i] == rsLinkbot::BIGWHEEL)
						wheels[i] = 3;
					else if (xmlwheels[i] == rsLinkbot::WHEEL)
						wheels[i] = 4;
					else
						wheels[i] = 0;
					break;
				}
				case rs::EV3:
				case rs::NXT: {
					if (xmlwheels[i] == rsMindstorms::SMALL)
						wheels[i] = 1;
					else if (xmlwheels[i] == rsMindstorms::BIG)
						wheels[i] = 2;
					else
						wheels[i] = 0;
					break;
				}
			}
		}
		emit newRobot(xmlbot->getID(), xmlbot->getForm(), xmlbot->getPosition(), xmlbot->getQuaternion(), xmlbot->getJoints(), xmlbot->getLED(), wheels, xmlbot->getName());
		xmlbot = reader.getNextRobot(-1);
	}

	// add all obstacles
	rsXML::Obstacle *xmlob = reader.getNextObstacle();
	while (xmlob) {
		emit newObstacle(xmlob->getID(), xmlob->getForm(), xmlob->getPosition(), xmlob->getQuaternion(), xmlob->getColor(), xmlob->getDimensions(), xmlob->getMass());
		xmlob = reader.getNextObstacle();
	}

	// add all markers
	rsXML::Marker *xmlm = reader.getNextMarker();
	while (xmlm) {
		emit newMarker(xmlm->getID(), xmlm->getForm(), xmlm->getStart(), xmlm->getEnd(), xmlm->getColor(), xmlm->getSize(), xmlm->getLabel());
		xmlm = reader.getNextMarker();
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
		rs::Pos p(_r_model->data(_r_model->index(i, rsRobotModel::P_X)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Y)).toDouble(),
				  _r_model->data(_r_model->index(i, rsRobotModel::P_Z)).toDouble());

		// get euler angles
		double r[3] = {DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_PHI)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_THETA)).toDouble()),
					   DEG2RAD(_r_model->data(_r_model->index(i, rsRobotModel::R_PSI)).toDouble())};

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

		switch (form) {
			case rs::LINKBOTI:
			case rs::LINKBOTL: {
				// joints
				rs::Vec j;
				j.allocate(3);

				// set new robot data
				Writer::setRobot(robot, name, p, q, j, c);

				// set wheel sizes
				for (int i = 0; i < 2; i++) {
					if (wheelID[i] == 1)
						wheel[i] = rsLinkbot::TINYWHEEL;
					else if (wheelID[i] == 2)
						wheel[i] = rsLinkbot::SMALLWHEEL;
					else if (wheelID[i] == 3)
						wheel[i] = rsLinkbot::BIGWHEEL;
					else if (wheelID[i] == 4)
						wheel[i] = rsLinkbot::WHEEL;
				}

				// done
				break;
			}
			case rs::EV3:
			case rs::NXT: {
				// joints
				rs::Vec j;
				j.allocate(2);

				// set new robot data
				Writer::setRobot(robot, name, p, q, j, c);

				// set wheel sizes
				for (int i = 0; i < 2; i++) {
					if (wheelID[i] == 1)
						wheel[i] = rsMindstorms::SMALL;
					else if (wheelID[i] == 2)
						wheel[i] = rsMindstorms::BIG;
				}

				// done
				break;
			}
		}

		// write wheels
		Writer::setRobotWheels(robot, wheel[0], radius, wheel[1], radius);
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

		// get name
		std::string name = _o_model->data(_o_model->index(i, rsObstacleModel::TEXT)).toString().toStdString();

		// get position
		rs::Pos p(_o_model->data(_o_model->index(i, rsObstacleModel::P_X)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObstacleModel::P_Y)).toDouble(),
				  _o_model->data(_o_model->index(i, rsObstacleModel::P_Z)).toDouble());

		// quaternion
		int axis = _o_model->data(_o_model->index(i, rsObstacleModel::AXIS)).toInt();
		rs::Quat q;
		switch (axis) {
			case 0: // x
				q[1] = sin(0.785398);
				q[3] = cos(0.785398);
				break;
			case 1: // y
				q[0] = sin(0.785398);
				q[3] = cos(0.785398);
				break;
		}

		// get led color
		QColor color(_o_model->data(_o_model->index(i, rsObstacleModel::COLOR)).toString());
		rs::Vec led(color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0);

		// save obstacle
		switch (form) {
			case rs::BOX: case rs::CYLINDER: case rs::SPHERE: {
				rs::Vec dims(_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
							 _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble());
				tinyxml2::XMLElement *obstacle = Writer::getOrCreateObstacle(form, id);
				Writer::setObstacle(obstacle, name, p, q, dims, led, mass);
				break;
			}
			case rs::DOT: case rs::LINE: case rs::TEXT: {
				rs::Pos p2(_o_model->data(_o_model->index(i, rsObstacleModel::L_1)).toDouble(),
						   _o_model->data(_o_model->index(i, rsObstacleModel::L_2)).toDouble(),
						   _o_model->data(_o_model->index(i, rsObstacleModel::L_3)).toDouble());
				tinyxml2::XMLElement *marker = Writer::getOrCreateMarker(form, id);
				Writer::setMarker(marker, name, p, p2, led, mass);
				break;
			}
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

int xmlParser::save(void) {
	return Writer::save();
}

int xmlParser::saveFile(const QString &fileName) {
	return Writer::saveFile(fileName.toStdString());
}

