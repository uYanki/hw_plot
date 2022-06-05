#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDebug>

#include <QShortcut>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

// 官网:https://www.qcustomplot.com/
// https://blog.csdn.net/happyjoey217/article/details/115007711
// https://www.jianshu.com/p/124d4ba5e816 [好文]
//#include "QCustomPlot/qcustomplot.h"

#include "u_baseplot.h"

#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    //******************************************//

private:
    QString m_cmd_buf;  // 指令缓冲区

    bool m_bConnect = false;  // 是否已连接

    QSerialPort* m_serial;

    QTcpServer*        m_tcp_server;
    QList<QTcpSocket*> m_clients;

    QTcpSocket* m_tcp_client;

    QUdpSocket* m_udp;

    u_baseplot* m_plot;
    QTimer* m_timer_replot;  // refresh plot
    QMenu*  m_plot_menu;     // right-click context menu

    size_t m_cur_data_idx = 1;

    QMenu* m_recv_input_menu;  // 原数据显示框的右键菜单

    // hotkey
    QShortcut* m_hk_debug_connect;  // auto connect (debug)
    bool       m_bDebugConnect = false;

    // labels on status bar
    size_t     m_recv_cnt = 0, m_send_cnt = 0;
    QLabel*    m_status_recv_cnt;                    // count of recv bytes
    QLabel*    m_status_send_cnt;                    // count of send bytes
    QCheckBox *m_status_chk_DTR, *m_status_chk_RTS;  // flag of serial singal

private:
    bool handleCmd(const QByteArray& recv);

private slots:
    bool eventFilter(QObject*, QEvent*);

    void on_btn_run_clicked();

public:
    bool savefile(QString suffix /*文件后缀名*/, std::function<void(QTextStream& /*缓冲区*/)> pFunc);
};
#endif  // MAINWINDOW_H
