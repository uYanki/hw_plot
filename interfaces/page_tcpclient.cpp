#include "page_tcpclient.h"
#include "ui_page_tcpclient.h"

page_tcpclient::page_tcpclient(QWidget* parent) : QWidget(parent), ui(new Ui::page_tcpclient) {
    ui->setupUi(this);
}

page_tcpclient::~page_tcpclient() {
    delete ui;
}
