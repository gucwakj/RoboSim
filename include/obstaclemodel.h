#ifndef OBSTACLEMODEL_H
#define OBSTACLEMODEL_H

#include <iostream>
#include <vector>

#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <QIcon>
#include <QMimeData>

#include <rs/Enum>

namespace rsObstacleModel {

	enum obstacles_list {
		ID,
		FORM,
		P_X,
		P_Y,
		P_Z,
		L_1,
		L_2,
		L_3,
		COLOR,
		MASS,
		AXIS,
		TEXT,
		NUM_COLUMNS
	};

}

class obstacleModel : public QAbstractTableModel {
		Q_OBJECT
	public:
		obstacleModel(QObject* = 0);
		~obstacleModel(void);

		// for subclassing
		int columnCount(const QModelIndex& = QModelIndex()) const;
		QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
		QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
		int rowCount(const QModelIndex& = QModelIndex()) const;

		// for editing
		Qt::ItemFlags flags(const QModelIndex&) const;
		bool setData(const QModelIndex&, const QVariant&, int = Qt::EditRole);

		// for resizing
		bool insertRows(int, int, const QModelIndex& = QModelIndex());
		bool removeRows(int, int, const QModelIndex& = QModelIndex());

		// for drag and drop
		Qt::DropActions supportedDropActions(void) const;
		bool dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&);

		// utility
		void printModel(void);
		QVariant findByID(int);

	public slots:
		bool addObstacle(int = 0, int = Qt::EditRole);
		bool newMarker(int, int, double*, double*, double*, int, std::string, int = Qt::EditRole);
		bool newObstacle(int, int, double*, double*, double*, double*, double, int = Qt::EditRole);
		void setUnits(bool);

	private:
		QVariant convert(double, bool) const;

	private:
		QList<QStringList> _list;
		int _l_preconfig[rsLinkbot::NUM_PRECONFIG];
		bool _units;			// 1: SI, 0: US
};

#endif // OBSTACLEMODEL_H

