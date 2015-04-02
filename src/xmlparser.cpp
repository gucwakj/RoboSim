#include <QDebug>

#include <rsXML/Reader>

#include "xmlparser.h"

xmlParser::xmlParser(char *filename) : rsXML::Writer(filename, filename) { }

void xmlParser::parse(char *name) {
	rsXML::Reader reader(name);
	emit trace(reader.getTrace());
	emit units(reader.getUnits());
}

void xmlParser::dataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
}

void xmlParser::setTrace(bool state) {
	Writer::setTrace(state);
}

void xmlParser::setUnits(bool state) {
	Writer::setUnits(state);
}

