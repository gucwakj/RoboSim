#ifndef QOSGWIDGET_H
#define QOSGWIDGET_H

#include <iostream>
#include <vector>

#include <osgQt/GraphicsWindowQt>

#include <rsScene/Scene>

#include "robotmodel.h"

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
	Q_OBJECT

	public:
		explicit QOsgWidget(QWidget* = 0);

		void setModel(robotModel*);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex&);
		void changeLevel(void);

	protected:
		~QOsgWidget();

	private:
		rsScene::Scene *_scene;
		robotModel *_model;
		int _current;
		int _level;
};

#endif // QOSGWIDGET_H
