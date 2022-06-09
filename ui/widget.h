#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QFormLayout>

#include <QTimer>
#include <QMenu>
#include <QComboBox>
#include <QSplitter>

#include <QSpinBox>
#include <QDoubleSpinBox>

/* serial */
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/* tcp & udp */
#include <QHostInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>


#include "uyk_menu.h"
#include "uyk_treeitem_chan.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    void initUI(void);
    void initVal(void);

    void (QComboBox::*pSIGNAL_COMBOBOX_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;

    /************** interface **************/

    QSerialPort* m_Serial = nullptr;

    // client -> cli, server -> ser
    QTcpServer*        m_TcpServer = nullptr;
    QList<QTcpSocket*> m_TcpSerClis; // Connections

    QTcpSocket* m_TcpClient    = nullptr;
    QTimer*     m_TmrReconnect = nullptr;

    QUdpSocket* m_Udp = nullptr;

    /************** statistics **************/

    size_t m_BytesOfRecv = 0;
    size_t m_BytesOfSend = 0;

    /************** menu **************/

    QMenu* m_MenuOfRecv = nullptr;  // @ input_recv
    QMenu* m_MenuOfSend = nullptr;  // @ input_send
    QMenu* m_MenuOfSendBtn = nullptr;  // @ btn_send

    /************** Continuous sending **************/

    QWidget* m_CntrRepeatSend; // container -> cntr
    QSpinBox* m_SpnRepeatDelay = nullptr;
    QSpinBox* m_SpnRepeatTimes= nullptr;


};
#endif // WIDGET_H
