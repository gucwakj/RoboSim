#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <vector>

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
		void grid(std::vector<double>);
		void trace(bool);
		void units(bool);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setGridEnabled(bool);
		void setGridHash(double);
		void setGridMaxX(double);
		void setGridMaxY(double);
		void setGridMinX(double);
		void setGridMinY(double);
		void setGridTics(double);
		void setTrace(bool);
		void setUnits(bool);

	private:
		bool _units;
		std::vector<double> _grid;
};

#endif // XMLPARSER_H_

