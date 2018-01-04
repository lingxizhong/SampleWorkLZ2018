/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "screenfuture.h"

screenFuture::screenFuture(QWidget *parent) : QWidget(parent)
{
    //Declare the stuff
    QLabel *intro = new QLabel(this);
    QGridLayout* gridLayout = new QGridLayout(this);
    QLabel *info1 = new QLabel(this);
    QFont textFont;
    QFont tabFont;
    gif = new QLabel;
    QPushButton* playBot = new QPushButton(this);
    QPushButton* playGif = new QPushButton(this);

    //Button Basics
    playBot->setText("Hard Coded Bot");
    playGif->setText("Machine Learning Bot");
    playBot->setStyleSheet("background-color: rgb(70,80,80); font-size: 20px");
    playGif->setStyleSheet("background-color: rgb(70,80,80); font-size: 20px");


    //Intro stylesheet and setup
    intro->setStyleSheet("QLabel { background-color : rgb(70,80,80); color : white; border-color: white; border-style: outset; border-width: 2px; border-color: beige}");
    tabFont.setBold(true);
    tabFont.setStyleHint(QFont::Helvetica);
    tabFont.setPixelSize(70);
    intro->setFont(tabFont);
    intro->setText("The Future of Bots");
    intro->resize(1000, 100);

    //Tab body
    textFont.setPixelSize(14);
    QString text = "Almost all game bots up to now have been coded by hand";
    text += "\nThis requires a lot of engineering effort and time to go through"\
            "\nthrough all possible cases and even more, to write a good bot,"\
            "\nyou have to be good at the game yourself. "
            "\nAlso in competitive games," \
            "\nthe bot cannot get any better and can be exploited by users";


    text += "\nRecently, methods for learning bots, " \
            "\nspecifically using reinforcement learning, have become successful"\
            "\nInstead of having to code everything by hand, " \
            "\nthe bot learns from trial and error and get better by itself"\
            "\nand if the game changes or to play a new game,"\
            "\nyou have to write a whole new program.  " \
            "\nIn the case of reinforcement learning, " \
            "\nthe same algorithm can learn"\
            "\nto play many games with only minor modifications";
            "\nand even more, to write a good bot, "\
            "\nyou have to be good at the game yourself." \
            "\nAlso in competitive games, " \
            "\nthe bot cannot get any better and can be exploited by users";

    text += "\nThis slide shows a bot that we coded by hand, " \
            "\nagainst a bot trained with a general algorithm "\
            "\nthat has trained for 10k episodes of the game.";

    info1->setFont(textFont);
    info1->setText(text);
    info1->setStyleSheet("color: white; font-size: 20px; background-color: rgb(70,80,80); padding: 10px");
    //Adding to the layout
    gridLayout->addWidget(intro,   0,0,1,0);
    gridLayout->addWidget(info1,   1,0,1,1);
    gridLayout->addWidget(gif, 1,1,1,1);
    gridLayout->addWidget(playBot, 2,0);
    gridLayout->addWidget(playGif, 2,1);

    connect(playBot, SIGNAL(clicked(bool)), this, SLOT(playBot()));
    connect(playGif, SIGNAL(clicked(bool)), this, SLOT(playGif()));

}

void screenFuture::playBot(){
    //true is bot, false is player
    extern int game(bool);
    QFuture<void> future = QtConcurrent::run(game, true);
}

void screenFuture::playGif(){
    QMovie *movie = new QMovie("../cs3505-f17-a8-edu-app-matwilso/diagrams/trials10000.gif");
    gif->setMovie(movie);
    movie->setSpeed(200);
    movie->start();
}
