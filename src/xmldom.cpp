#include <iostream>
#include "xmldom.h"

xmlDom::xmlDom(const QString &filename) {
	_status = 0;
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		std::cerr << "Error: Cannot read file " << qPrintable(filename)
				  << ": " << qPrintable(file.errorString())
				  << std::endl;
		_status = 1;
	}
	_doc.setContent(&file);
}

int xmlDom::parseConfig(int &version) {
	QDomElement config = _doc.documentElement().firstChildElement("config");

	// get version
	QDomElement child = config.firstChildElement("version");
	version = child.attributeNode("val").value().toInt();

	// success
	return 0;
}

int xmlDom::parseSim(QStandardItemModel *model) {
	_model = model;
	QDomElement sim = _doc.documentElement().firstChildElement("sim");

	// get children
	QDomElement child = sim.firstChild().toElement();
	while (!child.isNull()) {
		if	(	child.tagName() == "linkboti" ||
				child.tagName() == "linkbotl" ||
				child.tagName() == "linkbott" ||
				child.tagName() == "mobot"
			)
			this->parse_robot(child);
		child = child.nextSibling().toElement();
	}

	// success
	return 0;
}

int xmlDom::parse_robot(QDomElement &child) {
	int row = _model->rowCount();
	int col = 0;
	_model->insertRows(row, 1, QModelIndex());
	_model->setData(_model->index(row, col++, QModelIndex()), child.attributeNode("id").value());
	_model->setData(_model->index(row, col++, QModelIndex()), child.tagName());

	for (int i = 0; i < child.firstChildElement("position").attributes().length(); i++) {
		int j = child.firstChildElement("position").attributes().item(i).toAttr().value().toInt();
		_model->setData(_model->index(row, col++, QModelIndex()), j);
	}
	for (int i = 0; i < child.firstChildElement("rotation").attributes().length(); i++) {
		int j = child.firstChildElement("rotation").attributes().item(i).toAttr().value().toInt();
		_model->setData(_model->index(row, col++, QModelIndex()), j);
	}

	// success
	return 0;
}
