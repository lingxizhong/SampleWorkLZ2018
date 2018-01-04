/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "screenreward.h"

screenReward::screenReward(QWidget *parent) : QWidget(parent)
{
    //Declarring objects
    layout = new QVBoxLayout(this);
    headerText = new QLabel(this);
    markovText = new QLabel(this);
    rewardText = new QLabel(this);

    //Making the font
    font.setBold(true);
    font.setStyleHint(QFont::Helvetica);
    font.setPixelSize(70);

    //Setting header text
    headerText->setFont(font);
    headerText->setText("Rewarding the AI");
    headerText->resize(1000, 100);
    headerText->setStyleSheet("QLabel { background-color : rgb(70,80,80); color : white; border-color: white; border-style: outset; border-width: 2px; border-color: beige}");

    //Changing the font for the rest of the page text
    font.setBold(false);
    font.setPixelSize(20);

    // Hardcoding the text
    markovText->setFont(font);
    markovText->setText("Reinforcement learning uses something called the Markov Decision Process.\n"
                        "The Markov Decision Process is where you have a series of states that the \n"
                        "Environment and the agent are in, all of the available actions that can be taken, \n"
                        "and you have a function called the model that computes the probability of ending\n"
                        "at a select state. Then a reward is given once the agent has taken an action.");
    markovText->setStyleSheet("background-color: rgb(80,90,90); padding: 10px");

    rewardText->setFont(font);
    rewardText->setText("The agent chooses the action that gives it the most reward. The tricky part \n"
                        "about it is getting the agent to look at long term rewards to best optimize its \n"
                        "solution");
    rewardText->setStyleSheet("background-color: rgb(80,90,90); padding: 10px");

    //Code for the diagram
    diagram = new QPushButton;
    QPixmap pixReward("../cs3505-f17-a8-edu-app-matwilso/diagrams/reward.png");
    QIcon buttonIcon(pixReward);
    diagram->setIcon(buttonIcon);
    diagram->setIconSize(QSize(600, 500));
    diagram->setStyleSheet("background-color: rgb(150,160,160)");
    flag = 0;

    // Adding objects to the layout
    layout->addWidget(headerText);
    layout->addWidget(markovText);
    layout->addWidget(diagram);
    layout->addWidget(rewardText);
    setLayout(layout);

    // Connect for the diagrams
    connect(diagram, SIGNAL(clicked(bool)), this, SLOT(change(bool)));
}


// Slot for changing the diagram
void screenReward::change(bool) {
    if(flag == 0) {
        QPixmap pixDiagram2("../cs3505-f17-a8-edu-app-matwilso/diagrams/markov.png");
        QIcon ButtonIcon2(pixDiagram2);
        diagram->setIcon(ButtonIcon2);
        diagram->setIconSize(QSize(600, 500));
        flag = 1;
    }
    else {
        QPixmap pixDiagram1("../cs3505-f17-a8-edu-app-matwilso/diagrams/reward.png");
        QIcon ButtonIcon1(pixDiagram1);
        diagram->setIcon(ButtonIcon1);
        diagram->setIconSize(QSize(600, 500));
        flag = 0;
    }
}

screenReward::~screenReward(){
    delete headerText;
    delete markovText;
    delete rewardImage;
    delete markovImage;
}
