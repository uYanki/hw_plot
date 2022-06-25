#ifndef tab_interfaces_H
#define tab_interfaces_H

#include <QWidget>

#include <QMessageBox>
#include <QTimer>

#include <QComboBox>

#include <QMouseEvent>
#include <QRegExpValidator>

#include <QClipboard>
#include <QShortcut>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QHostInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

#include "uyk_delayms.h"

namespace Ui {
    class tab_interfaces;
}

#define LIMIT_PORT "^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"  // 0~65535

class tab_interfaces : public QWidget {
    Q_OBJECT

public:
    explicit tab_interfaces(QWidget* parent = nullptr);
    ~tab_interfaces();

private:
    Ui::tab_interfaces* ui;

private:
    void (QComboBox::*pSIGNAL_CMB_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;
    QRegExpValidator* pINPUT_RANGE_LIMIT ;// 限制输入范围

    /****************** interfaces ******************/

private:
    // serial port

    QSerialPort* m_SerialPort = nullptr;

    void initSerialPort(void);
    void scanSerialPort(void);
    bool openSerialPort(void);
    void closeSerialPort(void);

    // tcp server (client -> cli, server -> ser)

    QString            m_LocalIP;
    QTcpServer*        m_TcpServer = nullptr;
    QList<QTcpSocket*> m_TcpSerClis;  // Connections

    void initTcpServer(void);
    bool openTcpServer(void);
    void closeTcpServer(void);

    // tcp client

    QTcpSocket* m_TcpClient = nullptr;

    void initTcpClient(void);
    bool openTcpClient(void);
    void closeTcpClient(void);

    QTimer* m_TmrReconnect = nullptr;

    // udp

    QUdpSocket* m_Udp = nullptr;

    void initUdp(void);
    bool openUdp(void);
    void closeUdp(void);

    bool eventFilter(QObject* watched, QEvent* event);

    /****************** send & recv ******************/

public:
    // 字节统计
    size_t m_BytesOfRecv = 0;
    size_t m_BytesOfSend = 0;

    // 指令前缀&后缀
    QString m_prefix;
    QString m_suffix;

private:

    QTimer* m_TmrSpeedCalc = nullptr; // 收发速率计算

    QByteArray m_buffer;  // 指令缓冲
    bool m_runstate = false;

public:
    bool listen(bool state);
    bool send(QByteArray data);

signals:
    // from interfaces
    void recvData(const QByteArray& recv);
    void recvCmd(const QByteArray& recv);
    // from logger
    void recvLog(const QString& log);

    // 数据收发速率 kBps (kiloByte per second，比特/秒)
    void update_kBps(const QString& send, const QString& recv);

private slots:
    void analyzeCmd(const QByteArray& recv);
    void on_btn_run_clicked();

};

#endif  // tab_interfaces_H
