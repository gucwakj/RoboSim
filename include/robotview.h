#ifndef ROBOTVIEW_H_
#define ROBOTVIEW_H_

#include <iostream>

#include <QListView>

#include "robotmodel.h"

class robotView : public QListView {
		Q_OBJECT
	public:
		robotView(robotModel*, QWidget* = 0);

	public slots:
		void dataChanged(const QModelIndex&, const QModelIndex&);
};

#endif // ROBOTVIEW_H_
