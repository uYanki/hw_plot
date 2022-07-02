#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

#include "interfaces/datahandler.h"

#include "getlocalip.h"
#include <QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    // 默认宽度占比

    // ui->splitter_1->setStretchFactor(0,1);
    // ui->splitter_1->setStretchFactor(1,0);

    ui->splitter_1->setSizes(QList<int>() << 1 << 0);  // 隐藏收发区

    ui->splitter_2->setStretchFactor(0,0);
    ui->splitter_2->setStretchFactor(1,1);


    // 连接信号和槽

    connect(ui->interfaces, &tab_interfaces::readdata,ui->datashow,&dataarea::readdata);
    connect(ui->interfaces, &tab_interfaces::updatestat,ui->datashow,&dataarea::updatestat);
    connect(ui->interfaces, &tab_interfaces::readline,ui->datafilter,&tab_dataformat::readline);
    connect(ui->datafilter, &tab_dataformat::readcmd,ui->datashow,&dataarea::readcmd);
    connect(ui->interfaces, &tab_interfaces::runstate,ui->datashow,&dataarea::runstate);
    connect(ui->datashow, &dataarea::senddata,ui->interfaces,&tab_interfaces::senddata);

    // connect(ui->datafilter, &tab_dataformat::handleVals,[&](const QVector<double> values){
    //     ui->plot->addValues(values);
    // });

    connect(ui->datafilter, &tab_dataformat::readvals, ui->plot,&plot_multicurve::addValues);


}

Widget::~Widget()
{
    delete ui;
}

