#include "tab_interfaces.h"
#include "ui_tab_interfaces.h"

#include <QDebug>
#include "datahandler.h"

tab_interfaces::tab_interfaces(QWidget* parent) : QWidget(parent), ui(new Ui::tab_interfaces) {
    ui->setupUi(this);

    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->stack, SLOT(setCurrentIndex(int)));

    connect(ui->p_serialport, &datahandler::readdata, this, &tab_interfaces::readdata);
    connect(ui->p_tcpserver, &datahandler::readdata, this, &tab_interfaces::readdata);
    connect(ui->p_tcpclient, &datahandler::readdata, this, &tab_interfaces::readdata);
    connect(ui->p_document, &datahandler::readdata, this, &tab_interfaces::readdata);

    connect(ui->p_serialport, &datahandler::readline, this, &tab_interfaces::readline);
    connect(ui->p_tcpserver, &datahandler::readline, this, &tab_interfaces::readline);
    connect(ui->p_tcpclient, &datahandler::readline, this, &tab_interfaces::readline);
     connect(ui->p_document, &datahandler::readline, this, &tab_interfaces::readline);

    connect(ui->p_serialport, &datahandler::updatestat, this, &tab_interfaces::updatestat);
    connect(ui->p_tcpserver, &datahandler::updatestat, this, &tab_interfaces::updatestat);
    connect(ui->p_tcpclient, &datahandler::updatestat, this, &tab_interfaces::updatestat);
     connect(ui->p_document, &datahandler::updatestat, this, &tab_interfaces::updatestat);

    auto p = [&](bool b) {
        state = b;
        ui->cmb_interface->setEnabled(!b);
        ui->btn_run->setChecked(b);
        emit runstate(b);
    };
    connect(ui->p_serialport, &datahandler::runstate, p);
    connect(ui->p_tcpserver, &datahandler::runstate, p);
    connect(ui->p_tcpclient, &datahandler::runstate, p);
    connect(ui->p_document, &datahandler::runstate, p);
}

tab_interfaces::~tab_interfaces() { delete ui; }

void tab_interfaces::senddata(const QByteArray& bytes) {
    // if(state)
    switch (ui->stack->currentIndex()) {
        case 0: ui->p_serialport->senddata(bytes); break;  // serialport
        case 1: ui->p_tcpserver->senddata(bytes); break;   // tcpserver
        case 2: ui->p_tcpclient->senddata(bytes); break;   // tcpclient
    }
}

void tab_interfaces::on_btn_run_clicked() {
    if (state) {
        switch (ui->stack->currentIndex()) {
            case 0: ui->p_serialport->stop(); break;  // serialport
            case 1: ui->p_tcpserver->stop(); break;   // tcpserver
            case 2: ui->p_tcpclient->stop(); break;   // tcpclient
            case 3: ui->p_document->stop(); break;   // document
        }
    } else {
        switch (ui->stack->currentIndex()) {
            case 0: ui->p_serialport->start(); break;  // serialport
            case 1: ui->p_tcpserver->start(); break;   // tcpserver
            case 2: ui->p_tcpclient->start(); break;   // tcpclient
            case 3: ui->p_document->start(); break;   // document
        }
    }
}
