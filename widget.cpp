#include "widget.h"
#include "ui_widget.h"

// QString、QStringLitera、QLatin1String: https://wenku.baidu.com/view/ced6d629f22d2af90242a8956bec0975f465a401.html

#define QStr QStringLiteral

Widget::Widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    m_Serial    = new QSerialPort(this);
    m_TcpServer = new QTcpServer(this);
    m_TcpClient = new QTcpSocket(this);
    m_Udp       = new QUdpSocket(this);

    initUI();
    initVal();
}

Widget::~Widget() { delete ui; }

void Widget::initUI() {
    /************** window **************/

    setWindowTitle(QStr("uykAssist"));

    /************** tabbar_interface **************/

    ui->tabbar_interface->setCurrentIndex(0);
    ui->tabbar_interface->tabBar()->hide();  // 隐藏夹头

    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->tabbar_interface, SLOT(setCurrentIndex(int)));

    // shortcut (show/hide)
     m_hkShowInterface = new QShortcut(QKeySequence("A"), this);
     connect(m_hkShowInterface, &QShortcut::activated, [&]() {
         ui->groupBox->setVisible(!ui->groupBox->isVisible());
         ui->groupBox_3->setVisible(!ui->groupBox_3->isVisible());
    });

    /************** [interface] serial **************/

    // 串口扫描
    ui->cmb_serial_port->installEventFilter(this);

    // 串口描述
    ui->label_serial_port_desc->setVisible(false);
    connect(ui->cmb_serial_port, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { ui->label_serial_port_desc->setText(ui->cmb_serial_port->currentData().toString()); });

    // 参数设置
    connect(ui->cmb_serial_baudrate, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setBaudRate(ui->cmb_serial_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_serial_databits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setDataBits((QSerialPort::DataBits)(ui->cmb_serial_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_serial_parity, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setParity((QSerialPort::Parity)i); });                                                     // 校验位
    connect(ui->cmb_serial_flowcontrol, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setFlowControl((QSerialPort::FlowControl)i); });                                      // 数据流控
    connect(ui->cmb_serial_stopbits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) {
        switch (i) {  //停止位
            case 0: m_Serial->setStopBits(QSerialPort::OneStop); break;
            case 1: m_Serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: m_Serial->setStopBits(QSerialPort::TwoStop); break;
        }
    });

    // 流控信号
    connect(ui->chk_serial_signal_DTR, &QCheckBox::stateChanged, [&](int i) { m_Serial->setDataTerminalReady(i); });
    connect(ui->chk_serial_signal_RTS, &QCheckBox::stateChanged, [&](int i) { m_Serial->setRequestToSend(i); });

    /************** input & btn 's menu **************/

    // 重复发送
    ui->btn_send->setMenu(m_MenuOfSendBtn = new QMenu(this));
    QAction* actRepeatSend = m_MenuOfSendBtn->addAction(QStr("Continuous sending"));
    actRepeatSend->setCheckable(true);
    connect(actRepeatSend, &QAction::toggled, [&](bool i) { m_CntrRepeatSend->setStyleSheet(QString("color:%1;").arg(i ? "black" : "gray")); });

    m_MenuOfSendBtn->addAction(new uyk_custom_action([&](QWidget* parent) -> QWidget* {
        m_CntrRepeatSend = new QWidget(parent);
        m_CntrRepeatSend->setStyleSheet("color:gray;");
        QFormLayout* layout = new QFormLayout(m_CntrRepeatSend);
        layout->setMargin(6);
        // 发送延时
        layout->addRow(QStr("delay:"), m_SpnRepeatDelay = new QSpinBox(m_CntrRepeatSend));
        m_SpnRepeatDelay->setRange(0, 99999);
        m_SpnRepeatDelay->setSuffix(QStr(" ms"));  // 后缀
        m_SpnRepeatDelay->setValue(1000);          // 默认延时
        // 发送次数
        layout->addRow(QStr("times:"), m_SpnRepeatTimes = new QSpinBox(m_CntrRepeatSend));
        m_SpnRepeatTimes->setToolTip(QStr("-1: infinity"));
        m_SpnRepeatTimes->setRange(-1, 9999);
        return m_CntrRepeatSend;
    },this));

    // 发送区和接收区的宽度调整

    QSplitter* spl1 = new QSplitter(Qt::Orientation::Horizontal, this);
    ui->hbox_data->addWidget(spl1);
    spl1->setHandleWidth(1);  // 分割条宽度

    spl1->addWidget(ui->group_recv);
    spl1->addWidget(ui->group_send);
    spl1->setStretchFactor(0, 3);  // 初始比例
    spl1->setStretchFactor(1, 2);

    // 发送区和接收区的右键菜单

    m_MenuOfRecv = new QMenu(this);
    m_MenuOfSend = new QMenu(this);

    // menu->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->input_recv, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfRecv->exec(QCursor::pos()); });  // 弹出菜单
    connect(ui->input_send, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfSend->exec(QCursor::pos()); });

    m_MenuOfRecv->addAction(QStr("raw data"), [&]() { ui->input_recv->clear(); })->setCheckable(true);
    m_MenuOfRecv->addAction(QStr("timestamp"), [&]() { ui->input_recv->clear(); })->setCheckable(true);
    m_MenuOfRecv->addSeparator();
    m_MenuOfRecv->addAction(QStr("save to txt"), [&]() {});
    m_MenuOfRecv->addAction(QStr("save to csv"), [&]() {});
    m_MenuOfRecv->addAction(QStr("clear"), [&]() { ui->input_recv->clear(); });

    m_MenuOfSend->addAction(QStr("clear"), [&]() { ui->input_send->clear(); });

    /******** oper tree *******/

    // test item

    new uyk_treeitem_oper(ui->tree_oper, QStr("chan1"));
    new uyk_treeitem_oper(ui->tree_oper, QStr("chan2"));
    new uyk_treeitem_oper(ui->tree_oper, QStr("chan3"));
    new uyk_treeitem_oper(ui->tree_oper, QStr("chan4"));

    /******** plot *******/


}

void Widget::initVal() {


    // 指令头长度
    m_LenOfCmdPrefix = m_CmdPrefix.length();

    m_MenuOfRecv = new QMenu(this);  // 自定义菜单
    ui->input_recv->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    m_MenuOfRecv->addAction(QStringLiteral("clear"), [&]() { ui->input_recv->clear(); });
    m_MenuOfRecv->addAction(QStringLiteral("save"), [&]() { savefile("txt", [&](QTextStream& out) { out << ui->input_recv->toPlainText(); }); });
    m_MenuOfRecv->addAction(QStringLiteral("logMd"), [&]() { ui->input_recv->clear(); });  // 日志模式
    m_MenuOfRecv->addAction(QStringLiteral("raw data"), [&]() { m_RawDataMd = !m_RawDataMd; }); // 不解析模式
    connect( ui->input_recv, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfRecv->exec(QCursor::pos()); });  // 弹出菜单

    // 数据发送
    connect(ui->btn_send, &QPushButton::clicked, [&]() { SendData(QByteArray(ui->input_send->toPlainText().toLatin1())); });

    // 串口扫描
    ScanSerialPort();
    m_Serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
     m_Serial->setParity(QSerialPort::Parity::NoParity);
     m_Serial->setDataBits(QSerialPort::DataBits::Data8);
     m_Serial->setStopBits(QSerialPort::StopBits::OneStop);
     m_Serial->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    // 默认ip地址
    QHostInfo ip_info = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress address, ip_info.addresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            QString localIP = address.toString();
            ui->label_tcp_server_local_ip->setText(localIP);
            ui->input_tcp_client_ip->setText(localIP);
            break;
        }
    }
    // 左键双击复制本地ip地址
    ui->label_tcp_server_local_ip->installEventFilter(this);


    // input limit
    QValidator* validator = new QIntValidator(0, 65535, this);
    ui->input_tcp_server_port->setValidator(validator);
    ui->input_tcp_client_port->setValidator(validator);
    ui->input_udp_remote_ip->setValidator(validator);

    // configure data handler

    // serial
    connect(m_Serial, &QSerialPort::readyRead, [&]() { while(m_Serial->bytesAvailable()){AnalyzeCmd(m_Serial->readLine());} });

    // client
    m_TmrReconnect = new QTimer(this);
    connect(m_TmrReconnect, &QTimer::timeout, [&]() {  // auto recconnect
        if (ui->chk_tcp_client_auto_reconnect->checkState() &&
            ui->cmb_interface->currentText() == "TCP Client") {
            m_TcpClient->connectToHost(ui->input_tcp_client_ip->text(), ui->input_tcp_client_port->text().toUInt(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        } else
            m_TmrReconnect->stop();
    });
    connect(m_TcpClient, &QTcpSocket::readyRead, [&]() { while(m_TcpClient->bytesAvailable()){AnalyzeCmd(m_TcpClient->readLine());} });
    connect(m_TcpClient, &QTcpSocket::connected, [&]() { m_TmrReconnect->stop(); });
    connect(m_TcpClient, &QTcpSocket::disconnected, [&]() { if(ui->chk_tcp_client_auto_reconnect->checkState()) m_TmrReconnect->start(2000); });

    // server
    connect(m_TcpServer, &QTcpServer::newConnection, [&]() {
        while (m_TcpServer->hasPendingConnections()) {
            QTcpSocket* client = m_TcpServer->nextPendingConnection();
            m_TcpSerClis.append(client);
            QString client_info = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
            // ui->cmb_tcp_server_connections->addItem(client_info);
            ui->input_recv->appendPlainText("[" + client_info + "]Soket Connected");
            // 注:以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 就会导致内存泄漏
            connect(client, &QTcpSocket::readyRead, [this, client]() { while(client->bytesAvailable()){AnalyzeCmd(client->readLine());} });              // 数据接收
            connect(client, &QTcpSocket::disconnected, [this, client, client_info]() {  // 掉线检测
                client->deleteLater();
                m_TcpSerClis.removeOne(client);
                ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerClis.count()));
                // ui->cmb_tcp_server_connections->removeItem(client_info);
                ui->input_recv->appendPlainText("[" + client_info + "]Soket Disconnected");
            });
        }
        ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerClis.count()));
    });

    ui->plot->yAxis->setRange(0,6000);

}



bool Widget::SendData(QByteArray data) {
    if (!ui->btn_run->isChecked() || data.isEmpty()) return false;
    ui->label_bytes_of_send->setText("Send: " + QString::number(m_BytesOfSend += data.length()));
    if (ui->cmb_interface->currentText() == "Serial") {  // disconnect serial
        m_Serial->write(data);
    } else if (ui->cmb_interface->currentText() == "TCP Server") {  // stop listen
        foreach (auto client, m_TcpSerClis)
            client->write(data);
    } else if (ui->cmb_interface->currentText() == "TCP Client") {  // disconnect tcp server
        m_TcpClient->write(data);
    } else if (ui->cmb_interface->currentText() == "UDP") {  // disconnect udp
        m_Udp->write(data);
    }
    return true;
}

void Widget::on_btn_run_clicked() {
    if (ui->btn_run->isChecked()) {  // start running

        // connect serial
        if (ui->cmb_interface->currentText() == "Serial") {
            m_Serial->setPortName(ui->cmb_serial_port->currentText());
            if (m_Serial->open(QIODevice::ReadWrite)) {
                ui->cmb_serial_port->setEnabled(true);
            } else {
                QMessageBox::information(this, "error", "fail to connect");
                return;  // 连接失败
            }

        }
        // start listen
        else if (ui->cmb_interface->currentText() == "TCP Server") {
            m_TcpServer->listen(QHostAddress::Any, ui->input_tcp_server_port->text().toUInt());
        }
        // connect tcp server
        else if (ui->cmb_interface->currentText() == "TCP Client") {
            m_TcpClient->connectToHost(ui->input_tcp_client_ip->text(), ui->input_tcp_client_port->text().toUInt(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
            m_TcpClient->waitForConnected(3000);
            if (m_TcpClient->state() == QAbstractSocket::UnconnectedState) {
                QMessageBox::information(this, "error", "fail to connect");
                return;
            }

        }
        // connect udp
        else if (ui->cmb_interface->currentText() == "UDP") {
        }

        ui->cmb_interface->setEnabled(false);
        ui->btn_run->setChecked(true);

    } else {  // stop running

        // disconnect serial
        if (ui->cmb_interface->currentText() == "Serial") {
            ui->cmb_serial_port->setEnabled(true);
            m_Serial->close();
        }
        // stop listen
        else if (ui->cmb_interface->currentText() == "TCP Server") {
            // 需先断开所有连接, 否则还是会接收到客户发送来的消息的
            foreach (auto client, m_TcpSerClis)
                client->close();
            m_TcpServer->close();
            m_TcpSerClis.clear();
        }
        // disconnect tcp server
        else if (ui->cmb_interface->currentText() == "TCP Client") {
            m_TmrReconnect->stop();
            m_TcpClient->disconnectFromHost();
            m_TcpClient->close();
        }
        // disconnect udp
        else if (ui->cmb_interface->currentText() == "UDP") {
            m_Udp->close();
        }

        ui->cmb_interface->setEnabled(true);
        ui->btn_run->setChecked(false);
        m_CmdBuffer.clear();

    }
}


void Widget::ScanSerialPort() {
    ui->cmb_serial_port->clear();
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo& info, infos)
        ui->cmb_serial_port->addItem(info.portName(), info.description());
    ui->label_serial_port_desc->setVisible(infos.count() != 0);  // 串口描述
}

bool Widget::AnalyzeCmd(QByteArray recv) {
    m_CmdBuffer.append(recv);

    // 对接收的字节进行计数
    ui->label_bytes_of_recv->setText(QString("Recv: %1").arg(m_BytesOfRecv += recv.length()));

    // 显示接收的原内容
    if(m_RawDataMd){
        ui->input_recv->moveCursor(QTextCursor::End);
        ui->input_recv->insertPlainText(recv);
    }
    // 注: appendPlainText(recv) 会自动换行,所以不使用该函数

    if (!recv.contains('\n')) return false;  // 按行读取

    // 指令解析(format:##{...}##)

    int start = m_CmdBuffer.indexOf(m_CmdPrefix);
    if (start == -1) return false;
    start += m_LenOfCmdPrefix;
    int end = m_CmdBuffer.indexOf(m_CmdPuffix, start);
    if (end == -1) return false;


    QString CMD= m_CmdBuffer.mid(start, end - start);
    // 显示接收的命令
    if(!m_RawDataMd){
        ui->input_recv->moveCursor(QTextCursor::End);
        ui->input_recv->insertPlainText(CMD);
    }
    HandleCmd(CMD);

    m_CmdBuffer.clear();

    return true;
}

bool Widget::HandleCmd(QString cmd) {
    // 指令内容 val1,val2,val3...
    QStringList list = cmd.split(',', QString::SkipEmptyParts);
    if (list.length() == 0) return false;

    /*********** 执行指令 ***********/


    // 添加数据


    QVector<double> vals;

    for (int i = 0; i < list.length(); ++i)  vals << list.at(i).toFloat();

    ui->plot->addVals(vals);

    ui->plot->xAxis->setRange((ui->plot->m_index1 > 2000) ? (ui->plot->m_index1 - 2000) : 0, ui->plot->m_index1);

    return true;
}

bool Widget::eventFilter(QObject* watched, QEvent* event) {
    // scan available serial ports when hover enter
    if (watched == ui->cmb_serial_port) {
        if (event->type() == QEvent::HoverEnter)  // 鼠标进入
            ScanSerialPort();
    }
    // copy local ip to clipboard
    else if (watched == ui->label_tcp_server_local_ip) {
        if (event->type() == QEvent::MouseButtonDblClick) {  // 左键双击
            QMouseEvent* mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->button() == Qt::LeftButton) {
                QApplication::clipboard()->setText(ui->label_tcp_server_local_ip->text());  // 复制到剪辑版
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}
