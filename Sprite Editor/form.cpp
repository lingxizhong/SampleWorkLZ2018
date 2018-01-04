#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Form::accept);
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

Form::~Form()
{
    delete ui;
}

/*
 * Gets inputed width
 */
int Form::getWidth()
{
    return ui->widthBox->value();
}

/*
 * Gets inputed height
 */
int Form::getHeight()
{
    return ui->heightBox->value();
}
