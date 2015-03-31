#ifndef ROBOTEDITOR_H_
#define ROBOTEDITOR_H_

#include <iostream>

#include <QLabel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QWidget>
#include <QColorDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QItemDelegate>
#include <QComboBox>
#include <QStringListModel>
#include <QStackedWidget>
#include <QPushButton>
#include <QMetaProperty>

#include "robotmodel.h"

class ledColorPicker : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit ledColorPicker(QWidget* = 0);
		virtual ~ledColorPicker(void) {};

		QColor color(void) const;

	signals:
		void colorChanged(const QColor);

	public slots:
		void setColor(const QColor);

	private slots:
		void onButtonClicked(void);

	private:
		QColor _color;
		QPushButton *_button;
};

class robotEditor : public QWidget {
		Q_OBJECT
	public:
		robotEditor(robotModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex&);

	protected slots:
		void buttonPressed(void);
		void deleteCurrentIndex(void);
		void setUnits(bool);

	private:
		robotModel *_model;
		QDataWidgetMapper *_mapper;
		QStackedWidget *_pages;
		QPushButton *_deleteButton;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		bool _units;
};

class individualEditor : public QWidget {
		Q_OBJECT
	public:
		individualEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	protected slots:
		void rotate(double);

	private:
		QDataWidgetMapper *_mapper;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelUnits;
		QComboBox *_wheelBox;
		ledColorPicker *_colorPicker;
};

class customEditor : public QWidget {
		Q_OBJECT
	public:
		customEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	protected slots:
		void rotate(double);

	private:
		QDataWidgetMapper *_mapper;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelUnits;
		QComboBox *_wheelBox;
		QLabel *_radiusUnits;
		ledColorPicker *_colorPicker;
};

class preconfigEditor : public QWidget {
		Q_OBJECT
	public:
		preconfigEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	protected slots:
		void rotate(double);

	private:
		QDataWidgetMapper *_mapper;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		ledColorPicker *_colorPicker;
};

class robotEditorDelegate : public QItemDelegate {
		Q_OBJECT
	public:
		robotEditorDelegate(QObject* = 0);
		void setEditorData(QWidget*, const QModelIndex&) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif // ROBOTEDITOR_H_
