#include "widget.h"
#include "ui_widget.h"



// 托盘菜单 https://blog.csdn.net/a8039974/article/details/121358839
// tcp server: https://blog.csdn.net/gongjianbo1992/article/details/107743780

Widget::Widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowTitle("uykAssist");


    m_Serial    = new QSerialPort(this);
    m_TcpServer = new QTcpServer(this);
    m_TcpClient = new QTcpSocket(this);
    m_Udp       = new QUdpSocket(this);

    void (QComboBox::*pSIGNAL_COMBOBOX_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;

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

    // 隐藏夹头
    ui->tabbar_interface->tabBar()->hide();
    // 模式切换
    ui->tabbar_interface->setCurrentIndex(0);
    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->tabbar_interface, SLOT(setCurrentIndex(int)));

    // 串口扫描
    ui->cmb_serial_port->installEventFilter(this);
    ScanSerialPort();
    // 串口描述
    ui->label_serial_port_desc->setVisible(false);
    connect(ui->cmb_serial_port, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { ui->label_serial_port_desc->setText(ui->cmb_serial_port->currentData().toString()); });
    // 默认配置 (和ui界面相对应)
    m_Serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
    m_Serial->setParity(QSerialPort::Parity::NoParity);
    m_Serial->setDataBits(QSerialPort::DataBits::Data8);
    m_Serial->setStopBits(QSerialPort::StopBits::OneStop);
    m_Serial->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    // 参数设置
    connect(ui->cmb_serial_baudrate, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setBaudRate(ui->cmb_serial_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_serial_databits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setDataBits((QSerialPort::DataBits)(ui->cmb_serial_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_serial_stopbits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) {
        switch (i) {  //停止位
            case 0: m_Serial->setStopBits(QSerialPort::OneStop); break;
            case 1: m_Serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: m_Serial->setStopBits(QSerialPort::TwoStop); break;
        }
    });
    connect(ui->cmb_serial_parity, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setParity((QSerialPort::Parity)i); });                 // 校验位
    connect(ui->cmb_serial_flowcontrol, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setFlowControl((QSerialPort::FlowControl)i); });  // 数据流控
    // 流控信号
    connect(ui->chk_serial_signal_DTR, &QCheckBox::stateChanged, [&](int i) { m_Serial->setDataTerminalReady(i); });
    connect(ui->chk_serial_signal_RTS, &QCheckBox::stateChanged, [&](int i) { m_Serial->setRequestToSend(i); });

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
    ui->input_udp_port->setValidator(validator);

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
            m_TcpSerConnections.append(client);
            QString client_info = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
            // ui->cmb_tcp_server_connections->addItem(client_info);
            ui->input_recv->appendPlainText("[" + client_info + "]Soket Connected");
            // 注:以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 就会导致内存泄漏
            connect(client, &QTcpSocket::readyRead, [this, client]() { while(client->bytesAvailable()){AnalyzeCmd(client->readLine());} });              // 数据接收
            connect(client, &QTcpSocket::disconnected, [this, client, client_info]() {  // 掉线检测
                client->deleteLater();
                m_TcpSerConnections.removeOne(client);
                ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerConnections.count()));
                // ui->cmb_tcp_server_connections->removeItem(client_info);
                ui->input_recv->appendPlainText("[" + client_info + "]Soket Disconnected");
            });
        }
        ui->label_tcp_server_count_of_connections->setText(QString::number(m_TcpSerConnections.count()));
    });


    /*********** hotkey ***********/

    m_hkConnect = new QShortcut(QKeySequence("D"), this);
    connect(m_hkConnect, &QShortcut::activated, [&]() {ui->btn_run->click(); });  // 使用快捷键进行快速连接

    /*********** serial plot ***********/

    m_plot = new u_baseplot(this);

    ui->tabbar_data_show->insertTab(1, m_plot, "Plot");

    m_plot->xAxis->setLabel("x");
    m_plot->yAxis->setLabel("y");
    m_plot->yAxis->setRange(-100, 4096 * 1.5);
    m_plot->legend->setVisible(true);
    // 左边和底边的轴 axis, 右边和顶边的轴 axis2
    m_plot->axisRect()->setupFullAxesBox();  // 显示所有轴
    // 设置坐标轴颜色
    m_plot->xAxis->grid()->setZeroLinePen(QPen(Qt::red));
    m_plot->yAxis->grid()->setZeroLinePen(QPen(Qt::red));

    // 右键菜单

    QMenu* m_plot_menu = m_plot->m_menu;

    m_plot_menu->addAction(QStringLiteral("reset"), [&]() {
        m_plot->xAxis->setRange(0, m_cur_data_idx);
        m_plot->yAxis->setRange(0, 5000);
        m_plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    });

    m_plot_menu->addAction(QStringLiteral("clear"), [&]() {
        m_plot->clearGraphs();
        m_cur_data_idx = 0;
        m_plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    });

    m_plot_menu->addAction(QStringLiteral("save one to csv"), [&]() {
        savefile("csv", [&](QTextStream& out) {
            auto cbegin = m_plot->graph(0)->data()->constBegin();
            auto cend   = m_plot->graph(0)->data()->constEnd();
            for (auto i = cbegin; i != cend; ++i) out << i->value << "\n";
        });
    });

    m_plot_menu->addAction(QStringLiteral("save all to csv"), [&]() {
        uint8_t cnt = m_plot->graphCount();
        for (int i = 0; i < cnt; ++i) {
            // m_plot->graph(i)->setName(QString("line%1").arg(i + 1));
        }
    });

    /******** channel tree *******/
    ui->tree_channel->setRootIsDecorated(true);
    ui->tree_channel->setHeaderHidden(true);
    ui->tree_channel->setIndentation(0);

}

Widget::~Widget() { delete ui; }

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

    // 自动添加曲线

    static Qt::GlobalColor color_of_series[6] = {Qt::red, Qt::darkGreen, Qt::blue, Qt::darkCyan, Qt::magenta, Qt::darkYellow};  // 曲线颜色

    for (int i = m_plot->graphCount(); i < list.length(); ++i) {
        m_plot->addGraph();
        m_plot->graph(i)->setName(QString("channel%1").arg(i + 1));
        m_plot->graph(i)->setPen(QPen(QColor(color_of_series[i]), 3));
        u_ChannelTreeItem *chan = new u_ChannelTreeItem(ui->tree_channel,QString("channel%1").arg(i + 1));
        m_channels.append(chan);
    }

    // 添加数据

    for (int i = 0; i < list.length(); ++i) {
        m_plot->graph(i)->addData(m_cur_data_idx, list.at(i).toFloat());
    }

    m_plot->xAxis->setRange((m_cur_data_idx > 2000) ? (m_cur_data_idx - 2000) : 0, m_cur_data_idx);

    m_cur_data_idx++;

    /*********** 清除缓冲区 ***********/

    return true;
}

bool Widget::SendData(QByteArray data) {
    if (!ui->btn_run->isChecked() || data.isEmpty()) return false;
    ui->label_bytes_of_send->setText("Send: " + QString::number(m_BytesOfSend += data.length()));
    if (ui->cmb_interface->currentText() == "Serial") {  // disconnect serial
        m_Serial->write(data);
    } else if (ui->cmb_interface->currentText() == "TCP Server") {  // stop listen
        foreach (auto client, m_TcpSerConnections)
            client->write(data);
    } else if (ui->cmb_interface->currentText() == "TCP Client") {  // disconnect tcp server
        m_TcpClient->write(data);
    } else if (ui->cmb_interface->currentText() == "UDP") {  // disconnect udp
        m_Udp->write(data);
    }
    return true;
}

bool Widget::savefile(QString suffix, std::function<void(QTextStream&)> pFunc) {
    // 选择文件保存路径
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString csvFile = QFileDialog::getExistingDirectory(this, "selct a path to save file", desktop);
    if (csvFile.isEmpty()) return false;

    // 以系统时间戳生成文件名
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_date      = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
    csvFile += tr("/data_%1.%2").arg(current_date, suffix);

    // 打开文件(如不存在则会自动创建)
    QFile file(csvFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;

    // 输入字符流
    QTextStream out(&file);
    pFunc(out);

    // 关闭文件
    file.close();

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
            foreach (auto client, m_TcpSerConnections)
                client->close();
            m_TcpServer->close();
            m_TcpSerConnections.clear();
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
