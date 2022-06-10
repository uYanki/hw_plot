#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QMenu>
#include <QTimer>

#include <QClipboard>
#include <QMessageBox>

#include <QShortcut>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QHostInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

#include "u_baseplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

private slots:
    void on_btn_run_clicked();

private:
    Ui::Widget* ui;

    // interface ( client -> cli, server -> ser )

    QSerialPort* m_Serial = nullptr;

    QTcpServer*        m_TcpServer = nullptr;
    QList<QTcpSocket*> m_TcpSerConnections;

    QTcpSocket* m_TcpClient    = nullptr;
    QTimer*     m_TmrReconnect = nullptr;

    QUdpSocket* m_Udp = nullptr;

    void ScanSerialPort(void);
    bool SendData(QByteArray data);

    /********** hotkey **********/

    QShortcut* m_hkConnect;

    /********** statistics **********/

    size_t m_BytesOfRecv = 0;
    size_t m_BytesOfSend = 0;

    /********** menu **********/

    QMenu* m_MenuOfRecv = nullptr;  // @ input_recv
    QMenu* m_MenuOfSend = nullptr;  // @ input_send

    /********** command **********/

    size_t m_LenOfCmdPrefix = 0;

    QByteArray m_CmdBuffer = "";       // 缓冲区
    QString    m_CmdPrefix = "this:";  // 指令前缀 "##{"
    QString    m_CmdPuffix = "\n";     // 指令后缀 "}##"

    bool AnalyzeCmd(QByteArray recv);
    bool HandleCmd(QString cmd);

    bool eventFilter(QObject* watched, QEvent* event);

    /********** plot **********/

    u_baseplot* m_plot;
    size_t      m_cur_data_idx = 0;

    bool savefile(QString suffix, std::function<void(QTextStream&)> pFunc);
};
#endif  // WIDGET_H
