#ifndef TAB_INTERFACES_H
#define TAB_INTERFACES_H

#include <QWidget>

namespace Ui {
    class tab_interfaces;
}

class tab_interfaces : public QWidget {
    Q_OBJECT

public:
    explicit tab_interfaces(QWidget* parent = nullptr);
    ~tab_interfaces();

private slots:
    void on_btn_run_clicked();

private:
    Ui::tab_interfaces* ui;
};

#endif  // TAB_INTERFACES_H
