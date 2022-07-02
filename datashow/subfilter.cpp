#include "subfilter.h"
#include "ui_subfilter.h"

subfilter::subfilter(QWidget* parent) : QWidget(parent), ui(new Ui::subfilter) {
    ui->setupUi(this);
    ui->input_recv->bindMenu();
}

subfilter::~subfilter() { delete ui; }

void subfilter::recvcmd(const QString& recv) {
    if (ui->input_filter->text().isEmpty() || recv.contains(ui->input_filter->text()))
        ui->input_recv->append(recv);
}
