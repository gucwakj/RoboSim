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
#include <QPushButton>
#include <QMetaProperty>

#include "robotmodel.h"

class ColorEditor : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit ColorEditor(QWidget *parent = 0);
		virtual ~ColorEditor(void) {};

		QColor color() const;

	signals:
		void colorChanged(const QColor &color);

	public slots:
		void setColor(const QColor &color);

	private slots:
		void onButtonClicked();

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
		void customWheel(int);
		void rotate(double);
		void setUnits(bool);

	private:
		robotModel *_model;
		QDataWidgetMapper *_mapper;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelUnits;
		QComboBox *_wheelBox;
		ColorEditor *_colorEditor;
};

class robotEditorDelegate : public QItemDelegate {
		Q_OBJECT
	public:
		robotEditorDelegate(QObject* = 0);
		void setEditorData(QWidget*, const QModelIndex&) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif // ROBOTEDITOR_H_
