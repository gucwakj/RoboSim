#include <QPainter>
#include <QColor>
#include <QPen>

#include <rs/Enum>

#include "robotview.h"

/*!
 *
 *
 *	robotView
 *
 *
 */

robotView::robotView(robotModel *model, QWidget *parent) : QListView(parent) {
	// set model
	this->setModel(model);

	// set qlistview properties
	this->setViewMode(QListView::IconMode);
	this->setMovement(QListView::Static);
	this->setResizeMode(QListView::Adjust);
	this->setIconSize(QSize(48, 48));
	this->setMinimumWidth(64);
	this->setSpacing(12);
	this->setCurrentIndex(model->index(0, 0));
	this->setModelColumn(rsModel::ID);
	this->setUniformItemSizes(true);

	// drag-drop
	this->viewport()->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setDropIndicatorShown(true);
	this->setDragDropMode(QAbstractItemView::DropOnly);
	this->setStyle(new robotViewStyle());
}

void robotView::dataChanged(const QModelIndex &/*topLeft*/, const QModelIndex &bottomRight) {
	this->setCurrentIndex(model()->index(bottomRight.row(), 0));
}

void robotView::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid())
		QListView::setCurrentIndex(index);
	else
		QListView::clearSelection();
}

/*!
 *
 *
 *	robotViewStyle
 *
 *
 */

/*!	\brief draw primitive element.
 *
 *	Draw the elements of this robot view.  A special dropIndicator is
 *	used here.
 *
 */

void robotViewStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
	if (element == QStyle::PE_IndicatorItemViewItemDrop) {
		painter->setRenderHint(QPainter::Antialiasing, true);

		// fill robot view area with 'add robot' message
		QSize size = dynamic_cast<const robotView *>(widget)->viewport()->frameSize();
		QRect dropRect = QRect(0, 0, size.width(), size.height());

		/*// add small robot box next to last one
		// get position of last robot in list
		const robotView *rv = static_cast<const robotView *>(widget);
		QRect rect = rv->visualRect(rv->model()->index(rv->model()->rowCount() - 1, 0));
		// position new one one unit over
		QRect dropRect;
		if (rect.x() + rect.width() + rect.width()/2 + 2*rv->spacing() > size.width())
			dropRect = QRect(rv->spacing(), rect.y() + rect.height() + rv->spacing(), rect.width(), rect.height());
		else
			dropRect = QRect(rect.x() + rect.width() + rv->spacing(), rect.y(), rect.width(), rect.height());
		*/

		// draw rectangle
		QPalette palette;
		QColor cRect(palette.highlight().color());
		cRect.setAlpha(100);
		QBrush brushRect(cRect);
		QPen penRect(brushRect, 2);
		painter->setPen(penRect);
		painter->setBrush(brushRect);
		painter->drawRoundedRect(dropRect, 5, 5);

		// draw text
		QColor cText(palette.highlightedText().color());
		cText.setAlpha(200);
		QPen penText(cText);
		painter->setPen(penText);
		painter->drawText(dropRect, Qt::AlignCenter, tr("Add New Robot to RoboSim"));
	}
	else {
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
}

