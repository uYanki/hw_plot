#ifndef PAGE_TCPCLIENT_H
#define PAGE_TCPCLIENT_H

#include <QWidget>

namespace Ui {
    class page_tcpclient;
}

class page_tcpclient : public QWidget {
    Q_OBJECT

public:
    explicit page_tcpclient(QWidget* parent = nullptr);
    ~page_tcpclient();

private:
    Ui::page_tcpclient* ui;
};

#endif  // PAGE_TCPCLIENT_H
