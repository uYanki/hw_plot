#ifndef PAGE_TCPCLIENT_H
#define PAGE_TCPCLIENT_H

#include <QWidget>

#include <QMessageBox>

#include <QTcpSocket>

#include "datahandler.h"
#include "getlocalip.h"

namespace Ui {
    class page_tcpclient;
}

class page_tcpclient : public datahandler {
    Q_OBJECT

public:
    explicit page_tcpclient(QWidget* parent = nullptr);
    ~page_tcpclient();

private:
    Ui::page_tcpclient* ui;

public:
    QTcpSocket* const m_Client;

    void start(void) Q_DECL_OVERRIDE;
    void stop(void) Q_DECL_OVERRIDE;
    void senddata(const QByteArray& bytes) Q_DECL_OVERRIDE;

private:
    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
    // QTimer* const m_TmrReconnect;
};

#endif  // PAGE_TCPCLIENT_H
