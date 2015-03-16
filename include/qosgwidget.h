#ifndef QOSGWIDGET_H
#define QOSGWIDGET_H

#include <iostream>
#include <vector>

#include <QEvent>
#include <QKeyEvent>

#include <osgQt/GraphicsWindowQt>

#include <rsScene/Scene>

#include "robotmodel.h"

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
		Q_OBJECT
	public:
		explicit QOsgWidget(QWidget* = 0);

		void setModel(robotModel*);

	signals:
		void indexChanged(const QModelIndex&);
		void nullIndex(void);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex&);
		void changeLevel(void);
		void clickedIndex(int);
		void deleteIndex(QModelIndex, int, int);

	protected:
		~QOsgWidget();
		bool eventFilter(QObject*, QEvent*);

	private:
		rsScene::Scene *_scene;
		robotModel *_model;
		int _current;
		int _level;
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
