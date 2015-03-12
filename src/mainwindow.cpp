#include "mainwindow.h"
#include "roboteditor.h"
#include "robotmodel.h"
#include "robotview.h"
#include "xmlparser.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	// set up UI from forms file
	ui = new Ui::MainWindow;
	ui->setupUi(this);

	// parse xml
	//xmlParser *xml = new xmlParser("/home/kgucwa/.robosimrc");

	// build robot selector
	QStringList names, icons;
	names << "Linkbot I" <<  "Linkbot L" << "Mobot" << "NXT";
	icons << "linkbotI.jpg" << "linkbotL.jpg" << "mobot.jpg" << "mobot.jpg";
	this->build_selector(ui->list_robots, names, icons);

	// build preconfig selector
	names.clear();
	icons.clear();
	names << "Bow" <<  "Explorer" << "Four Bot Drive" << "Four Wheel Drive" << "Four Wheel Explorer" << "Group Bow" << "Inchworm" << "Lift" << "Omnidrive" << "Snake" << "Stand";
	icons << "bow.jpg" <<  "explorer.jpg" << "fourbotdrive.jpg" << "fourwheeldrive.jpg" << "fourwheelexplorer.jpg" << "groupbow.jpg" << "inchworm.jpg" << "lift.jpg" << "omnidrive.jpg" << "snake.jpg" << "stand.jpg";
	this->build_selector(ui->list_preconfig, names, icons);

	// build obstacles selector
	names.clear();
	icons.clear();
	names << "Box" <<  "Cylinder" << "Sphere";
	icons << "box.jpg" << "cylinder.jpg" << "sphere.jpg";
	this->build_selector(ui->list_obstacles, names, icons);

	// build drawings selector
	names.clear();
	icons.clear();
	names << "Line" <<  "Point" << "Text";
	icons << "line.jpg" << "point.jpg" << "text.jpg";
	this->build_selector(ui->list_drawings, names, icons);

	// set up robot model
	robotModel *model = new robotModel(this);

	// set up osg view
	ui->osgWidget->setModel(model);

	// set up robot view
	robotView *view = new robotView(model);
	ui->layout_robots->addWidget(view);

	// set up robot editor
	robotEditor *editor = new robotEditor(model);
	ui->layout_robots->addWidget(editor);

	// connect designer elements to slots
	QWidget::connect(ui->pushButton, SIGNAL(clicked()), model, SLOT(addRobot()));
	QWidget::connect(ui->pushButton_2, SIGNAL(clicked()), model, SLOT(addPreconfig()));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), model, SLOT(setUnits(bool)));
	QWidget::connect(ui->si, SIGNAL(toggled(bool)), editor, SLOT(setUnits(bool)));

	// connect robot pieces together
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), editor, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(view, SIGNAL(clicked(const QModelIndex&)), ui->osgWidget, SLOT(setCurrentIndex(const QModelIndex&)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));
	QWidget::connect(editor, SIGNAL(indexChanged(QModelIndex)), ui->osgWidget, SLOT(setCurrentIndex(const QModelIndex&)));

	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), editor, SLOT(dataChanged(QModelIndex, QModelIndex)));
	QWidget::connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ui->osgWidget, SLOT(dataChanged(QModelIndex, QModelIndex)));

	// parsing of xml complete
	//ui->statusBar->showMessage(tr("Loaded %1").arg(fileName), 2000);
}

MainWindow::~MainWindow(void) {
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	std::cerr << "pushbutton clicked" << std::endl;
}

void MainWindow::build_selector(QListWidget *widget, QStringList &names, QStringList &icons) {
	for (int i = 0; i < names.size(); i++) {
		QListWidgetItem *button = new QListWidgetItem(widget);
		button->setIcon(QIcon(icons[i]));
		button->setText(names[i]);
		button->setTextAlignment(Qt::AlignCenter);
		button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
	}
}
