/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENINTRO_H
#define SCREENINTRO_H

#include <QObject>
#include <QWidget>
#include "QLabel"
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

class screenIntro : public QWidget
{
    Q_OBJECT
public:
    explicit screenIntro(QWidget *parent = nullptr);
    QPushButton *diagram;
    int flag;
signals:

public slots:
    void change(bool);
};

#endif // SCREENINTRO_H
