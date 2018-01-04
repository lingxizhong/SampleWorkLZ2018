#include "spriteview.h"
#include "ui_spriteview.h"
#include <QShortcut>

SpriteView::SpriteView(Model& model, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SpriteView)
{
    ui->setupUi(this);

	// Color for blank background
	QColor blankColor("white");
	// Initialize color picker button
    QColor startColor("black");
    ui->colorButton->setStyleSheet(COLOR_STYLE.arg(startColor.name()));
    activeColor = startColor;

    rows_ = 8;
    columns_ = 8;
    frameCount = 0;
    currentPrev= 0;
    *onionTables = {NULL};
    for(int i = 0; i < 3; i++)
    {
        onionTables[i] = NULL;
    }
    // Short Cuts
    QShortcut *save = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(save, SIGNAL(activated()), &model, SLOT(saveFrame()));

    QShortcut *open = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(open, SIGNAL(activated()), &model, SLOT(resetFrame()));
    connect(open, SIGNAL(activated()), this, SLOT(openFile()));

    QShortcut *newfile = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(newfile, SIGNAL(activated()), &model, SLOT(resetFrame()));
    connect(newfile, SIGNAL(activated()), this, SLOT(newFile()));

    QShortcut *draw = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(draw, SIGNAL(activated()), &model, SLOT(drawToolOn()));

    QShortcut *bucket = new QShortcut(QKeySequence("Ctrl+B"), this);
    connect(bucket, SIGNAL(activated()), &model, SLOT(bucketToolOn()));

    QShortcut *eraser = new QShortcut(QKeySequence("Ctrl+E"), this);
    connect(eraser, SIGNAL(activated()), &model, SLOT(eraserToolOn()));

    QShortcut *frame = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(frame, SIGNAL(activated()), this, SLOT(initNewFrame()));

    QShortcut *onion = new QShortcut(QKeySequence("Ctrl+T"), this);
    connect(onion, SIGNAL(activated()), this, SLOT(showOnionSkins()));

    QShortcut *color = new QShortcut(QKeySequence("Ctrl+C"), this);
    connect(color, SIGNAL(activated()), this, SLOT(on_colorButton_clicked()));

    QShortcut *duplicate = new QShortcut(QKeySequence("Ctrl+V"),this);
    connect(duplicate, SIGNAL(activated()), this, SLOT(initNewFrame()));
    connect(duplicate, SIGNAL(activated()), &model, SLOT(duplicate()));
    connect(duplicate, SIGNAL(activated()), &model, SLOT(updatePreview()));

    QShortcut *deleteFrame = new QShortcut(QKeySequence("Ctrl+W"),this);
    connect(deleteFrame, SIGNAL(activated()), &model, SLOT(deleteFrame()));
    connect(deleteFrame, SIGNAL(activated()), this, SLOT(deleteCurrFrame()));
    connect(deleteFrame, SIGNAL(activated()), &model, SLOT(updatePreview()));

    // Frames
    connect(ui->addFrameButton, SIGNAL(clicked(bool)), this, SLOT(initNewFrame()));

    connect(ui->deleteButton, SIGNAL(clicked(bool)), &model, SLOT(deleteFrame()));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteCurrFrame()));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), &model, SLOT(updatePreview()));

    connect(ui->duplicateButton, SIGNAL(clicked(bool)), this, SLOT(initNewFrame()));
    connect(ui->duplicateButton, SIGNAL(clicked(bool)), &model, SLOT(duplicate()));
    connect(ui->duplicateButton, SIGNAL(clicked(bool)), &model, SLOT(updatePreview()));
    connect(&model, SIGNAL(dupThis(Frame)), this, SLOT(duplicateFrame(Frame)));

    // File Menu
    connect(ui->actionSave_File, SIGNAL(triggered(bool)), &model, SLOT(saveFrame()));
    connect(&model, SIGNAL(getFrame(QVector<Frame>)), this, SLOT(saveFile(QVector<Frame>)));

    connect(ui->actionOpen_File, SIGNAL(triggered(bool)), &model, SLOT(resetFrame()));
    connect(ui->actionOpen_File, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(this, SIGNAL(loadColor(int,int)), &model, SLOT(paintCommand(int,int)));

    connect(ui->actionNew_File, SIGNAL(triggered(bool)), &model, SLOT(resetFrame()));
    connect(ui->actionNew_File, SIGNAL(triggered(bool)), this, SLOT(newFile()));

    connect(ui->actionExport_GIF, SIGNAL(triggered(bool)), this, SLOT(exportGifWindow()));
    connect(this, SIGNAL(exportGifSig(QString, int, int)), &model, SLOT(exportGif(QString, int, int)));
    connect(this, SIGNAL(updateSpeed(int)), &model, SLOT(updateSpeed(int)));

    // Button
    connect(ui->drawButton, SIGNAL(clicked(bool)), &model, SLOT(drawToolOn()));
    connect(ui->eraseButton, SIGNAL(clicked(bool)), &model, SLOT(eraserToolOn()));
    connect(&model, SIGNAL(eraserTurnOn(bool)), this, SLOT(eraserOn(bool)));
    connect(ui->bucketButton, SIGNAL(clicked(bool)), &model, SLOT(bucketToolOn()));

    // Create Frame in Model
    connect(this, SIGNAL(createFrame(int,int)), &model, SLOT(newFrame(int,int)));
    connect(this, SIGNAL(currentFrame(int)), &model, SLOT(currentFrame(int)));
    connect(this, SIGNAL(pixelColor(std::tuple<int,int,int,int>)), &model, SLOT(setColor(std::tuple<int,int,int,int>)));
    connect(ui->tableWidget, SIGNAL(cellPressed(int,int)), &model, SLOT(paintCommand(int,int)));
    connect(ui->tableWidget, SIGNAL(cellEntered(int,int)), &model, SLOT(paintCommand(int,int)));
    connect(&model, SIGNAL(colorThisPixel(int,int)), this, SLOT(colorCell(int,int)));

    // Preview Animation
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(previewAnim()));
    connect(ui->tableWidget, SIGNAL(cellEntered(int,int)), &model, SLOT(updatePreview()));
    connect(this, SIGNAL(createFrame(int,int)), &model, SLOT(updatePreview()));
    connect(ui->fpsSlider, SIGNAL(valueChanged(int)), &model, SLOT(updatePreview()));
    connect(&model, SIGNAL(getImages(QVector<QImage>)), this, SLOT(updatePrevImages(QVector<QImage>)));
    connect(ui->fpsSlider, SIGNAL(valueChanged(int)), this, SLOT(changeFPS(int)));

    //Onion Skinning
    connect(ui->onionButton, SIGNAL(clicked(bool)), this, SLOT(showOnionSkins()));
}

/*
 * Allows the user to save their sprite sheet.
 */
void SpriteView::saveFile(QVector<Frame> f)
{
    QString file_name = QFileDialog::getSaveFileName(this,
                                                tr("Save Sprite Sheet"), "",
                                                tr("Sprite Sheet (*.ssp);;All Files (*)"));

    Frame frame;
    frame = Frame::fromTableWidget(ui->tableWidget);
    QString ASCII_text;

    if (file_name.isEmpty())
        return;
    else
    {
        QFile file(file_name);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }

        // Write down in ASCII Text
        // Writing down Height and Width
        ASCII_text += QString::number(rows_) + " " + QString::number(columns_) + QString("\n");
        // Writing down Number of Frames
        ASCII_text += QString::number(frameCount) + QString("\n");
        // Writing down Pixels
        foreach (Frame p, f)
        {
            ASCII_text += p.toString();
        }

        QTextStream stream(&file);
        stream << ASCII_text; // to save data to file
    }
}

/*
 * Allows the user to open a saved .ssp file
 */
void SpriteView::openFile()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                tr("Open Sprite Sheet"), "",
                                                tr("Sprite Sheet (*.ssp);;All Files (*)"));
    if (file_name.isEmpty())
        return;
    else
    {
        QFile file(file_name);
        // Check if you can open file
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }

        // Start reading information from file.
        QTextStream in(&file);

        QStringList heightAndWidth = in.readLine().split(" ");
        rows_ = heightAndWidth[0].toInt();
        columns_ = heightAndWidth[1].toInt();

        if(onionTables[0] != NULL)
            cleanUp();
        initStartFrame();

        QString frame = in.readLine();

        int numOfFrames = frame.toInt();
        int currentFrame = 0;
        // Looping to draw each frame
        while (currentFrame < numOfFrames)
        {
            int y = 0;
            // Looping to draw all of the pixels
            while (y < rows_)
            {
                int x = 0;
                QString current_line = in.readLine();
                QStringList colors = current_line.split(" ");

                // To set each pixel with the correct color
                for (int i = 0; i <= colors.size() - 4; i += 4)
                {
                    setActiveColor(QColor(colors[i].toInt(),colors[i+1].toInt(),colors[i+2].toInt(),colors[i+3].toInt()));
                    colorCell(y, x);
                    emit loadColor(y, x);
                    x++;
                }
                y++;
            }
            // Check to see if there are more frames before adding another frame
            if (!in.atEnd())
            {
                // Add Frame here
                initNewFrame();
            }
            ui->previewLabel->clear();
            currentFrame++;
            triggerResizeEvent();
        }
        // Reset activeColor
        setActiveColor(QColor(0, 0, 0, 255));
    }
}

/*
 * New File button is clicked on
 */
void SpriteView::newFile()
{
    Form popup;
    if(popup.exec() == QDialog::Accepted)
    {
        rows_ = popup.getHeight();
        columns_ = popup.getWidth();
        if(onionTables[0] != NULL)
            cleanUp();
        initStartFrame();
        ui->previewLabel->clear();
        // Makes sure the color is saved for each new file
        std::tuple<int,int,int,int> currentColor(activeColor.red(), activeColor.green(), activeColor.blue(), activeColor.alpha());
        emit pixelColor(currentColor);
    }
}

/*
 * When the user calls this it will copy their current selected frame to a new one.
 */
void SpriteView::duplicateFrame(Frame f)
{
    QColor prevColor = activeColor;
    bool eraserWasOn = eraser;
    for(int y = 0; y < f.row; y++)
    {
        for(int x = 0; x < f.column; x++)
        {
            if(eraser)
                eraser = false;
            std::tuple<int, int, int, int> color = f.getPixel(x, y);
            QColor frameColor(std::get<0>(color), std::get<1>(color), std::get<2>(color), std::get<3>(color));
            setActiveColor(frameColor);
            colorCell(x, y);
        }
    }
    if(eraserWasOn)
        eraser = true;
    setActiveColor(prevColor);
}

/*
 * Helper method in order to load or make a new file
 */
void SpriteView::initStartFrame()
{
    // Reset UI
    frameCount = 0;
    currentPrev = 0;
    ui->framesTable->setRowCount(0);
    ui->framesTable->setColumnCount(0);
    ui->addFrameButton->setEnabled(true);
    ui->duplicateButton->setEnabled(true);
    ui->colorButton->setEnabled(true);
    ui->deleteButton->setEnabled(false);
    ui->fpsSlider->setEnabled(true);
    ui->previewLabel->clear();
    ui->fpsSlider->setValue(0);
    prevImages.clear();
    ui->onionButton->setEnabled(true);
    initMainDrawBoxItems(rows_, columns_);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    initNewFrame();
    initPreview();
    initOnionTables();
    triggerResizeEvent();
    timer->stop();
}

/*
 * Allows user to export the preview into a gif
 */
void SpriteView::exportGifWindow()
{
    QString file_name = QFileDialog::getSaveFileName(this,
                                                tr("Export GIF"), "",
                                                tr("GIF (*.gif)"));
    if (file_name.isEmpty())
        return;
    else
    {
        emit exportGifSig(file_name, rows_, columns_);
    }
}

/*
 * Initialize the items in the main draw box so that we can
 * change the color of them
 */
void SpriteView::initMainDrawBoxItems(int row, int column)
{
    ui->tableWidget->setRowCount(row);
    ui->tableWidget->setColumnCount(column);
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < column; c++)
        {
            ui->tableWidget->setItem(r, c, new QTableWidgetItem);
		}
    }
}

/*
 * Creates a new frame and adds it to the frame viewer on the side
 */
void SpriteView::initNewFrame()
{
    if(ui->addFrameButton->isEnabled() || ui->duplicateButton->isEnabled())
    {
        hideOnionSkins();
        QTableWidgetItem *newRow = new QTableWidgetItem();
        ui->framesTable->setRowCount(frameCount+1);
        ui->framesTable->setColumnCount(1);
        ui->framesTable->setItem(frameCount+1, 1, newRow);
        ui->framesTable->verticalHeader()->resizeSection(frameCount, 135);
        ui->framesTable->horizontalHeader()->resizeSection(frameCount, 220);
        ui->framesTable->horizontalHeader()->setVisible(false);

        QTableWidget *newFrame = new QTableWidget();
        newFrame->setObjectName("tableWidget"+QString::number(frameCount));

        initFrameItem(newFrame);
        // clear the current drawing frame
        initMainDrawBoxItems(rows_, columns_);

        ui->framesTable->setCellWidget(frameCount, 0, newFrame);
        frameCount++;
        if(frameCount > 1)
        {
            ui->deleteButton->setEnabled(true);
        }
        emit frameCreated(Frame::fromTableWidget(ui->tableWidget));

        // Set this to be the current frame(/TableWidget)
        currentTableWidget = newFrame;
        emit createFrame(rows_, columns_);
    }
}

/*
 * Deletes the Frame that the user is currently using
 */
void SpriteView::deleteCurrFrame()
{
    if(ui->deleteButton->isEnabled())
    {
        currentPrev = 0;
        int newIndex = getCurrentFrameIndex();
        ui->framesTable->removeRow(getCurrentFrameIndex());
        frameCount--;
        if(frameCount == 1)
        {
            ui->deleteButton->setEnabled(false);
        }
        QTableWidget* littleTable;
        if(newIndex != frameCount)
        {
            littleTable = (QTableWidget*)ui->framesTable->cellWidget(newIndex, 0);
        }
        else
        {
            littleTable = (QTableWidget*)ui->framesTable->cellWidget(newIndex - 1, 0);
        }
        // Copy the contents of this frame to the main draw box
        copyQTableWidgetContents(littleTable, ui->tableWidget);
        currentTableWidget = littleTable;
        hideOnionSkins();
        emit currentFrame(getCurrentFrameIndex());
    }
}

/*
 * Initialize a new frame in the side bar
 */ 
void SpriteView::initFrameItem(QTableWidget *newFrame)
{
    int itemSizeD = rows_;
    if(rows_ < columns_){itemSizeD = columns_;}
    int pixSize = ceil(125/itemSizeD);
    if(pixSize<=1)
        pixSize=1;
    //initialize items in table
    newFrame->verticalHeader()->setVisible(false);
    newFrame->horizontalHeader()->setVisible(false);
    newFrame->setRowCount(rows_);
    newFrame->setColumnCount(columns_);
    newFrame->verticalHeader()->setDefaultSectionSize(pixSize);
    newFrame->horizontalHeader()->setDefaultSectionSize(pixSize);
    newFrame->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    newFrame->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    //disable selecting and editing cells
    newFrame->setEditTriggers(QAbstractItemView::NoEditTriggers);
    newFrame->setFocusPolicy(Qt::NoFocus);
    newFrame->setSelectionMode(QAbstractItemView::NoSelection);

    // Connect this itemClicked signal to trigger the onFrameSelected
	// (used for later clicking to set this to the current frame) 
    connect(newFrame, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onFrameSelected(QTableWidgetItem*)));

    for (int r = 0; r < rows_; r++)
    {
        for (int c = 0; c < columns_; c++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setBackground(QColor(0,0,0,0));
            newFrame->setItem(r, c, newItem);
        }
    }
}

/*
 * Initializes the animation preview table
 */
void SpriteView::initPreview()
{
    ui->previewLabel->setFixedWidth(250);
    ui->previewLabel->setFixedHeight(250);
}
/*
 * Copy all of the colors in one QTableWidget to another
 * (this is used for copying from side panel frame to main draw window)
 */ 
void SpriteView::copyQTableWidgetContents(QTableWidget* from, QTableWidget* to)
{
	// throw an error if they are not the same size
    if (from->rowCount() != to->rowCount() || from->columnCount() != to->columnCount())
    {
		throw;
	}

	int rowCount = from->rowCount();
	int columnCount = from->columnCount();
    for (int r = 0; r < rowCount; r++)
    {
        for (int c = 0; c < columnCount; c++)
        {
			QColor fromColor = from->item(r, c)->background().color();
			to->item(r, c)->setBackground(fromColor);
		}
    }
}

/*
 * Change the active pen color and the color displayed in the box
 */
void SpriteView::setActiveColor(QColor color) 
{
	activeColor = color;
	ui->colorButton->setStyleSheet(COLOR_STYLE.arg(activeColor.name()));
	ui->colorButton->setAutoFillBackground(true);
	ui->colorButton->setFlat(true);
    std::tuple<int,int,int,int> currentColor(activeColor.red(), activeColor.green(), activeColor.blue(), activeColor.alpha());
    emit pixelColor(currentColor);
}

/*
 * User clicked to change color of pen
 */
void SpriteView::on_colorButton_clicked()
{
	QColor chosenColor = QColorDialog::getColor();
	// if user didn't cancel the dialog, set the color to the one chosen 
    if (chosenColor.isValid())
    {
        setActiveColor(chosenColor);
	}
}

/*
 * User is clicking and dragging in the drawing box
 *
 * so color both the QWidgetTable for the main drawing box and the current frame
 */
void SpriteView::colorCell(int row, int column)
{
    if (eraser == true)
    {
        QColor eraserColor(255, 255, 255, 0);
        ui->tableWidget->item(row, column)->setBackground(eraserColor);
        currentTableWidget->item(row,column)->setBackground(eraserColor);
    }
    else
    {
        // change the color of the currently displayed drawing
        ui->tableWidget->item(row, column)->setBackground(activeColor);
        // also change the color of the current selected frame
        currentTableWidget->item(row,column)->setBackground(activeColor);
    }
}

/*
 * Enables the eraser w/o changing the current active color
 */
void SpriteView::eraserOn(bool on)
{
    eraser = on;
}

/*
 * Called when any of the cells in the frame preview is clicked
 *
 * (note: to change the current frame, the user has to click in the cell area
 * of the frame preview)
 */
void SpriteView::onFrameSelected(QTableWidgetItem *item)
{
	// Grab the parent QTableWidget* of the cell that was clicked on 
	QTableWidget* parent = item->tableWidget(); 
	// Copy the contents of this frame to the main draw box 
	copyQTableWidgetContents(parent, ui->tableWidget); 
	currentTableWidget = parent;
    hideOnionSkins();
    emit currentFrame(getCurrentFrameIndex());
}

/*
 * Called according to the timer and fps
 * Copies frames and plays animation
 */
void SpriteView::previewAnim()
{
    int ratioNum;
    QImage temp;

    if(rows_<columns_)
    {
        ratioNum = ceil((double)250/columns_);
        temp = prevImages[currentPrev].scaled(250, rows_*ratioNum);
        ui->previewLabel->setPixmap(QPixmap::fromImage(temp));

    }
    else if(columns_<rows_)
    {
        ratioNum = ceil((double)250/rows_);
        temp = prevImages[currentPrev].scaled(columns_*ratioNum,250);
        ui->previewLabel->setPixmap(QPixmap::fromImage(temp));
    }
    else
    {
        temp = prevImages[currentPrev].scaled(250,250);
        ui->previewLabel->setPixmap(QPixmap::fromImage(temp));
    }
    ui->previewLabel->show();
    if(currentPrev < frameCount-1)
        currentPrev++;
    else
        currentPrev = 0;
}

/*
 * Updates images for preview animation when frame is changed
 */
void SpriteView::updatePrevImages(QVector<QImage> images)
{
    prevImages = images;
}
/*
 * Updates the fps when fps slider is changed
 */
void SpriteView::changeFPS(int fps)
{
    int speed;
    if(fps!=0)
    {
        speed = 1000/fps;
        timer->start(speed);
    }
    else
    {
        timer->stop();
        speed = 0;
    }
    emit updateSpeed(speed);
}

/*
 * Shows/hides onion skin when onion skin button is clicked
 */
void SpriteView::showOnionSkins()
{
    if(onionTables[0]!=NULL)
    {
        int currentFrameI = getCurrentFrameIndex();
        if(onionTables[0]->isVisible())
            hideOnionSkins();
        else
        {
            if(currentFrameI != 0)
            {
                int j = 0;
                int i = currentFrameI-1;
                while( i >= 0 && i != currentFrameI-4)//for(int i= currentFrameI-1; 0 <= i; i--)
                {
                    QTableWidget *temp = (QTableWidget*)ui->framesTable->cellWidget(i, 0);
                    for (int r = 0; r < rows_; r++) {
                        for (int c = 0; c < columns_; c++) {
                            QColor fromColor =temp->item(r,c)->background().color();
                            if(fromColor.alpha()!=0)
                                fromColor.setAlpha(50);
                            onionTables[j]->item(r, c)->setBackground(fromColor);
                        }
                    }
                    onionTables[j]->show();
                    j++;
                    i--;
                }
            }
        }
    }
}

/*
 * Used when new frame is added, new frame is clicked on, new file, open file.
 */
void SpriteView::hideOnionSkins()
{
    for(int i = 0; i < 3; i++)
    {
        if(onionTables[i] != NULL)
        {
            if(onionTables[i]->isVisible())
                onionTables[i]->hide();
        }
    }
}
/*
 * Initialize onion tables
 */
void SpriteView::initOnionTables()
{
    stackedLayout = new QStackedLayout;
    ui->horizontalLayout->addLayout(stackedLayout,0);
    stackedLayout->addWidget(ui->tableWidget);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    for(int i = 0; i < 3; i++)
    {
        onionTables[i] = new QTableWidget();
        onionTables[i]->setRowCount(rows_);
        onionTables[i]->setColumnCount(columns_);
        onionTables[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        onionTables[i]->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        onionTables[i]->verticalHeader()->setVisible(false);
        onionTables[i]->horizontalHeader()->setVisible(false);
        onionTables[i]->setObjectName("onionTable"+QString::number(i));
        onionTables[i]->setStyleSheet("background-color:rgba(0,0,0,0)");
        onionTables[i]->hide();
        onionTables[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        onionTables[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        stackedLayout->addWidget(onionTables[i]);
        for (int r = 0; r < rows_; r++)
        {
            for (int c = 0; c < columns_; c++)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem();
                onionTables[i]->setItem(r, c, newItem);
            }
        }
    }
}

/*
 * Gets the index of the current frame being displayed
 */
int SpriteView::getCurrentFrameIndex()
{
    int currentFrameIndex = 0;
    for(int i = 0; i < frameCount; i++)
    {
        if(currentTableWidget == ui->framesTable->cellWidget(i,0))
            currentFrameIndex = i;
    }
    return currentFrameIndex;
}

/*
 * Used to resize main drawing table and onion skinning tables when user resizes.
 */
void SpriteView::resizeEvent(QResizeEvent* event)
{
    if(columns_>rows_)
    {
        int ratio = ceil((double)rows_*(ui->tableWidget->width()/columns_));
        ui->tableWidget->setFixedHeight(ratio);
        ui->tableWidget->setFixedWidth(QWIDGETSIZE_MAX);
        for(int i = 0; i < 3; i++)
        {
            if(onionTables[i] !=NULL)
            {
                onionTables[i]->setFixedHeight(ui->tableWidget->height());
                onionTables[i]->setFixedWidth(ui->tableWidget->maximumWidth());
            }
        }
    }
    else if(ui->tableWidget->rowCount()!=0 && ui->tableWidget->columnCount()!=0)
    {
        int ratio = ceil((double)columns_*(ui->tableWidget->height()/rows_));
        ui->tableWidget->setFixedWidth(ratio);
        ui->tableWidget->setFixedHeight(QWIDGETSIZE_MAX);
        for(int i = 0; i < 3; i++)
        {
            if(onionTables[i] !=NULL)
            {
                onionTables[i]->setFixedHeight(ui->tableWidget->maximumHeight());
                onionTables[i]->setFixedWidth(ui->tableWidget->width());
            }
        }
    }
    event->isAccepted();
}

/*
 * Used to trigger a resizeEvent in code when new file or open file is clicked.
 */
void SpriteView::triggerResizeEvent()
{
    QSize newSize(100,100);
    QSize oldSize = this->size();
    triggerResizer = new QResizeEvent(newSize,oldSize);
    resizeEvent(triggerResizer);
}

/*
 * Clean up when new file is made, file is opened, or closing app
 */
void SpriteView::cleanUp()
{
    //Main Table
    for (int r = 0; r < rows_; r++)
    {
        for (int c = 0; c < columns_; c++)
        {
            delete ui->tableWidget->item(r,c);
        }
    }

    //Frames
    frames.clear();
    for(int i = 0; i < frameCount; i++)
    {
        for (int r = 0; r < rows_; r++)
        {
            for (int c = 0; c < columns_; c++)
            {
                delete ((QTableWidget*)ui->framesTable->cellWidget(i,0))->item(r,c);
            }
        }
        delete ui->framesTable->item(i, 0);
    }

    //Onion Skinning
    if(onionTables[0] != NULL)
    {
        for(int i = 0; i < 3; i++)
        {
            onionTables[i]->hide();
            for (int r = 0; r < rows_; r++)
            {
                for (int c = 0; c < columns_; c++)
                {
                    delete onionTables[i]->item(r,c);
                }
            }
            delete onionTables[i];
            onionTables[i]=NULL;
        }
    }
    if(stackedLayout!=NULL)
        delete stackedLayout;
    if (triggerResizer!=NULL)
        delete triggerResizer;
}
SpriteView::~SpriteView()
{
    cleanUp();
    delete timer;
    delete ui;
}
