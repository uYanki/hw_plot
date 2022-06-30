#include "dataarea.h"
#include "ui_dataarea.h"

dataarea::dataarea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataarea)
{
    ui->setupUi(this);

    // 默认宽度占比
    ui->splitter->setStretchFactor(0,7);
    ui->splitter->setStretchFactor(1,3);
}

dataarea::~dataarea()
{
    delete ui;
}
