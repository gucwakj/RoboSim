#ifndef ROBOTMODEL_H
#define ROBOTMODEL_H

#include <iostream>
#include <vector>

#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <QIcon>

#include <rs/enum.hpp>

namespace rsModel {

	enum robot_item_list {
		ID,
		FORM,
		P_X,
		P_Y,
		P_Z,
		R_PHI,
		R_THETA,
		R_PSI,
		RADIUS,
		WHEEL,
		PRECONFIG,
		NUM_COLUMNS
	};

}

class robotModel : public QAbstractTableModel {
		Q_OBJECT
	public:
		robotModel(QObject* = 0);
		~robotModel(void);

		// for subclassing
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;

		// for editing
		Qt::ItemFlags flags(const QModelIndex&) const;
		bool setData(const QModelIndex&, const QVariant&, int = Qt::EditRole);

		// for resizing
		bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
		bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

		// for drag and drop
		Qt::DropActions supportedDropActions(void) const;

		// utility
		void printModel(void);

	signals:

	public slots:
		bool addRobot(int = Qt::EditRole);
		bool addPreconfig(int = 1, int = Qt::EditRole);

	private:
		QList<QStringList> _list;
		int _l_preconfig[rsLinkbot::NUM_PRECONFIG];
};

#endif // ROBOTMODEL_H
