#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <QModelIndex>
#include <QObject>

#include <rsXML/Writer>

class xmlParser : public QObject, public rsXML::Writer {
		Q_OBJECT
	public:
		xmlParser(char*);
		virtual ~xmlParser(void) {};
		void parse(char*);

	signals:
		void trace(bool);
		void units(bool);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setTrace(bool);
		void setUnits(bool);
};

#endif // XMLPARSER_H_

