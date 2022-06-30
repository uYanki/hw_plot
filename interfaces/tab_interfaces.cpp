#include "tab_interfaces.h"
#include "ui_tab_interfaces.h"

#include <QDebug>
#include "datahandler.h"

static bool state = false;

tab_interfaces::tab_interfaces(QWidget* parent) : QWidget(parent), ui(new Ui::tab_interfaces) {
    ui->setupUi(this);

    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->stack, SLOT(setCurrentIndex(int)));

    connect(ui->p_serialport, &datahandler::readline, [&](const QByteArray& recv) { qDebug() << recv; });
    connect(ui->p_tcpserver, &datahandler::readline, [&](const QByteArray& recv) { qDebug() << recv; });
    connect(ui->p_tcpclient, &datahandler::readline, [&](const QByteArray& recv) { qDebug() << recv; });

    auto p = [&](bool b) {
        state = b;
        ui->cmb_interface->setEnabled(!b);
        ui->btn_run->setChecked(b);
    };
    connect(ui->p_serialport, &datahandler::runstate, p);
    connect(ui->p_tcpserver, &datahandler::runstate, p);
    connect(ui->p_tcpclient, &datahandler::runstate, p);
}

tab_interfaces::~tab_interfaces() { delete ui; }

void tab_interfaces::on_btn_run_clicked() {
    if (state) {
        switch (ui->stack->currentIndex()) {
            case 0: ui->p_serialport->stop(); break;  // serialport
            case 1: ui->p_tcpserver->stop(); break;   // tcpserver
            case 2: ui->p_tcpclient->stop(); break;   // tcpclient
        }
        state = false;
    } else {
        switch (ui->stack->currentIndex()) {
            case 0: ui->p_serialport->start(); break;  // serialport
            case 1: ui->p_tcpserver->start(); break;   // tcpserver
            case 2: ui->p_tcpclient->start(); break;   // tcpclient
        }
    }
}
