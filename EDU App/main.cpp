/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "mainwindow.h"
#include <QApplication>
#include "game.cpp"
#include <QThread>
#include <QFuture>
#include <QtConcurrent>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    //Comment these out to stop the game
    //extern int game();
    //QFuture<void> future = QtConcurrent::run(game);


    return a.exec();
}
