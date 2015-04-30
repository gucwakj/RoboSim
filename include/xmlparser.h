#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <vector>

#include <QModelIndex>
#include <QObject>

#include <rs/Macros>
#include <rs/Pos>
#include <rs/Quat>
#include <rs/Vec>
#include <rsXML/Reader>
#include <rsXML/Writer>

#include "obstaclemodel.h"
#include "robotmodel.h"

class xmlParser : public QObject, public rsXML::Writer {
		Q_OBJECT
	public:
		xmlParser(const std::string);
		virtual ~xmlParser(void) {};
		void parse(const char*);
		void setRobotModel(robotModel*);
		void setObstacleModel(obstacleModel*);

	signals:
		void newMarker(int, int, rs::Pos, rs::Pos, rs::Vec, int, std::string);
		void newObstacle(int, int, rs::Pos, rs::Quat, rs::Vec, rs::Vec, double);
		void newRobot(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, const rs::Vec&, std::string);
		void grid(std::vector<double>);
		void trace(bool);
		void units(bool);

	public slots:
		void deleteObstacleIndex(QModelIndex, int, int);
		void deleteRobotIndex(QModelIndex, int, int);
		void obstacleDataChanged(QModelIndex, QModelIndex);
		void robotDataChanged(QModelIndex, QModelIndex);
		int save(void);
		int saveFile(const QString&);
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

