#include "tab_interfaces.h"
#include "ui_tab_interfaces.h"

#include <QDebug>
#include "datahandler.h"

tab_interfaces::tab_interfaces(QWidget* parent) : QWidget(parent), ui(new Ui::tab_interfaces) {
    ui->setupUi(this);
    connect(ui->p_serialport, &datahandler::readline, [&](const QString& recv) { qDebug() << recv; });
}

tab_interfaces::~tab_interfaces() { delete ui; }

void tab_interfaces::on_btn_run_clicked() {
    static bool state = false;

    if (state) {
        switch (ui->cmb_interface->currentIndex()) {
            case 0: ui->p_serialport->stop(); break;  // serialport
            case 1: break;                            // tcpserver
            case 2: break;                            // tcpclient
        }
        state = false;
    } else {
        switch (ui->cmb_interface->currentIndex()) {
            case 0: state = ui->p_serialport->start(); break;  // serialport
            case 1: state = ui->p_tcpserver->start(); break;                      // tcpserver
            case 2: state = false; break;                      // tcpclient
        }
    }

    ui->cmb_interface->setEnabled(!state);
    ui->btn_run->setChecked(state);
}
