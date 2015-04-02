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
	int i = topLeft.row();
	tinyxml2::XMLElement *robot = Writer::getOrCreateRobot(	_r_model->data(_r_model->index(topLeft.row(), rsRobotModel::FORM), Qt::EditRole).toInt(),
															_r_model->data(_r_model->index(topLeft.row(), rsRobotModel::ID), Qt::EditRole).toInt());

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
	Writer::setRobot(robot, p, Q, j, c);

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

