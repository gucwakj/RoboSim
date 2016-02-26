#include <QPainter>
#include <QColor>
#include <QPen>

#include <rs/Enum>

#include "objectview.h"

/*!
 *
 *
 *	objectView
 *
 *
 */

objectView::objectView(objectModel *model, QWidget *parent) : QListView(parent) {
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
	this->setModelColumn(rsObjectModel::ID);
	this->setUniformItemSizes(false);
	this->installEventFilter(this);
	this->setToolTip("Click on an object to edit.\nUse Backspace Key to delete.");
	this->setToolTipDuration(-1);

	// drag-drop
	this->viewport()->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setDropIndicatorShown(true);
	this->setDragDropMode(QAbstractItemView::DropOnly);
	this->setStyle(new objectViewStyle());
}

void objectView::dataChanged(const QModelIndex &/*topLeft*/, const QModelIndex &bottomRight) {
	this->setCurrentIndex(model()->index(bottomRight.row(), 0));
}

void objectView::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid())
		QListView::setCurrentIndex(index);
	else
		QListView::clearSelection();
}

bool objectView::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		switch (keyEvent->key()) {
			case Qt::Key_Backspace:
			case Qt::Key_Delete: {
				QModelIndex index = this->currentIndex();
				this->model()->removeRows(index.row(), 1);
				emit indexChanged(this->currentIndex());
				break;
			}
			default:
				return QObject::eventFilter(obj, event);
				break;
		}
		return true;
	}
	else
		return QObject::eventFilter(obj, event);
}

/*!
 *
 *
 *	objectViewStyle
 *
 *
 */

/*!	\brief draw primitive element.
 *
 *	Draw the elements of this object view.  A special dropIndicator is
 *	used here.
 *
 */

void objectViewStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
	if (element == QStyle::PE_IndicatorItemViewItemDrop) {
		painter->setRenderHint(QPainter::Antialiasing, true);

		// fill object view area with 'add object' message
		QSize size = dynamic_cast<const objectView *>(widget)->viewport()->frameSize();
		QRect dropRect = QRect(0, 0, size.width(), size.height());

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
		painter->drawText(dropRect, Qt::AlignCenter, tr("Add New Object to RoboSim"));
	}
	else {
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
}

