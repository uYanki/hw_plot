#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QFormLayout>

#include <QComboBox>
#include <QMenu>
#include <QShortcut>
#include <QSplitter>
#include <QTimer>

#include <QDoubleSpinBox>
#include <QSpinBox>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QHostInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

/* custom */
#include "uyk_custom_action.h"
#include "uyk_treeitem_chan.h"
#include "uyk_treeitem_oper.h"
#include "uyk_baseplot.h"

/* plugins */


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

    void initUI(void);
    void initVal(void);

    void (QComboBox::*pSIGNAL_COMBOBOX_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;

    /************** interface **************/

    QSerialPort* m_Serial = nullptr;

    // client -> cli, server -> ser
    QTcpServer*        m_TcpServer = nullptr;
    QList<QTcpSocket*> m_TcpSerClis;  // Connections

    QTcpSocket* m_TcpClient    = nullptr;
    QTimer*     m_TmrReconnect = nullptr;

    QUdpSocket* m_Udp = nullptr;

    void ScanSerialPort(void);
    bool SendData(QByteArray data);
    bool m_RawDataMd=true;

    /************** statistics **************/

    size_t m_BytesOfRecv = 0;
    size_t m_BytesOfSend = 0;

    /************** menu **************/

    QMenu* m_MenuOfRecv    = nullptr;  // @ input_recv
    QMenu* m_MenuOfSend    = nullptr;  // @ input_send
    QMenu* m_MenuOfSendBtn = nullptr;  // @ btn_send

    /************** Continuous sending **************/

    QWidget*  m_CntrRepeatSend;  // container -> cntr
    QSpinBox* m_SpnRepeatDelay = nullptr;
    QSpinBox* m_SpnRepeatTimes = nullptr;

    /********** hotkey **********/

    QShortcut* m_hkShowInterface;

    /********** command **********/

    size_t m_LenOfCmdPrefix = 0;

    QByteArray m_CmdBuffer = "";       // 缓冲区
    QString    m_CmdPrefix = "this:";  // 指令前缀 "##{"
    QString    m_CmdPuffix = "\n";     // 指令后缀 "}##"

    bool AnalyzeCmd(QByteArray recv);
    bool HandleCmd(QString cmd);

    bool eventFilter(QObject* watched, QEvent* event);
};
#endif  // WIDGET_H
