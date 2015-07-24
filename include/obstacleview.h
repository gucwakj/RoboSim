#ifndef OBSTACLEVIEW_H_
#define OBSTACLEVIEW_H_

#include <QListView>
#include <QEvent>
#include <QKeyEvent>
#include <QProxyStyle>

#include "obstaclemodel.h"

class obstacleView : public QListView {
		Q_OBJECT
	public:
		obstacleView(obstacleModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(const QModelIndex&, const QModelIndex&);
		void setCurrentIndex(const QModelIndex&);

	protected:
		bool eventFilter(QObject*, QEvent*);
};

class obstacleViewStyle : public QProxyStyle {
	public:
		void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

#endif // OBSTACLEVIEW_H_
