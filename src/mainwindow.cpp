#include "mainwindow.h"
#include "robosimwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	// set up UI from forms file
	ui = new Ui::MainWindow;
	ui->setupUi(this);

	// create robosim widget
	roboSimWidget *widget = new roboSimWidget(this);

	// load widget
	this->setContentsMargins(0, 0, 0, 0);
	this->setCentralWidget(widget);

	// menu actions
	QWidget::connect(ui->action_About, SIGNAL(triggered()), widget, SLOT(about()));
	QWidget::connect(ui->action_AboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	QWidget::connect(ui->action_Load, SIGNAL(triggered()), widget, SLOT(load()));
	QWidget::connect(ui->action_Save, SIGNAL(triggered()), widget, SLOT(save()));
	QWidget::connect(ui->action_SaveAs, SIGNAL(triggered()), widget, SLOT(saveAs()));
	QWidget::connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

	// create status bar
	this->statusBar()->showMessage("Loaded", 2000);
	QWidget::connect(widget, SIGNAL(statusMessage(const QString&, int)), this->statusBar(), SLOT(showMessage(const QString&, int)));
}

MainWindow::~MainWindow(void) {
}

