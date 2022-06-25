
#include "tab_interfaces.h"
#include "ui_tab_interfaces.h"

#define QStr QStringLiteral

tab_interfaces::tab_interfaces(QWidget* parent) : QWidget(parent), ui(new Ui::tab_interfaces) {
    ui->setupUi(this);

    // init ui

    ui->tabbar_interface->setCurrentIndex(0);
    ui->tabbar_interface->tabBar()->hide();  // 隐藏夹头
    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->tabbar_interface, SLOT(setCurrentIndex(int)));

    // range limit @port

    pINPUT_RANGE_LIMIT = new QRegExpValidator(QRegExp(LIMIT_PORT), this);
    ui->input_tcp_server_port->setValidator(pINPUT_RANGE_LIMIT);
    ui->input_tcp_server_port->setValidator(pINPUT_RANGE_LIMIT);

    // quickly run / stop (hotkey:D)
    QShortcut* hkRun = new QShortcut(QKeySequence("D"), this);
    connect(hkRun, &QShortcut::activated, [&]() {
        ui->btn_run->setChecked(!ui->btn_run->isChecked());
        on_btn_run_clicked();
    });

    // get local ip
    QHostInfo host = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress address, host.addresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            m_LocalIP = address.toString();

    ui->input_tcp_client_ip->setText(m_LocalIP);
    ui->label_tcp_server_local_ip->setText(m_LocalIP);

    // 左键双击复制本地ip地址
    ui->label_tcp_server_local_ip->installEventFilter(this);

    // init interfaces

    initSerialPort();
    initTcpServer();
    initTcpClient();
    initUdp();

    // send and recv speed calc
    m_TmrSpeedCalc = new QTimer(this);
    connect(m_TmrSpeedCalc, &QTimer::timeout, [&]() {
        // 数据传输速率 (kBps)
        QString       m_SpeedOfsend;
        static size_t last_BytesOfRecv = 0;
        static size_t last_BytesOfSend = 0;
        QString       m_SpeedOfRecv    = QString("%1 kB/s").arg((m_BytesOfRecv - last_BytesOfRecv) / 1024.0f);
        QString       m_SpeedOfSend    = QString("%1 kB/s").arg((last_BytesOfSend - m_BytesOfSend) / 1024.0f);
        last_BytesOfRecv               = m_BytesOfRecv;
        last_BytesOfSend               = m_BytesOfSend;
        emit update_kBps(m_SpeedOfSend, m_SpeedOfRecv);
    });
}

tab_interfaces::~tab_interfaces() { delete ui; }

/**************** serialport ****************/

void tab_interfaces::initSerialPort() {
    m_SerialPort = new QSerialPort(this);

    // default params
    m_SerialPort->setBaudRate(QSerialPort::BaudRate::Baud115200);
    m_SerialPort->setParity(QSerialPort::Parity::NoParity);
    m_SerialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_SerialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_SerialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    // configure params
    connect(ui->cmb_serial_port, pSIGNAL_CMB_INDEX_CHANGE, [&](int) { ui->label_serial_port_desc->setText(ui->cmb_serial_port->currentData().toString()); });
    connect(ui->cmb_serial_baudrate, pSIGNAL_CMB_INDEX_CHANGE, [&](int) { m_SerialPort->setBaudRate(ui->cmb_serial_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_serial_databits, pSIGNAL_CMB_INDEX_CHANGE, [&](int) { m_SerialPort->setDataBits((QSerialPort::DataBits)(ui->cmb_serial_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_serial_parity, pSIGNAL_CMB_INDEX_CHANGE, [&](int i) { m_SerialPort->setParity((QSerialPort::Parity)i); });                                                     // 校验位
    connect(ui->cmb_serial_flowcontrol, pSIGNAL_CMB_INDEX_CHANGE, [&](int i) { m_SerialPort->setFlowControl((QSerialPort::FlowControl)i); });                                      // 数据流控
    connect(ui->cmb_serial_stopbits, pSIGNAL_CMB_INDEX_CHANGE, [&](int i) {
        switch (i) {  //停止位
            case 0: m_SerialPort->setStopBits(QSerialPort::OneStop); break;
            case 1: m_SerialPort->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: m_SerialPort->setStopBits(QSerialPort::TwoStop); break;
        }
    });

    // set flow control signals
    connect(ui->chk_serial_signal_DTR, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setDataTerminalReady(i==Qt::CheckState::Checked); });
    connect(ui->chk_serial_signal_RTS, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setRequestToSend(i==Qt::CheckState::Checked); });

    // scan serial port
    scanSerialPort();
    ui->cmb_serial_port->installEventFilter(this);  // 鼠标进入串口选择框时扫描串口

    // read data
    connect(m_SerialPort, &QSerialPort::readyRead, [&]() { while(m_SerialPort->bytesAvailable()){ analyzeCmd(m_SerialPort->readLine());} });

    // handle error
    connect(m_SerialPort, &QSerialPort::errorOccurred, [&](QSerialPort::SerialPortError e) {
        if (e != QSerialPort::SerialPortError::NoError) {
            listen(false);
            delay_ms(1000);  // 延时再扫描串口, 等系统反应过来
            scanSerialPort();
        }
    });
}

void tab_interfaces::scanSerialPort() {
    ui->cmb_serial_port->clear();
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo& info, infos)
        ui->cmb_serial_port->addItem(info.portName(), info.description());
    ui->label_serial_port_desc->setVisible(infos.count() != 0);  // 串口描述
}

bool tab_interfaces::openSerialPort() {
    if (ui->cmb_serial_port->currentText().isEmpty()) return false;
    m_SerialPort->setPortName(ui->cmb_serial_port->currentText());
    if (m_SerialPort->open(QIODevice::ReadWrite)) {
        ui->cmb_serial_port->setEnabled(false);
        ui->chk_serial_signal_DTR->setEnabled(true);
        ui->chk_serial_signal_RTS->setEnabled(true);
        return true;
    }
    QMessageBox::warning(this, "ERROR", "fail to open serial port");
    return false;
}

void tab_interfaces::closeSerialPort() {
    ui->cmb_serial_port->setEnabled(true);
    ui->chk_serial_signal_DTR->setEnabled(false);
    ui->chk_serial_signal_RTS->setEnabled(false);
    m_SerialPort->close();
}

/**************** tcpserver ****************/

void tab_interfaces::initTcpServer() {
    m_TcpServer = new QTcpServer(this);

    connect(m_TcpServer, &QTcpServer::newConnection, [&]() {
        while (m_TcpServer->hasPendingConnections()) {
            QTcpSocket* client = m_TcpServer->nextPendingConnection();
            m_TcpSerClis.append(client);
            QString client_info = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
            // ui->cmb_tcp_server_connections->addItem(client_info);
            // ui->input_recv->appendPlainText("[" + client_info + "]Soket Connected");
            // 注: 以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 就会导致内存泄漏
            connect(client, &QTcpSocket::readyRead, [this, client]() { while(client->bytesAvailable()){analyzeCmd(client->readLine());} });              // 数据接收
            connect(client, &QTcpSocket::disconnected, [this, client, client_info]() {  // 掉线检测
                client->deleteLater();
                m_TcpSerClis.removeOne(client);
                ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerClis.count()));
                // ui->cmb_tcp_server_connections->removeItem(client_info);
                // ui->input_recv->appendPlainText("[" + client_info + "]Soket Disconnected");
            });
        }
        ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerClis.count()));
    });
}

bool tab_interfaces::openTcpServer() {
    uint16_t port = ui->input_tcp_server_port->text().toUInt();
    if (port == 0) return false;
    if (m_TcpServer->listen(QHostAddress::Any, port)) return true;
    QMessageBox::warning(this, QStr("ERROR"), QStr("fail to listen port"));
    return false;
}

void tab_interfaces::closeTcpServer() {
    // 需先断开所有连接, 否则服务器还是会接收到客户发送来的消息的
    foreach (auto client, m_TcpSerClis) client->close();
    m_TcpServer->close();
    m_TcpSerClis.clear();
}

/**************** tcpclient ****************/

void tab_interfaces::initTcpClient() {
    m_TcpClient = new QTcpSocket(this);

    // auto recconnect

    static size_t retry_times = 0;  // 重连次数

    m_TmrReconnect = new QTimer(this);

    connect(ui->chk_tcp_client_auto_reconnect, &QCheckBox::stateChanged, [&](int i) {if(i==Qt::CheckState::Unchecked)m_TmrReconnect->stop(); });

    connect(m_TmrReconnect, &QTimer::timeout, [&]() {
        if (ui->cmb_interface->currentIndex() != 2) {  // tcp client
            m_TmrReconnect->stop();
            return;
        }
        if (!listen(true)) emit recvLog(QString("fail to reconnect tcp server ... retry times: %1").arg(++retry_times));
    });

    connect(m_TcpClient, &QTcpSocket::readyRead, [&]() { while(m_TcpClient->bytesAvailable()){analyzeCmd(m_TcpClient->readLine());} });
    connect(m_TcpClient, &QTcpSocket::connected, [&]() { m_TmrReconnect->stop(); });
    connect(m_TcpClient, &QTcpSocket::disconnected, [&]() {
        listen(false);
        if (ui->chk_tcp_client_auto_reconnect->checkState()) {  // 断线重连
            retry_times = 0;
            m_TmrReconnect->start(1000);
        }
    });
}

bool tab_interfaces::openTcpClient() {
    uint16_t port = ui->input_tcp_client_port->text().toUInt();
    m_TcpClient->connectToHost(ui->input_tcp_client_ip->text(), port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    m_TcpClient->waitForConnected(2000);
    return m_TcpClient->state() == QAbstractSocket::ConnectedState;
}

void tab_interfaces::closeTcpClient() {
    m_TmrReconnect->stop();
    m_TcpClient->disconnectFromHost();
    m_TcpClient->close();
}

/**************** udp ****************/

void tab_interfaces::initUdp() {
    m_Udp = new QUdpSocket(this);
}

bool tab_interfaces::openUdp() {
    /*

    UDP是小于512字节的数据报,TCP是连续的数据流.
    - 单播:1对1数据传输
    - 广播:1对同网络范围的全部客户端(QHostAddress::Broadcast)
    - 组播:加入多播组,向组内发消息,组内的所有成员都能接收到消息(组播ip地址范围:224.0.0.0-239.255.255.255)
    writeDatagram()/readDatagram()

    单播:发送数据到指定的地址和端口号
    mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress("192.168.137.1"),6677);

    例子:mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress("224.0.0.100"),6677);

    广播,
    广播地址ip:QHostAddress::Broadcast
    例子:mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress::Broadcast,6677);

    差别在ip地址
    */

    return false;
}

void tab_interfaces::closeUdp() {}

/********************************************************************/

void tab_interfaces::on_btn_run_clicked() { listen(!m_runstate); }  // 状态反转

bool tab_interfaces::eventFilter(QObject* watched, QEvent* event) {
    // scan available serial ports when hover enter
    if (watched == ui->cmb_serial_port && event->type() == QEvent::HoverEnter) {
        scanSerialPort();
    }
    // copy local ip to clipboard
    else if (watched == ui->label_tcp_server_local_ip) {
        if (event->type() == QEvent::MouseButtonDblClick) {  // 左键双击
            QMouseEvent* mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->button() == Qt::LeftButton) {
                QApplication::clipboard()->setText(m_LocalIP);  // 复制到剪辑版
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

bool tab_interfaces::listen(bool state){
    if (state == m_runstate) return state;

    // state = true -> running
    // state = false -> wait for starting

    if (state) {
        // run
        bool b = false;
        switch (ui->cmb_interface->currentIndex()) {
            case 0: b = openSerialPort(); break;  // serial
            case 1: b = openTcpServer(); break;   // tcp server
            case 2: b = openTcpClient(); break;   // tcp client
            case 3: b = openUdp(); break;         // udp
        }
        state = b;
    } else {
        // stop
        m_buffer.clear();
        switch (ui->cmb_interface->currentIndex()) {
            case 0: closeSerialPort(); break;  // serial
            case 1: closeTcpServer(); break;   // tcp server
            case 2: closeTcpClient(); break;   // tcp client
            case 3: closeUdp(); break;         // udp
        }
    }

    // timer of speed calc

    if(state){
        m_TmrSpeedCalc->start(1000);
    }else{
        m_TmrSpeedCalc->stop();
        emit update_kBps(QStr("0 kB/s"),QStr("0 kB/s"));
    }

    // update ui
    ui->btn_run->setChecked(state);
    ui->cmb_interface->setEnabled(!state);

    return m_runstate = state;
}

bool tab_interfaces::send(QByteArray data) {
    if (!m_runstate || data.isEmpty()) return false;
    m_BytesOfSend += data.length();
    //    ui->label_bytes_of_send->setText("Send: " + QString::number(m_BytesOfSend += data.length()));

    switch (ui->cmb_interface->currentIndex()) {
        case 0: m_SerialPort->write(data); break;  // serial
        case 1:
            foreach (auto client, m_TcpSerClis) client->write(data);
            break;                                // tcp server
        case 2: m_TcpClient->write(data); break;  // tcp client
        case 3: m_Udp->write(data); break;        // udp
    }
    return true;
}

QByteArray strmid(const QByteArray& text, const QString& left, const QString& right) {
    int start = left.isEmpty() ? 0 : text.indexOf(left);
    if (start == -1) return "";
    start += left.length();
    if (right.isEmpty()) return text.right(text.size() - start);
    int end = text.indexOf(right, start);
    if (end == -1) return "";
    return text.mid(start, end - start);
}

void tab_interfaces::analyzeCmd(const QByteArray& recv) {
    m_buffer.append(recv);
    m_BytesOfRecv += recv.length();  // 字节计次
    emit recvData(recv);
    if (!recv.contains('\n')) return;                    // 按行读取
    emit recvCmd(strmid(m_buffer, m_prefix, m_suffix));  // 命令截取
    m_buffer.clear();                                    // 清除缓冲
}


