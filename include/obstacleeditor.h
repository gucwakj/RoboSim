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
#include "roboteditor.h"

/*class colorEditor : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit colorEditor(QWidget* = 0);
		virtual ~colorEditor(void) {};

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
};*/

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
		colorEditor *_colorEditor;
};
/*
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
		colorEditor *_colorEditor;
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
		colorEditor *_colorEditor;
};
*/
class obstacleEditorDelegate : public QItemDelegate {
		Q_OBJECT
	public:
		obstacleEditorDelegate(QObject* = 0);
		void setEditorData(QWidget*, const QModelIndex&) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif // OBSTACLEEDITOR_H_
