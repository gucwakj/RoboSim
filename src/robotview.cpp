#include <rs/enum.hpp>

#include "robotview.h"

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

	// drag-drop
	this->setAcceptDrops(true);
	this->setDragEnabled(false);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setDropIndicatorShown(true);
	this->setDragDropMode(QAbstractItemView::DropOnly);
}

void robotView::dataChanged(const QModelIndex &/*topLeft*/, const QModelIndex &bottomRight) {
	this->setCurrentIndex(model()->index(bottomRight.row(), 0));
}
