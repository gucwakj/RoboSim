#ifndef OBSTACLEEDITOR_H_
#define OBSTACLEEDITOR_H_

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

#include "obstaclemodel.h"

class bodyColorPicker : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit bodyColorPicker(QWidget* = 0);
		virtual ~bodyColorPicker(void) {};

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

class obstacleEditor : public QWidget {
		Q_OBJECT
	public:
		obstacleEditor(obstacleModel*, QWidget* = 0);

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
		obstacleModel *_model;
		QDataWidgetMapper *_mapper;
		QStackedWidget *_pages;
		QPushButton *_deleteButton;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		bool _units;
};

class boxEditor : public QWidget {
		Q_OBJECT
	public:
		boxEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	private:
		QDataWidgetMapper *_mapper;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		bodyColorPicker *_colorPicker;
};

class cylinderEditor : public QWidget {
		Q_OBJECT
	public:
		cylinderEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	private:
		QDataWidgetMapper *_mapper;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		bodyColorPicker *_colorPicker;
};

class sphereEditor : public QWidget {
		Q_OBJECT
	public:
		sphereEditor(QDataWidgetMapper*, QWidget* = 0);
		void nullIndex(bool);
		void setUnits(bool);

	private:
		QDataWidgetMapper *_mapper;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		bodyColorPicker *_colorPicker;
};

class obstacleEditorDelegate : public QItemDelegate {
		Q_OBJECT
	public:
		obstacleEditorDelegate(QObject* = 0);
		void setEditorData(QWidget*, const QModelIndex&) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif // OBSTACLEEDITOR_H_

