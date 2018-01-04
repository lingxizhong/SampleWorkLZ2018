#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QDebug>

namespace Ui {
class Form;
}

class Form : public QDialog
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    int getWidth();
    int getHeight();

private slots:

private:
    Ui::Form *ui;
};

#endif // FORM_H
