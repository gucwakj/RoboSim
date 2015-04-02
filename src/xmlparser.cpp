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

void xmlParser::dataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
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

