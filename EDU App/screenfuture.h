/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENFUTURE_H
#define SCREENFUTURE_H

#include <QObject>
#include <QWidget>
#include <QObject>
#include <QWidget>
#include "QLabel"
#include <QGridLayout>
#include <QPushButton>
#include <QFuture>
#include <QtConcurrent>
#include <QMovie>
class screenFuture : public QWidget
{
    Q_OBJECT
public:
    explicit screenFuture(QWidget *parent = nullptr);
    QLabel *gif;

signals:

public slots:
    void playBot();
    void playGif();


};





#endif // SCREENFUTURE_H
