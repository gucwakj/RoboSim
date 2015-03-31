#ifndef QOSGWIDGET_H
#define QOSGWIDGET_H

#include <iostream>
#include <vector>

#include <QEvent>
#include <QKeyEvent>

#include <osgQt/GraphicsWindowQt>

#include <rsScene/Scene>

#include "obstaclemodel.h"
#include "robotmodel.h"

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
		Q_OBJECT
	public:
		explicit QOsgWidget(QWidget* = 0);

		void setRobotModel(robotModel*);
		void setObstacleModel(obstacleModel*);

	signals:
		void obstacleIndexChanged(const QModelIndex&);
		void robotIndexChanged(const QModelIndex&);
		void nullIndex(void);

	public slots:
		void changeLevel(void);
		void clickedIndex(int);
		void deleteObstacleIndex(QModelIndex, int, int);
		void deleteRobotIndex(QModelIndex, int, int);
		void gridDefaults(void);
		void gridEnabled(bool);
		void gridHash(double);
		void gridMaxX(double);
		void gridMaxY(double);
		void gridMinX(double);
		void gridMinY(double);
		void gridTics(double);
		void obstacleDataChanged(QModelIndex, QModelIndex);
		void robotDataChanged(QModelIndex, QModelIndex);
		void setCurrentObstacleIndex(const QModelIndex&);
		void setCurrentRobotIndex(const QModelIndex&);
		void setUnits(bool);

	protected:
		~QOsgWidget();
		bool eventFilter(QObject*, QEvent*);
		double convert(double);

	private:
		rsScene::Scene *_scene;
		robotModel *_r_model;
		obstacleModel *_o_model;
		bool _units;
		int _current;
		int _level;
		std::vector<double> _grid;
};

class QMouseHandler : public QObject, virtual public rsScene::MouseHandler {
		Q_OBJECT
	public:
		explicit QMouseHandler(rsScene::Scene*);
		virtual ~QMouseHandler(void) {};

		virtual int pick(const osgGA::GUIEventAdapter&, osgViewer::Viewer*);

	signals:
		void clickedIndex(int);
};

#endif // QOSGWIDGET_H
