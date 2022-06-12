#include "widget.h"
#include "ui_widget.h"

#define QStr QStringLiteral

void delay_ms(unsigned int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

QString getLocalIP() {
    QHostInfo host = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress address, host.addresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    return "";
}


Widget::Widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowTitle(QStr("assist"));

    initUI();

    initSerialPort();

    m_LocalIP = getLocalIP();
    initTcpServer();

    initTcpClient();

    initDataFilter();
}

Widget::~Widget() { delete ui; }

void Widget::initUI() {
    /************** tab widget **************/

    ui->tabbar_interface->setCurrentIndex(0);
    ui->tabbar_interface->tabBar()->hide();  // 隐藏夹头

    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->tabbar_interface, SLOT(setCurrentIndex(int)));

    // shortcut ( show/hide tab widget )
    QShortcut* hkTabWidget = new QShortcut(QKeySequence("A"), this);
    connect(hkTabWidget, &QShortcut::activated, [&]() { ui->tabWidget->setVisible(!ui->tabWidget->isVisible()); });


    QShortcut* hkRun = new QShortcut(QKeySequence("D"), this);
    connect(hkRun, &QShortcut::activated, [&]() {
        ui->btn_run->setChecked(!ui->btn_run->isChecked());
        on_btn_run_clicked();
    });

    // 发送区和接收区的宽度调整

     QSplitter* spl1 = new QSplitter(Qt::Orientation::Horizontal, this);
     ui->hbox_data->addWidget(spl1);
     spl1->setHandleWidth(1);  // 分割条宽度

     spl1->addWidget(ui->group_recv);
     spl1->addWidget(ui->group_send);
     spl1->setStretchFactor(0, 3);  // 初始比例
     spl1->setStretchFactor(1, 2);


     // 图表区和收发区的高度调整

      QSplitter* spl2 = new QSplitter(Qt::Orientation::Vertical, this);

      ui->verticalLayout_3->addWidget(spl2);
      spl2->setHandleWidth(1);  // 分割条宽度

      spl2->addWidget(ui->graphicsView);
      spl2->addWidget(ui->tabWidget_2);

      spl2->setSizes(QList<int>()<<1<<0); // 隐藏收发区

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

}

void Widget::initSerialPort() {
    m_SerialPort = new QSerialPort(this);

    // default params
    m_SerialPort->setBaudRate(QSerialPort::BaudRate::Baud115200);
    m_SerialPort->setParity(QSerialPort::Parity::NoParity);
    m_SerialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_SerialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_SerialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    // configure params
    connect(ui->cmb_serial_port, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { ui->label_serial_port_desc->setText(ui->cmb_serial_port->currentData().toString()); });
    connect(ui->cmb_serial_baudrate, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_SerialPort->setBaudRate(ui->cmb_serial_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_serial_databits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_SerialPort->setDataBits((QSerialPort::DataBits)(ui->cmb_serial_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_serial_parity, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_SerialPort->setParity((QSerialPort::Parity)i); });                                                     // 校验位
    connect(ui->cmb_serial_flowcontrol, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_SerialPort->setFlowControl((QSerialPort::FlowControl)i); });                                      // 数据流控
    connect(ui->cmb_serial_stopbits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) {
        switch (i) {  //停止位
            case 0: m_SerialPort->setStopBits(QSerialPort::OneStop); break;
            case 1: m_SerialPort->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: m_SerialPort->setStopBits(QSerialPort::TwoStop); break;
        }
    });

    // set flow control signals
    connect(ui->chk_serial_signal_DTR, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setDataTerminalReady(i); });
    connect(ui->chk_serial_signal_RTS, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setRequestToSend(i); });

    // scan serial port
    scanSerialPort();
    ui->cmb_serial_port->installEventFilter(this);

    // read data
    connect(m_SerialPort, &QSerialPort::readyRead, [&]() { while(m_SerialPort->bytesAvailable()){handleCommand(m_SerialPort->readLine());} });

     // error
    connect(m_SerialPort, &QSerialPort::errorOccurred, [&](QSerialPort::SerialPortError e) {
        if (e != QSerialPort::SerialPortError::NoError){
            ui->btn_run->setChecked(!ui->btn_run->isChecked());
            on_btn_run_clicked();
            delay_ms(100); // 延时再扫描串口, 等系统反应过来
            scanSerialPort();
        }
    });

}

void Widget::scanSerialPort() {
    ui->cmb_serial_port->clear();
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo& info, infos)
        ui->cmb_serial_port->addItem(info.portName(), info.description());
    ui->label_serial_port_desc->setVisible(infos.count() != 0);  // 串口描述
}

bool Widget::openSerialPort() {
    if(ui->cmb_serial_port->currentText().isEmpty()) return false;
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

void Widget::closeSerialPort() {
    ui->cmb_serial_port->setEnabled(true);
    ui->chk_serial_signal_DTR->setEnabled(false);
    ui->chk_serial_signal_RTS->setEnabled(false);
    m_SerialPort->close();
}

void Widget::initTcpServer() {
    m_TcpServer = new QTcpServer(this);

    ui->label_tcp_server_local_ip->setText(m_LocalIP);
    ui->input_tcp_server_port->setValidator(pINPUT_RANGE_LIMIT);

    connect(m_TcpServer, &QTcpServer::newConnection, [&]() {
        while (m_TcpServer->hasPendingConnections()) {
            QTcpSocket* client = m_TcpServer->nextPendingConnection();
            m_TcpSerClis.append(client);
            QString client_info = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
            // ui->cmb_tcp_server_connections->addItem(client_info);
            // ui->input_recv->appendPlainText("[" + client_info + "]Soket Connected");
            // 注:以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 就会导致内存泄漏
            connect(client, &QTcpSocket::readyRead, [this, client]() { while(client->bytesAvailable()){handleCommand(client->readLine());} });              // 数据接收
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

    // 左键双击复制本地ip地址
    ui->label_tcp_server_local_ip->installEventFilter(this);
}

bool Widget::openTcpServer() {
    uint16_t port= ui->input_tcp_server_port->text().toUInt();
    if(port == 0) return false;
    if (m_TcpServer->listen(QHostAddress::Any,port)) return true;
    QMessageBox::warning(this, "ERROR", "fail to listen port");
    return false;
}

void Widget::closeTcpServer() {
    // 需先断开所有连接, 否则服务器还是会接收到客户发送来的消息的
    foreach (auto client, m_TcpSerClis) client->close();
    m_TcpServer->close();
    m_TcpSerClis.clear();
}

void Widget::initTcpClient() {
    m_TcpClient = new QTcpSocket(this);

    ui->input_tcp_client_ip->setText(m_LocalIP);
    ui->input_tcp_server_port->setValidator(pINPUT_RANGE_LIMIT);

    m_TmrReconnect = new QTimer(this);
    connect(m_TmrReconnect, &QTimer::timeout, [&]() {  // auto recconnect
        if (ui->chk_tcp_client_auto_reconnect->checkState() &&
            ui->cmb_interface->currentText() == "TCP Client") {
            m_TcpClient->connectToHost(ui->input_tcp_client_ip->text(), ui->input_tcp_client_port->text().toUInt(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        } else
            m_TmrReconnect->stop();
    });
    connect(m_TcpClient, &QTcpSocket::readyRead, [&]() { while(m_TcpClient->bytesAvailable()){handleCommand(m_TcpClient->readLine());} });
    connect(m_TcpClient, &QTcpSocket::connected, [&]() { m_TmrReconnect->stop(); });
    connect(m_TcpClient, &QTcpSocket::disconnected, [&]() { if(ui->chk_tcp_client_auto_reconnect->checkState()) {m_TmrReconnect->start(1000); }});

}

bool Widget::openTcpClient() {
    uint16_t port= ui->input_tcp_client_port->text().toUInt();
    if(port == 0) return false;
    m_TcpClient->connectToHost(ui->input_tcp_client_ip->text(),port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    m_TcpClient->waitForConnected(1000);
    if (m_TcpClient->state() == QAbstractSocket::UnconnectedState) return false;
    m_TmrReconnect->stop();
    return true;
}

void Widget::closeTcpClient() {
    m_TmrReconnect->stop();
    m_TcpClient->disconnectFromHost();
    m_TcpClient->close();
}

void Widget::initUdp()
{
 m_Udp=new QUdpSocket(this);
}

bool Widget::openUdp()
{
/*

UDP是小于512字节的数据报，TCP是连续的数据流。
- 单播：1对1数据传输
- 广播：1对同网络范围的全部客户端（QHostAddress::Broadcast）
- 组播：加入多播组，向组内发消息，组内的所有成员都能接收到消息（组播ip地址范围：224.0.0.0-239.255.255.255）
writeDatagram()/readDatagram()

单播:发送数据到指定的地址和端口号
mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress("192.168.137.1"),6677);

例子：mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress("224.0.0.100"),6677);

广播，
广播地址ip：QHostAddress::Broadcast
例子：mSocket->writeDatagram(ui->textEdit->toPlainText().toUtf8(),QHostAddress::Broadcast,6677);

差别在ip地址
*/
}

void Widget::closeUdp()
{

}

void Widget::initDataFilter()
{
    // menu

    m_MenuDataFilter = new QMenu(this);
    m_MenuDataFilter->addAction("new filter",[&](){m_datafilters.append(new uyk_treeitem_datafilter(ui->tree_datafilter,QStr("filter %1").arg(m_datafilters.size()+1)));});

    m_MenuDataFilter->addSeparator();
    m_MenuDataFilter->addAction("expand all",[&](){foreach(auto i,m_datafilters) i->setExpanded(true);});
    m_MenuDataFilter->addAction("collapse all",[&](){foreach(auto i,m_datafilters) i->setExpanded(false);});

     m_MenuDataFilter->addSeparator();
     m_MenuDataFilter->addAction("enable all",[&](){foreach(auto i,m_datafilters) i->setCheckState(0,Qt::CheckState::Checked);});
     m_MenuDataFilter->addAction("disable all",[&](){foreach(auto i,m_datafilters) i->setCheckState(0,Qt::CheckState::Unchecked);});
     m_MenuDataFilter->addAction("remove all",[&](){ ui->tree_datafilter->clear(); m_datafilters.clear();} );

     m_MenuDataFilter->addSeparator();
     m_MenuDataFilter->addAction("load",[&](){});
     m_MenuDataFilter->addAction("save",[&](){});

    // tree

    connect(ui->chk_datafilter_enable,&QCheckBox::stateChanged,[&](int i){ui->tree_datafilter->setEnabled(i==Qt::CheckState::Checked);});

    ui->tree_datafilter->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tree_datafilter,&QTreeWidget::customContextMenuRequested,[&](){m_MenuDataFilter->exec(QCursor::pos());});

    // ui->tree_datafilter->setEditTriggers(QTreeWidget::EditTrigger::EditKeyPressed); // F2

}

QString strmid(const QByteArray& text,const QString& left,const QString& right){
    int start =left.isEmpty()?0:text.indexOf(left);
    if( start == -1) return "";
    start+=left.length();
    if(right.isEmpty()) return text.right(text.size()-start);
    int end = text.indexOf(right,start);
    if( end == -1 ) return "";
    return text.mid(start, end - start);
}

void Widget::handleCommand(const QByteArray &recv){

    m_CmdBuf.append(recv);

    m_BytesOfRecv += recv.length(); // 字节计次
    if (!recv.contains('\n')) return; // 按行读取

    if(ui->chk_datafilter_enable->checkState() == Qt::CheckState::Checked) // 过滤器
        foreach(auto i,m_datafilters) if(i->filter(m_CmdBuf)){ m_CmdBuf.clear(); return;  }

    QString cmd = strmid(m_CmdBuf,ui->input_dataformat_prefix->text(),ui->input_dataformat_suffix->text()); // 命令截取

    m_CmdBuf.clear(); // 清除缓冲区

    QStringList vals_recv;
    if(ui->input_dataformat_delimiter->text().isEmpty()){
        vals_recv << cmd;
    }else{
        vals_recv = cmd.split(ui->input_dataformat_delimiter->text(),QString::SkipEmptyParts);
    }


    QVector<double> vals;
    for(int i= 0;i<vals_recv.size();++i){
        if(m_channels.size()<i+1)  m_channels.append(new uyk_treeitem_channel (ui->tree_channel,QStr("channel %1").arg(m_channels.size()+1)));

        if(m_channels.at(i)->checkState(0)==Qt::CheckState::Checked){
            vals<<vals_recv.at(i).toDouble()*m_channels.at(i)->m_spn_yScale->value();
        }else{
            vals<<vals_recv.at(i).toDouble();
        }

    }

    qDebug()<<vals;

}

bool Widget::eventFilter(QObject* watched, QEvent* event) {
    // scan available serial ports when hover enter, 鼠标进入串口选择框时扫描串口
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

void Widget::on_btn_run_clicked() {
    bool b = false;
    if (ui->btn_run->isChecked()) {
        switch (ui->cmb_interface->currentIndex()) {
            case 0: b = openSerialPort(); break;  // serial
            case 1: b = openTcpServer(); break;   // tcp server
            case 2: b = openTcpClient(); break;   // tcp client
            case 3: b = false; break;             // udp
        }
    } else {
        m_CmdBuf.clear();
        switch (ui->cmb_interface->currentIndex()) {
            case 0: closeSerialPort(); break;  // serial
            case 1: closeTcpServer(); break;   // tcp server
            case 2: closeTcpClient(); break;   // tcp client
            case 3: break;                     // udp
        }
    }
    ui->btn_run->setChecked(b);
    ui->cmb_interface->setEnabled(!b);
}

bool Widget::sendData(QByteArray data) {
    if (!ui->btn_run->isChecked() || data.isEmpty()) return false;
    ui->label_bytes_of_send->setText("Send: " + QString::number(m_BytesOfSend += data.length()));

    switch (ui->cmb_interface->currentIndex()) {
        case 0:  m_SerialPort->write(data); break;  // serial
        case 1: foreach (auto client, m_TcpSerClis)  client->write(data); break;   // tcp server
        case 2: m_TcpClient->write(data); break;   // tcp client
        case 3: m_Udp->write(data); break;                     // udp
    }
    return true;
}
