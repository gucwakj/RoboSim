#ifndef XMLDOM_H
#define XMLDOM_H

#include <QDomDocument>
#include <QFile>
#include <QStandardItemModel>

class xmlDom {
	public:
		xmlDom(const QString &filename);
		int parseConfig(int &version);
		int parseSim(QStandardItemModel *model);

	private:
		int parse_robot(QDomElement &child);

		QDomDocument _doc;
		QStandardItemModel *_model;
		int _status;
};

#endif // XMLDOM_H
