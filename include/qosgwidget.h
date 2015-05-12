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

class QListWidgetItem;

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
		Q_OBJECT
	public:
		explicit QOsgWidget(QWidget* = 0);

		void setRobotModel(robotModel*);
		void setObstacleModel(obstacleModel*);

	signals:
		void currentTab(int);
		void obstacleIndexChanged(const QModelIndex&);
		void robotIndexChanged(const QModelIndex&);
		void nullIndex(void);

	public slots:
		void clickedObstacleIndex(int);
		void clickedRobotIndex(int);
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
		void setBackgroundImage(int, std::string);
		void setCurrentBackground(int);
		void setCurrentIndex(int);
		void setCurrentObstacleIndex(const QModelIndex&);
		void setCurrentRobotIndex(const QModelIndex&);
		void setNewBackground(QListWidgetItem*, QListWidgetItem*);
		void setUnits(bool);

	protected:
		~QOsgWidget(void);
		double convert(double);
		bool eventFilter(QObject*, QEvent*);
		void highlight_robots(const QModelIndex&);

	private:
		rsScene::Scene *_scene;
		robotModel *_r_model;
		obstacleModel *_o_model;
		bool _units;
		int _current[3];
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
		void clickedObstacleIndex(int);
		void clickedRobotIndex(int);
};

#endif // QOSGWIDGET_H
