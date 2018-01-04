/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENTRIALS_H
#define SCREENTRIALS_H

#include <QObject>
#include <QWidget>
#include "QLabel"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QComboBox>
#include <QMovie>



class screenTrials : public QWidget
{
    Q_OBJECT
public:
    explicit screenTrials(QWidget *parent = nullptr);

private:
    QLabel *gif;

signals:

public slots: 
    void change(int);
};

#endif // SCREENTRIALS_H
