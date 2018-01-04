#include "model.h"

using namespace std;

Model::Model(QObject *parent) : QObject (parent)
{

}

/*
 * Used when new frame is made (button click, new file, open file)
 */
void Model::newFrame(int height, int width)
{
    Frame temp(height, width);
    frames.append(temp);
    dupFrame = currFrame;
    currFrame = frames.size() - 1;
    if (frames.size() >= 2)
    {
        frames[currFrame].setColor(frames[currFrame-1].getColor());
    }
}

/*
 * Used to remove frame when delete frame is clicked
 */
void Model::deleteFrame()
{
    if (frames.size() > 1)
    {
        frames.removeAt(currFrame);
    }
}

/*
 * Used when save is clicked
 */
void Model::saveFrame()
{
    emit getFrame(frames);
}

/*
 * Used to clear and reset frames when new file or open file is done
 */
void Model::resetFrame()
{
    frames.clear();
    draw = true;
    bucket = false;
    eraser = false;
    currFrame = 0;
    speed = 0;
}

/*
 * Exports .gif image with speed of preview
 */
void Model::exportGif(QString file_name, int rows, int columns)
{
    int tempSpeed;
    if(speed == 0)
    {
        tempSpeed = 1000;
    }
    else
        tempSpeed = speed;
    QGifImage file(file_name);
    QVector<QImage> images = framesToImages(rows, columns);
    for(int i = 0; i < images.length(); i++)
    {
        file.addFrame(images[i], tempSpeed);
    }
    file.save(file_name);
}

/*
 * Sets current frame index
 */
void Model::currentFrame(int x)
{
    currFrame = x;
    dupFrame = currFrame;
}

/*
 * Duplicates frame in vector
 */
void Model::duplicate()
{
    if (frames.size() > 0)
    {
        frames[currFrame] = frames[dupFrame];
        emit dupThis(frames[dupFrame]);
    }
}

/*
 * Converts frames to images when exporting gif and for preview.
 */
QVector<QImage> Model::framesToImages(int rows, int columns)
{
    QVector<QImage> images;
    int i, j, k;
    int mult = 1;
    for(i = 0; i < frames.length(); i++)
    {
        QImage temp(QSize(columns*mult, rows*mult), QImage::Format_ARGB32);
        for(k = 0; k < columns; k++)
        {
            for(j = 0; j < rows; j++)
            {
                tuple<int, int, int, int> values;
                values = frames[i].pixels[j][k];
                QRgb value = qRgb(std::get<0>(values), std::get<1>(values), std::get<2>(values));
                colorSection(mult, &temp, k, j, value);
            }
        }
        images.append(temp);
    }
    return images;
}

/*
 * Used for paint bucket to color area
 */
void Model::colorSection(int mult, QImage *image, int row, int column, QRgb value)
{
    int finalX = row*mult + mult;
    int finalY = column*mult + mult;
    for(int placeX = row*mult; placeX < finalX; placeX++)
    {
        for(int placeY = column*mult; placeY < finalY; placeY++)
        {
            image->setPixel(placeX, placeY, value);
        }
    }
}

/*
 * Check if on bucket or on draw
 */
void Model::paintCommand(int x, int y) {
    if (bucket == true)
    {
        paintBucket(x, y);
    }
    else
    {
        setFramePixel(x, y);
    }
}

/*
 * Sets the pixel in the model class
 */
void Model::setFramePixel(int x, int y)
{
    if (eraser == true)
    {
        frames[currFrame].erasePixel(x, y);
        emit eraserTurnOn(true);
    }
    else
    {
        frames[currFrame].setPixel(x, y);
        emit eraserTurnOn(false);
    }
    emit colorThisPixel(x, y);
}

/*
 * Sets color of current frame
 */
void Model::setColor(tuple<int, int, int, int> c)
{
    frames[currFrame].setColor(c);
}

/*
 * Sends images to preview
 */
void Model::updatePreview()
{
    if(frames.length()>0)
        emit getImages(framesToImages(std::get<0>(frames[0].getSize()), std::get<1>(frames[0].getSize())));
}

/*
 * Updates speed for .gif
 */
void Model::updateSpeed(int pace)
{
    speed = pace;
}

/*
 * Recursive method in order for the bucket tool to work
 */
void Model::paintBucket(int x, int y) {
    tuple<int, int, int, int> currentPixel = frames[currFrame].getPixel(x, y);
    tuple<int, int, int, int> currentColor = frames[currFrame].getColor();

    if(currentPixel == currentColor)
        return;

    setFramePixel(x, y);

    int width = frames[currFrame].column;
    int height = frames[currFrame].row;

    int x1 = x - 1;
    int x2 = x + 1;
    int y1 = y - 1;
    int y2 = y + 1;

    if (x1 >= 0 && frames[currFrame].getPixel(x1, y) == currentPixel)
    {
        QtConcurrent::run(this, &Model::paintBucket, x1, y);
    }
    if (x2 < width && frames[currFrame].getPixel(x2, y) == currentPixel)
    {
        QtConcurrent::run(this, &Model::paintBucket, x2, y);
    }
    if (y1 >= 0 && frames[currFrame].getPixel(x, y1) == currentPixel)
    {
        QtConcurrent::run(this, &Model::paintBucket, x, y1);
    }
    if (y2 < height && frames[currFrame].getPixel(x, y2) == currentPixel)
    {
        QtConcurrent::run(this, &Model::paintBucket, x, y2);
    }

}

/*
 * Turns on the bucket tool
 */
void Model::bucketToolOn()
{
    bucket = true;
    draw = false;
    eraser = false;
}

/*
 * Turns on the draw tool
 */
void Model::drawToolOn()
{
    draw = true;
    bucket = false;
    eraser = false;
}

/*
 * Turns on the eraser tool
 */
void Model::eraserToolOn()
{
    draw = false;
    bucket = false;
    eraser = true;
}


