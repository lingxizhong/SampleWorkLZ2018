#ifndef SPRITEVIEW_H
#define SPRITEVIEW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPalette>
#include <QColor>
#include <QColorDialog>
#include <QtDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QStackedLayout>
#include <tuple>
#include <iostream>
#include "model.h"
#include "frame.h"
#include "form.h"

const QString COLOR_STYLE("QPushButton { background-color : %1; }");


namespace Ui {
class SpriteView;
}

class SpriteView : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpriteView(Model& model, QWidget *parent = 0);
    ~SpriteView();

signals:
    void frameCreated(Frame frame);
    void createFrame(int height, int width);
    void pixelColor(std::tuple<int,int,int,int> c);
    void exportGifSig(QString file_name, int rows, int columns);
    void loadColor(int,int);
    void updateSpeed(int speed);
    void currentFrame(int);

private slots:
    void on_colorButton_clicked();
    void colorCell(int row, int column);
    void eraserOn(bool);
    void newFile();
    void saveFile(QVector<Frame>);
    void openFile();
    void initNewFrame();
    void deleteCurrFrame();
    void exportGifWindow();
    void onFrameSelected(QTableWidgetItem *item);
    void previewAnim();
    void updatePrevImages(QVector<QImage> images);
    void changeFPS(int);
    void showOnionSkins();
    //void on_drawButton_clicked();
    void duplicateFrame(Frame f);

private:
    Ui::SpriteView *ui;
	// the widget for the currently active frame
	QTableWidget* currentTableWidget; 
	int tableSize;
	QColor activeColor;
	QColor blankColor;
    int frameCount;
    QVector<Frame> frames;
    Form *popup;
    int rows_;
    int columns_;
    bool eraser;
    QTimer *timer;
    int currentPrev;
    QVector<QImage> prevImages;
    QStackedLayout *stackedLayout;
    QTableWidget *onionTables[3];
    QResizeEvent *triggerResizer;

    void initMainDrawBoxItems(int, int);
    void initFrameItem(QTableWidget *newFrame);
    void initPreview();
    void initOnionTables();
    void setActiveColor(QColor color);
    void copyQTableWidgetContents(QTableWidget* from, QTableWidget* to);
    void initStartFrame();
    QColor getActiveColor();
    int getCurrentFrameIndex();
    void cleanUp();
    void hideOnionSkins();
    void resizeEvent(QResizeEvent* event);
    void triggerResizeEvent();
};

#endif // SPRITEVIEW_H
