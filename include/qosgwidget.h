#ifndef QOSGWIDGET_H
#define QOSGWIDGET_H

#include <iostream>
#include <vector>

#include <osgQt/GraphicsWindowQt>

#include <rsScene/scene.hpp>

#include "robotmodel.h"

class QOsgWidget : public osgQt::GLWidget, public osgViewer::Viewer {
	Q_OBJECT

	public:
		explicit QOsgWidget(QWidget* = 0);

		void setModel(robotModel*);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex&);

	protected:
		~QOsgWidget();

	private:
		rsScene::Scene *_scene;
		robotModel *_model;
		std::vector<int> _robots;
		int _current;
};

#endif // QOSGWIDGET_H
