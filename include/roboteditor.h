#ifndef ROBOTEDITOR_H_
#define ROBOTEDITOR_H_

#include <iostream>

#include <QLabel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QItemDelegate>
#include <QComboBox>
#include <QStringListModel>
#include <QPushButton>
#include <QMetaProperty>

#include "robotmodel.h"

class robotEditor : public QWidget {
		Q_OBJECT
	public:
		robotEditor(robotModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex &);

	protected slots:
		void buttonPressed(void);
		void rotate(double);

	private:
		robotModel *_model;
		QDataWidgetMapper *_mapper;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		QDoubleSpinBox *_rZBox;
};

class robotEditorDelegate : public QItemDelegate {
		Q_OBJECT
	public:
		robotEditorDelegate(QObject* = 0);
		void setEditorData(QWidget*, const QModelIndex&) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif // ROBOTEDITOR_H_
