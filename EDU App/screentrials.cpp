/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "screentrials.h"

screenTrials::screenTrials(QWidget *parent) : QWidget(parent)
{
    QLabel *intro = new QLabel(this);
  //  QVBoxLayout *boxLayout = new QVBoxLayout;


    QGridLayout* girdLayout = new QGridLayout(this);

    intro->setStyleSheet("QLabel { background-color : rgb(70,80,80); color : white; border-color: white; border-style: outset; border-width: 2px; border-color: beige}");
    QFont tabFont;
    QComboBox *choices = new QComboBox;

    tabFont.setBold(true);
    tabFont.setStyleHint(QFont::Helvetica);
    tabFont.setPixelSize(70);

    intro->setFont(tabFont);
    intro->setText("Trials");
    intro->resize(1000, 100);

    choices->addItem("100 Trials Simulation", QString::number(0));
    choices->addItem("1000 Trials Simulation", QString::number(1));
    choices->addItem("10000 Trials Simulation", QString::number(2));
    choices->addItem("Success Rate Graph", QString::number(3));
    choices->setStyleSheet("font-size: 20px; color: white; background-color: rgb(70,80,80)");

    QFont textFont;
    textFont.setPixelSize(20);
    QLabel *info1 = new QLabel(this);
    info1->setFont(textFont);
    QString text = "In reinforcement learning, the more trials you run,\nthe more information that the model has to predict actions.\n";
    text+= "\nThis means that the more trials you have \nthe more accurate the model can be.";
    text+= "\nRun the simulations below. \n";
    text+= "What do you notice? \n";
    text+= "After making your observations, \n";
    text+= "Look at the Success Rate Graph";
    text+= "\nDoes it match your observations?";


    info1->setText(text);
    info1->setStyleSheet("background-color: rgb(80,90,90); padding: 10px");

    gif = new QLabel;
    //Adding to the layout
    girdLayout->addWidget(intro,   0,0,1,0);
    girdLayout->addWidget(info1,   1,0,1,1);
    girdLayout->addWidget(gif, 1,1,1,1);
    girdLayout->addWidget(choices, 2,0,1,2);
    connect(choices, SIGNAL(activated(int)), this, SLOT(change(int)));
}

void screenTrials::change(int index) {
    if(index == 0) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/trials100.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
        return;
    }
    if (index == 1) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/trials1000.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
        return;
    }
    if (index == 2) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/trials10000.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
        return;
    }
    if(index == 3) {
        QPixmap pixMap("../cs3505-f17-a8-edu-app-matwilso/diagrams/trialsGraph.png");
        gif->setPixmap(pixMap);
    }
}








