#ifndef QOSGWIDGET_H_
#define QOSGWIDGET_H_

#include <vector>

#include <QEvent>
#include <QKeyEvent>
#include <QTimer>

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>

#include <rsScene/Scene>

#include "objectmodel.h"
#include "robotmodel.h"

class QListWidgetItem;

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
		Q_OBJECT
	public:
		explicit QOsgWidget(QWidget* = 0);

		void setRobotModel(robotModel*);
		void setObjectModel(objectModel*);

	signals:
		void currentTab(int);
		void objectIndexChanged(const QModelIndex&);
		void robotIndexChanged(const QModelIndex&);
		void nullIndex(void);

	public slots:
		void clickedObjectIndex(int);
		void clickedRobotIndex(int, bool = true);
		void deleteObjectIndex(QModelIndex, int, int);
		void deleteRobotIndex(QModelIndex, int, int);
		void grid(std::vector<float>);
		void gridDefaults(void);
		void gridEnabled(bool);
		void gridHash(double);
		void gridMaxX(double);
		void gridMaxY(double);
		void gridMinX(double);
		void gridMinY(double);
		void gridTics(double);
		void objectDataChanged(QModelIndex, QModelIndex);
		void reidRobot(QModelIndex);
		void robotDataChanged(QModelIndex, QModelIndex);
		void setBackgroundImage(int, std::string);
		void setCurrentBackground(int);
		void setCurrentIndex(int);
		void setCurrentObjectIndex(const QModelIndex&);
		void setCurrentRobotIndex(const QModelIndex&);
		void setNewBackground(QListWidgetItem*, QListWidgetItem*);
		void setUnits(bool);

	protected:
		~QOsgWidget(void);
		double convert(double);
		bool eventFilter(QObject*, QEvent*);
		void highlight_robots(const QModelIndex&);
		void paintEvent(QPaintEvent*) { frame(); }

	private:
		rsScene::Scene *_scene;
		robotModel *_r_model;
		objectModel *_o_model;
		bool _units;
		int _current[3];
		int _level;
		std::vector<float> _grid;
		QTimer _timer;
		osgQt::GraphicsWindowQt *_gw;
};

class QMouseHandler : public QObject, virtual public rsScene::MouseHandler {
		Q_OBJECT
	public:
		explicit QMouseHandler(rsScene::Scene*);
		virtual ~QMouseHandler(void) {};

		virtual int pick(const osgGA::GUIEventAdapter&, osgViewer::Viewer*);

	signals:
		void clickedObjectIndex(int);
		void clickedRobotIndex(int, bool);
};

#endif // QOSGWIDGET_H_
