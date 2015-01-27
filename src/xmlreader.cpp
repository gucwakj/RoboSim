#include "xmlreader.h"

xmlReader::xmlReader(QTableWidget *table) {
	_table = table;
}

bool xmlReader::read(const QString &fileName) {
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		std::cerr << "Error: Cannot read file " << qPrintable(fileName)
				  << ": " << qPrintable(file.errorString())
				  << std::endl;
		return false;
	}
	_reader.setDevice(&file);
	_reader.readNext();
	while (!_reader.atEnd()) {
		if (_reader.isStartElement()) {
			std::cerr << "Value: " << qPrintable(_reader.name().toString()) << std::endl;
			if (_reader.name() == "config")
				read_config_element();
			else if (_reader.name() == "graphics")
				read_graphics_element();
			else if (_reader.name() == "ground")
				read_ground_element();
			else if (_reader.name() == "sim")
				read_sim_element();
			else {
				std::cerr << "error: " << std::endl;
				_reader.readNext();
				//_reader.raiseError(QObject::tr("Not a bookindex file"));
			}
		}
		else {
			_reader.readNext();
		}
	}
	file.close();
	if (_reader.hasError()) {
		std::cerr << "Error: Failed to parse file "
				  << qPrintable(fileName) << ": "
				  << qPrintable(_reader.errorString()) << std::endl;
		return false;
	} else if (file.error() != QFile::NoError) {
		std::cerr << "Error: Cannot read file " << qPrintable(fileName)
				  << ": " << qPrintable(file.errorString())
				  << std::endl;
		return false;
	}
	return true;
}

int xmlReader::getVersion(void) {
	return _version;
}

void xmlReader::read_config_element(void) {
	std::cerr << "entering config element" << std::endl;
	while (!_reader.atEnd()) {
		_reader.readNextStartElement();
		if (_reader.isEndElement()) {
			_reader.readNextStartElement();
			break;
		}

		if (_reader.isStartElement()) {
			if (_reader.name() == "version")
                _version = 0;//_reader.attributes().value("val").toInt();
			else
				_reader.readNextStartElement();

			_reader.readNextStartElement();
		}
		else {
			_reader.readNextStartElement();
		}
	}
	//std::cerr << "version: " << _version << std::endl;
	std::cerr << "leaving config element" << std::endl;

	return;
}

void xmlReader::read_graphics_element(void) {
	std::cerr << "entering graphics element" << std::endl;
	while (!_reader.atEnd()) {
		_reader.readNextStartElement();
		if (_reader.isEndElement()) {
			_reader.readNextStartElement();
			break;
		}

		if (_reader.isStartElement()) {
			if (_reader.name() == "grid") {
				//_grid[0] = _reader.attributes().value("minx").toDouble();
				//_grid[1] = _reader.attributes().value("maxx").toDouble();
				//_grid[2] = _reader.attributes().value("miny").toDouble();
				//_grid[3] = _reader.attributes().value("maxy").toDouble();
				int a = 9;
			}
			if (_reader.name() == "tracking") {
				//_tracked = _reader.attributes().value("val").toInt();
				int b = 3;
			}
			else
				_reader.readNextStartElement();

			_reader.readNextStartElement();
		}
		else {
			_reader.readNextStartElement();
		}
	}
	//std::cerr << "version: " << _version << std::endl;
	std::cerr << "leaving config element" << std::endl;

	return;
}

void xmlReader::read_ground_element(void) {
	std::cerr << "entering ground element" << std::endl;
}

void xmlReader::read_sim_element(void) {
	std::cerr << "entering sim element" << std::endl;
}

void xmlReader::readEntryElement(QTableWidgetItem *parent) {
	QTableWidgetItem *item = new QTableWidgetItem();

	_reader.readNext();
	while (!_reader.atEnd()) {
		if (_reader.isEndElement()) {
			_reader.readNext();
			break;
		}

		if (_reader.isStartElement()) {
			if (_reader.name() == "entry")
				readEntryElement(item);
			else if (_reader.name() == "page")
				readPageElement(item);
			else
				skip_unknown_element();
		}
		else {
			_reader.readNext();
		}
	}

	return;
}

void xmlReader::readPageElement(QTableWidgetItem *parent) {
}

void xmlReader::skip_unknown_element(void) {
	_reader.readNext();
	while (!_reader.atEnd()) {
		if (_reader.isEndElement()) {
			_reader.readNext();
			break;
		}

		if (_reader.isStartElement()) {
			skip_unknown_element();
		} else {
			_reader.readNext();
		}
	}
}
