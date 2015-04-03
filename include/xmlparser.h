#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <vector>

#include <QModelIndex>
#include <QObject>

#include <rs/Macros>
#include <rs/Pos>
#include <rs/Quat>
#include <rs/Vec>
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
		void newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, std::string);
		void newObstacle(int, int, double*, double*, double*, double*, double);
		void grid(std::vector<double>);
		void trace(bool);
		void units(bool);

	public slots:
		void obstacleDataChanged(QModelIndex, QModelIndex);
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

