#ifndef OBJECTVIEW_H_
#define OBJECTVIEW_H_

#include <QListView>
#include <QEvent>
#include <QKeyEvent>
#include <QProxyStyle>

#include "objectmodel.h"

class objectView : public QListView {
		Q_OBJECT
	public:
		objectView(objectModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(const QModelIndex&, const QModelIndex&);
		void setCurrentIndex(const QModelIndex&);

	protected:
		bool eventFilter(QObject*, QEvent*);
};

class objectViewStyle : public QProxyStyle {
	public:
		void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

#endif // OBSTACLEVIEW_H_
