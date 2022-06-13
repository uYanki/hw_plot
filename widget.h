#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QTimer>
#include <QShortcut>

#include <QSplitter>

#include <QMenu>
#include <QComboBox>

#include <QMessageBox>

#include <QMouseEvent>

#include <QClipboard>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QHostInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>


#include "uyk_savefile.h"
#include "uyk_treeitem_channel.h"
#include "uyk_treeitem_datafilter.h"
#include "uyk_baseplot.h"
#include "uyk_custom_action.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


QString strmid(const QByteArray& text,const QString& left,const QString& right);

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initUI(void);

    bool eventFilter(QObject* watched, QEvent* event);




    void (QComboBox::*pSIGNAL_COMBOBOX_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;

    QRegExpValidator *pINPUT_RANGE_LIMIT = new QRegExpValidator(QRegExp("^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"), this); // 1~65535

    /************** interfaces **************/

    // serial port

    QSerialPort* m_SerialPort = nullptr;

    void initSerialPort(void);
    void scanSerialPort(void);
    bool openSerialPort(void);
    void closeSerialPort(void);

    // tcp server (client -> cli, server -> ser)

    QString m_LocalIP;
    QTcpServer*        m_TcpServer = nullptr;
    QList<QTcpSocket*> m_TcpSerClis;  // Connections

    void initTcpServer(void);
    bool openTcpServer(void);
    void closeTcpServer(void);

    // tcp client

    QTcpSocket* m_TcpClient    = nullptr;
    QTimer*     m_TmrReconnect = nullptr;
    void initTcpClient(void);
    bool openTcpClient(void);
    void closeTcpClient(void);

    // udp

    QUdpSocket* m_Udp = nullptr;
    void initUdp(void);
    bool openUdp(void);
    void closeUdp(void);

    // 过滤器

    QMenu* m_MenuDataFilter = nullptr; // @ tree_datafilter
    QVector<uyk_treeitem_datafilter*> m_datafilters;
    void initDataFilter(void);

    // 图表

    QVector<uyk_treeitem_channel*> m_channels;
    QMenu* m_MenuChannel = nullptr; // @ tree_channel
    void initChanTree(void);

    // 字节统计
    size_t m_BytesOfRecv = 0;
    size_t m_BytesOfSend = 0;

    QMenu* m_MenuOfRecv    = nullptr;  // @ input_recv
    QMenu* m_MenuOfSend    = nullptr;  // @ input_send
    QMenu* m_MenuOfSendBtn = nullptr;  // @ btn_send

    // 缓冲区


    QByteArray m_CmdBuf;
    void handleCommand(const QByteArray& recv);
    bool sendData(QByteArray data);

    // 连续发送
    QWidget*  m_CntrRepeatSend;  // container -> cntr
    QSpinBox* m_SpnRepeatDelay = nullptr;
    QSpinBox* m_SpnRepeatTimes = nullptr;

    bool m_RawDataMd=false;
    bool m_TimestampMd=false;



private slots:
    void on_btn_run_clicked();

private:
    Ui::Widget *ui;
};


#endif // WIDGET_H
