#ifndef ROBOTVIEW_H_
#define ROBOTVIEW_H_

#include <iostream>

#include <QListView>
#include <QEvent>
#include <QKeyEvent>
#include <QProxyStyle>

#include "robotmodel.h"

class robotView : public QListView {
		Q_OBJECT
	public:
		robotView(robotModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(const QModelIndex&, const QModelIndex&);
		void setCurrentIndex(const QModelIndex&);

	protected:
		bool eventFilter(QObject*, QEvent*);
};

class robotViewStyle : public QProxyStyle {
	public:
		void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

#endif // ROBOTVIEW_H_
