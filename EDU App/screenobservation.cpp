/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "screenobservation.h"

screenObservation::screenObservation(QWidget *parent) : QWidget(parent)
{
    // Declaring my objects
    QLabel *intro = new QLabel(this);
    gif = new QLabel;
    QGridLayout* girdLayout = new QGridLayout(this);
    intro->setStyleSheet("QLabel { background-color : rgb(70,80,80); color : white; border-color: white; border-style: outset; border-width: 2px; border-color: beige}");
    QFont tabFont;
    QComboBox *choices = new QComboBox;

    //Assigning properties to the fonts
    tabFont.setBold(true);
    tabFont.setStyleHint(QFont::Helvetica);
    tabFont.setPixelSize(70);

    //Setting the intro
    intro->setFont(tabFont);
    intro->setText("Observations");
    intro->resize(1000, 100);

    //Making the combo box
    choices->setStyleSheet("font-size: 20px; color: white; background-color: rgb(70,80,80)");
    choices->addItem("Not knowing X Position", QString::number(0));
    choices->addItem("Not knowing Y Postiion", QString::number(1));
    choices->addItem("Not knowing Angle", QString::number(2));
    choices->addItem("Observation Success Rate Graph", QString::number(3));

    //Hard coding the text info
    QFont textFont;
    textFont.setPixelSize(20);
    QLabel *info1 = new QLabel(this);
    QString text = "In Reinforcement learning, the model \n";
    text+= "needs to be able to observe the evironment";
    text+= "\nThe more it can observe, the better";
    text+= "\nWe can see what happens when we blind";
    text+= "\nthe model in certain ways by running ";
    text+= "\nthe simulations below.";
    text+= "\nIn the first one, the bot does not know its x position";
    text+= "\nIn the second one, the bot does not know its height";
    text+= "\nIn the last one, the bot does not know its angle";

    //Setting the text
    info1->setFont(textFont);
    info1->setText(text);
    info1->setStyleSheet("background-color: rgb(80,90,90); padding: 10px");

    //Adding to the layout
    girdLayout->addWidget(intro,   0,0,1,0);
    girdLayout->addWidget(info1,   1,0,1,1);
    girdLayout->addWidget(gif, 1,1,1,1);
    girdLayout->addWidget(choices, 2,0,1,2);

    //Connecting the signal and the slot
    connect(choices, SIGNAL(activated(int)), this, SLOT(change(int)));
}

// The slot for making the combo box change what is displayed
void screenObservation::change(int index) {
    if (index == 0) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/obsX.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
    }
    if (index == 1) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/obsY.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
    }
    if (index == 2) {
        QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/obsANGLE-2.gif");
        gif->setMovie(movie);
        movie->setSpeed(200);
        movie->start();
    }
    if (index == 3) {
        QPixmap pixMap("../cs3505-f17-a8-edu-app-matwilso/diagrams/observation.png");
        gif->setPixmap(pixMap);
    }

}





