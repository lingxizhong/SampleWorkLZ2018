/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "screenwelcome.h"
#include "QLabel"
#include <QHBoxLayout>

screenWelcome::screenWelcome(QWidget *parent) : QWidget(parent)
{

    //Delcare the stuff
    QGridLayout *layout = new QGridLayout(this);
    QLabel *label = new QLabel(this);
    QLabel *madeBy = new QLabel(this);
    QFont font;


    //font start point
    font.setBold(true);
    font.setStyleHint(QFont::Helvetica);
    font.setPixelSize(100);

    //Tital Setup
    label->setFont(font);
    label->setText("ML Moonlander");
    label->resize(1000, 100);

    //Names Setup
    font.setPixelSize(20);
    madeBy->setText("Made By \nMatt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson");
    madeBy->setFont(font);




    //add widgets to layout last
    layout->addWidget(label,0,0,Qt::AlignCenter);
    layout->addWidget(madeBy,1,0,Qt::AlignCenter);






}
