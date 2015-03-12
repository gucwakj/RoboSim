#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <QModelIndex>
#include <QObject>

#include <rsXML/Reader>
#include <rsXML/Writer>

class xmlParser : public QObject, public rsXML::Reader, public rsXML::Writer {
		Q_OBJECT

	public:
		xmlParser(char*);
		virtual ~xmlParser(void) {};

	public slots:
		void dataChanged(QModelIndex, QModelIndex);

	private:
		int _version;
		tinyxml2::XMLDocument _doc;
};

#endif // XMLPARSER_H_

