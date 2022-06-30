#ifndef PAGE_TCPSERVER_H
#define PAGE_TCPSERVER_H

#include <QMessageBox>

#include <QClipboard>
#include <QMenu>

#include <QCheckBox>

#include <QTcpServer>
#include <QTcpSocket>

#include "datahandler.h"
#include "getlocalip.h"

namespace Ui {
    class page_tcpserver;
}

class page_tcpserver : public datahandler {
    Q_OBJECT

public:
    explicit page_tcpserver(QWidget* parent = nullptr);
    ~page_tcpserver();

private:
    Ui::page_tcpserver* ui;

    QMenu* const m_Menu;

public:
    QTcpServer* const  m_Server;
    QList<QTcpSocket*> m_Clients;

    void start(void) Q_DECL_OVERRIDE;
    void stop(void) Q_DECL_OVERRIDE;
    void senddata(const QByteArray& bytes) Q_DECL_OVERRIDE;

private:
    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
private slots:
    void on_cmb_local_ip_currentIndexChanged(const QString& arg1);  // 选中的IP置剪辑版
};

#endif  // PAGE_TCPSERVER_H
