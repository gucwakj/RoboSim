#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <vector>

#include <QModelIndex>
#include <QObject>

#include <rs/Macros>
#include <rsXML/Writer>

#include "obstaclemodel.h"
#include "robotmodel.h"

class xmlParser : public QObject, public rsXML::Writer {
		Q_OBJECT
	public:
		xmlParser(char*);
		virtual ~xmlParser(void) {};
		void parse(char*);
		void setRobotModel(robotModel*);
		void setObstacleModel(obstacleModel*);

	signals:
		void grid(std::vector<double>);
		void trace(bool);
		void units(bool);

	public slots:
		void robotDataChanged(QModelIndex, QModelIndex);
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
		obstacleModel *_o_model;
		robotModel *_r_model;
};

#endif // XMLPARSER_H_

