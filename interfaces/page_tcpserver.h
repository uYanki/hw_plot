#ifndef PAGE_TCPSERVER_H
#define PAGE_TCPSERVER_H

#include "datahandler.h"

#include <QMessageBox>

#include <QTimer>
#include <QComboBox>

#include <QMouseEvent>
#include <QHostInfo>
#include <QClipboard>
#include <QRegExpValidator>

#include <QTcpServer>

namespace Ui {
    class page_tcpserver;
}

class page_tcpserver : public datahandler {
    Q_OBJECT

public:
    QTcpServer* const  m_Server;
    QList<QTcpSocket*> m_Clients;

    bool start(void) Q_DECL_OVERRIDE;
    void stop(void) Q_DECL_OVERRIDE;
    void senddata(const QByteArray& bytes) Q_DECL_OVERRIDE;

    explicit page_tcpserver(QWidget* parent = nullptr);
    ~page_tcpserver();

private:
    Ui::page_tcpserver* ui;

    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
};

#endif  // PAGE_TCPSERVER_H
