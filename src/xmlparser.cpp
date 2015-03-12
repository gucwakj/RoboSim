#include <iostream>

#include <QFile>

#include "xmlparser.h"

xmlParser::xmlParser(char *filename) : rsXML::Reader(filename), rsXML::Writer(filename) {
    /*QString fileName = "/home/kgucwa/projects/playground/RS/RoboSim/robosimrc";
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		//QMessageBox::warning(this, tr("RoboSim"), tr("Cannot read file %1.").arg(fileName));
		//return;
	}*/
}

void xmlParser::dataChanged(QModelIndex topLeft, QModelIndex bottomRight) {
}

