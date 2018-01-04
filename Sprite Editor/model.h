#ifndef MODEL_H
#define MODEL_H

#include <QTableWidget>
#include <QObject>
#include <cstdlib>
#include <QVector>
#include <tuple>
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <iostream>
#include "frame.h"
#include "qgifglobal.h"
#include "qgifimage.h"

const int SPRITE_SIZE = 32;

class Model : public QObject
{
    Q_OBJECT
public:
	int spriteSize = SPRITE_SIZE;
    explicit Model(QObject *parent);

signals:
	void editCompleted(const QString &);
    void getFrame(QVector<Frame> frame);
    void getImages(QVector<QImage> images);
    void colorThisPixel(int,int);
    void eraserTurnOn(bool);
    void dupThis(Frame);

public slots:
    void newFrame(int height, int width);
    void setColor(std::tuple<int, int, int, int> c);
    void saveFrame();
    void updatePreview();
    void exportGif(QString file_name, int rows, int columns);
    void currentFrame(int);
    void resetFrame();
    void updateSpeed(int);
    void duplicate();
    void deleteFrame();
    void paintCommand(int, int);
    void bucketToolOn();
    void drawToolOn();
    void eraserToolOn();

private:
    QVector<Frame> frames;
    int currFrame;
    int speed;
    bool bucket;
    bool draw;
    bool eraser;
    int dupFrame;

    QVector<QImage> framesToImages(int rows, int columns);
    void colorSection(int mult, QImage *image, int row, int column, QRgb value);

    void paintBucket(int, int);
    void setFramePixel(int x, int y);

};

#endif // MODEL_H
