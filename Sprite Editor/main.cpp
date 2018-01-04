#include "spriteview.h"
#include "model.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	// we can add this back. it was just failing to compile
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Model m(0);
    SpriteView w(m);
    w.show();

    return a.exec();
}
