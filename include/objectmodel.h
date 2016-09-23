#ifndef OBJECTMODEL_H_
#define OBJECTMODEL_H_

#include <vector>

#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <QIcon>
#include <QMimeData>

#include <rs/Enum>
#include <rs/Pos>
#include <rs/Quat>
#include <rs/Vec>

namespace rsObjectModel {

	enum objects_list {
		ID,
		FORM,
		P_X,
		P_Y,
		P_Z,
		R_PHI,
		R_THETA,
		R_PSI,
		L_1,
		L_2,
		L_3,
		COLOR,
		MASS,
		SIZE,
		AXIS,
		TEXT,
		NUM_COLUMNS
	};

}

class objectModel : public QAbstractTableModel {
		Q_OBJECT
	public:
		objectModel(QObject* = 0);
		~objectModel(void);

		// for subclassing
		int columnCount(const QModelIndex& = QModelIndex()) const;
		QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
		QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
		int rowCount(const QModelIndex& = QModelIndex()) const;
		void sort(int, Qt::SortOrder = Qt::AscendingOrder);

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
		void clear(void);
		void printModel(void);
		QVariant findByID(int);

	public slots:
		bool addObject(int = 0, int = Qt::EditRole);
		bool newMarker(int, int, const rs::Pos&, const rs::Pos&, const rs::Pos&, const rs::Vec&, int, std::string, int = Qt::EditRole);
		bool newObstacle(int, int, const rs::Pos&, const rs::Quat&, const rs::Vec&, const rs::Vec&, double, int = Qt::EditRole);
		void setUnits(bool);

	private:
		QVariant convertLength(double, bool) const;
		QVariant convertMass(double, bool) const;

	private:
		QList<QStringList> _list;
		bool _units;			// 1: SI, 0: US
};

#endif // OBJECTMODEL_H_

