/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "screenwelcome.h"
#include "screenintro.h"
#include "screenreward.h"
#include "screenobservation.h"
#include "screentrials.h"
#include "screenlander.h"
#include "screenfuture.h"

#include <QString>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QLabel>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //setup for the mainwindow
    ui->setupUi(this);
    QSize* minSize = new QSize(1280, 720);
    this->setMinimumSize(*minSize);
    this->setMaximumSize(*minSize);
    this->setWindowTitle("ML Moonlander");
    this->setIconSize(QSize(0,0));
    this->centralWidget()->setStyleSheet("background-color: rgb(105, 120, 120);");

    //Program Wide StyleSheet Setup
    qApp->setStyleSheet("QLabel { background-color: rgb(46, 78, 105) }"
                        "QLabel { color: white }"
                        "QLabel { border-color: grey }"
                        "QComboBox { color: grey }"
                        "QPushButton { color: white }"
                        "QTabWidget { background-color: rgb(46, 78, 105) }"
                        "QTabWidget { color: black }"
                        "QTabWidget { border-color: grey }"
                        "QTabWidget { border-bottom: grey }"
                        "QTabWidget::pane { border: 0 }"
                        "QTabBar { color: white }"
                        "QTabBar::tab { background: rgb(70,80,80) }"
                        "QTabBar::tab { color: white }"
                        "QProgressBar { color: white }"
                        "");



    //Layout Setup
    QVBoxLayout *vLayoutMain;
    vLayoutMain = new QVBoxLayout(this->centralWidget());
    one = two = three = four = five = six = false;

    //Tab Setup
    QTabWidget* tab = new QTabWidget(this);
   // tab->setStyleSheet("background-color: grey;");
    ui->centralWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    tab->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    tab->setAutoFillBackground(true);

    //Progres Bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0,6);
    progressBar->setValue(0);


    //Create Screen Widgets
    screenWelcome* welcomeScreen = new screenWelcome(this);
    screenIntro* introScreen = new screenIntro(this);
    screenReward* rewardScreen = new screenReward(this);
    screenObservation* observationScreen = new screenObservation(this);
    screenTrials* trialsScreen = new screenTrials(this);
    screenLander* lunarLanderScreen  = new screenLander(this);
    screenFuture* futureScreen  = new screenFuture(this);


    //Create Tab Names
    QString welcome = "Welcome";
    QString intro = "Machine Learning: An Introduction";
    QString rewardString = "The Reward System";
    QString lunarString = "Lunar Landing Simulation";
    QString observationString = "Observation";
    QString trialsString = "Trials";
    QString futureString = "Past Vs. Future";


    //Add Widgets/.CPP to tab screens
    tab->addTab(welcomeScreen, welcome);
    tab->addTab(introScreen, intro);
    tab->addTab(rewardScreen, rewardString);
    tab->addTab(observationScreen, observationString);
    tab->addTab(trialsScreen, trialsString);
    tab->addTab(futureScreen, futureString);
    tab->addTab(lunarLanderScreen, lunarString);



    //Add to Main Layout
    vLayoutMain->addWidget(tab);
    vLayoutMain->addWidget(progressBar);

    //Signals and Slots
    connect(tab, &QTabWidget::currentChanged, this, &MainWindow::updateValue);
}

void MainWindow::updateValue(int index){

    int val = progressBar->value();
    switch (index) {
    case 1:
        if(!one){
            progressBar->setValue(++val);
            one = true;
        }
        break;
    case 2:
        if(!two){
            progressBar->setValue(++val);
            two = true;
        }
        break;
    case 3:
        if(!three){
            progressBar->setValue(++val);
            three = true;
        }
        break;
    case 4:
        if(!four){
            progressBar->setValue(++val);
            four = true;
        }
        break;
    case 5:
        if(!five){
            progressBar->setValue(++val);
            five = true;
        }
        break;
    case 6:
        if(!six){
            progressBar->setValue(++val);
            six = true;
        }
        break;
    default:
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
//    delete tab;
//    delete welcomescreen;
//    delete introScreen;
//    delete rewardScreen;
//    delete lunarLanderScreen;
}


