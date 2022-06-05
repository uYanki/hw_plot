#include "mainwindow.h"
#include "ui_mainwindow.h"

// 托盘菜单 https://blog.csdn.net/a8039974/article/details/121358839

// tcp server: https://blog.csdn.net/gongjianbo1992/article/details/107743780

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    /*********** window ***********/
    setWindowTitle("SerialPlot");

    /*********** hotkey ***********/
    m_hk_debug_connect = new QShortcut(QKeySequence("D"), this);
    connect(m_hk_debug_connect, &QShortcut::activated, [&]() {
        m_bDebugConnect=true;
        ui->btn_run->click(); });  // 使用快捷键进行快速连接(调试用)

    ui->text_recv->setReadOnly(true);  // 只读模式

    m_recv_input_menu = new QMenu(this);  // 自定义菜单
    ui->text_recv->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    m_recv_input_menu->addAction(QStringLiteral("clear"), [&]() { ui->text_recv->clear(); });
    m_recv_input_menu->addAction(QStringLiteral("save"), [&]() { savefile("txt", [&](QTextStream& out) { out << ui->text_recv->toPlainText(); }); });
    m_recv_input_menu->addAction(QStringLiteral("logMd"), [&]() { ui->text_recv->clear(); });  // 日志模式

    connect(ui->text_recv, &QPlainTextEdit::customContextMenuRequested, [&]() { m_recv_input_menu->exec(QCursor::pos()); });  // 弹出菜单

    /*********** tabbar of conntion mode ***********/
    ui->tabbar_mode->setCurrentIndex(ui->cmb_mode->currentIndex());  // 默认选项卡

    ui->tabbar_mode->tabBar()->hide();       // 隐藏选择夹头
    ui->tabbar_mode->setDocumentMode(true);  // 去选择夹边框

    void (QComboBox::*pSIGNAL_cmb_INDEX_CHANGE)(int) = &QComboBox::currentIndexChanged;
    connect(ui->cmb_mode, pSIGNAL_cmb_INDEX_CHANGE, [&](int idx) {  // 模式切换
        ui->tabbar_mode->setCurrentIndex(idx);
        setWindowTitle(ui->cmb_mode->currentText());  // 窗口标题

        bool bSerial = ui->cmb_mode->currentText() == "Serial";
        m_status_chk_DTR->setVisible(bSerial), m_status_chk_RTS->setVisible(bSerial);  // 显示/隐藏 DTR&RTS
    });

    /*********** tabs ***********/

    m_serial     = new QSerialPort(this);
    m_tcp_server = new QTcpServer(this);
    m_tcp_client = new QTcpSocket(this);
    m_udp        = new QUdpSocket(this);

    // input limit
    QValidator* validator = new QIntValidator(0, 65535, this);
    ui->input_tcp_server_port->setValidator(validator);
    ui->input_tcp_client_port->setValidator(validator);
    ui->input_udp_port->setValidator(validator);

    // configure serial
    ui->cmb_serial_port->installEventFilter(this);  // 鼠标进入时刷新可用串口
    ui->cmb_serial_baud->addItems(QStringList({"9600", "115200", "460800", "921600"}));
    m_serial->setParity(QSerialPort::Parity::NoParity);
    m_serial->setDataBits(QSerialPort::DataBits::Data8);
    m_serial->setStopBits(QSerialPort::StopBits::OneStop);
    m_serial->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    // serial params have been changed
    auto pSLOT_SERIAL_PARAM_CHANGED = [&](int) { if(m_bConnect) ui->btn_run->click(); };
    connect(ui->cmb_serial_port, pSIGNAL_cmb_INDEX_CHANGE, ui->btn_run, pSLOT_SERIAL_PARAM_CHANGED);
    connect(ui->cmb_serial_baud, pSIGNAL_cmb_INDEX_CHANGE, ui->btn_run, pSLOT_SERIAL_PARAM_CHANGED);

    // configure data handler
    connect(m_serial, &QSerialPort::readyRead, [&]() { while(m_serial->bytesAvailable()){handleCmd(m_serial->readLine());} });
    connect(m_tcp_client, &QTcpSocket::readyRead, [&]() { while(m_tcp_client->bytesAvailable()){handleCmd(m_tcp_client->readLine());} });
    connect(m_tcp_server, &QTcpServer::newConnection, [&]() {
        while (m_tcp_server->hasPendingConnections()) {
            QTcpSocket* client = m_tcp_server->nextPendingConnection();
            m_clients.append(client);
            ui->text_recv->appendPlainText(QString("[%1:%2] Soket Connected").arg(client->peerAddress().toString(), client->peerPort()));
            // 注:以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 就会导致内存泄漏
            connect(client, &QTcpSocket::readyRead, [this, client]() { while(client->bytesAvailable()){handleCmd(client->readLine());} });  // 数据接收
            connect(client, &QTcpSocket::disconnected, [this, client]() {   // 掉线检测
                client->deleteLater();
                m_clients.removeOne(client);
                ui->text_recv->appendPlainText(QString("[%1:%2] Soket Disonnected").arg(client->peerAddress().toString().arg(client->peerPort())));
            });
        }
    });

    /*********** serial plot ***********/

    m_plot = new u_baseplot(this);
    ui->tabbar_function->insertTab(1, m_plot, "Plot");

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

    m_plot_menu = m_plot->m_menu;

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

    // menu.addAction(QIcon("1.png"), QStringLiteral("添加"),this,SLOT(OnBscGroupRightAction()));

    /*********** status bar ***********/
    ui->statusbar->addWidget(m_status_send_cnt = new QLabel("Send:0"), 3);
    ui->statusbar->addWidget(m_status_recv_cnt = new QLabel("Recv:0"), 3);
    ui->statusbar->addWidget(m_status_chk_DTR = new QCheckBox("DTR"), 1);
    ui->statusbar->addWidget(m_status_chk_RTS = new QCheckBox("RTS"), 1);
    connect(m_status_chk_DTR, &QCheckBox::stateChanged, m_serial, &QSerialPort::setDataTerminalReady);
    connect(m_status_chk_RTS, &QCheckBox::stateChanged, m_serial, &QSerialPort::setRequestToSend);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->cmb_serial_port) {
        if (event->type() == QEvent::HoverEnter) {  // 鼠标进入
            // scan available ports
            ui->cmb_serial_port->clear();
            foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts())  // 扫描可用串口
                ui->cmb_serial_port->addItem(info.portName() + "(" + info.description() + ")", info.portName());
        }
    }
    return QWidget::eventFilter(watched, event);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btn_run_clicked() {
    if (m_bConnect) {
        /********** disconnect **********/

        m_cmd_buf.clear();  // 清除缓冲区

        if (ui->cmb_mode->currentText() == "Serial") {  // disconnect serial
            m_serial->close();
        } else if (ui->cmb_mode->currentText() == "TCP Server") {  //
            m_tcp_server->close();
        } else if (ui->cmb_mode->currentText() == "TCP Client") {  // disconnect tcp server
            m_tcp_client->close();
        } else if (ui->cmb_mode->currentText() == "UDP") {  //
        }

    } else {
        /********** connect **********/
        if (ui->cmb_mode->currentText() == "Serial") {  // connect serial
            if (m_bDebugConnect) {
                // connect first available port @ 115200
                m_serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
                foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
                    if (!info.isBusy()) m_serial->setPortName(info.portName());
            } else {
                m_serial->setPortName(ui->cmb_serial_port->currentData().toString());
                m_serial->setBaudRate(ui->cmb_serial_baud->currentText().toUInt());
            }

            if (!m_serial->open(QIODevice::ReadWrite)) {  // 连接失败
                QMessageBox::information(this, "error", "fail to connect");
                return;
            }

        } else if (ui->cmb_mode->currentText() == "TCP Server") {  // start to listen port
            m_tcp_server->listen(QHostAddress::Any, ui->input_tcp_server_port->text().toUInt());
        } else if (ui->cmb_mode->currentText() == "TCP Client") {  // connect tcp server
            m_tcp_client->connectToHost(QHostAddress::LocalHost, ui->input_tcp_client_port->text().toUInt());
        } else if (ui->cmb_mode->currentText() == "UDP") {
        }

        m_bDebugConnect = false;
    }

    ui->cmb_mode->setEnabled(m_bConnect);  // 模式切换

    m_bConnect = !m_bConnect;
}

bool MainWindow::handleCmd(const QByteArray& recv) {
    /*********** 数据处理 ***********/

    m_cmd_buf.append(recv);

    // 对接收的字节进行计数
    m_recv_cnt += recv.length();
    m_status_recv_cnt->setText(QString("Recv:%1").arg(m_recv_cnt));

    // 显示接收的原内容
    ui->text_recv->moveCursor(QTextCursor::End);
    ui->text_recv->insertPlainText(recv);
    // 注:appendPlainText(recv) 会自动换行,所以不使用该函数

    if (!recv.contains('\n')) return false;  // 按行读取

    // 指令解析(format:##{...}##)
    {
        int start = m_cmd_buf.indexOf("##{");
        if (start == -1) return false;
        start += 3;  // length of '##{'
        int end = m_cmd_buf.indexOf("}##", start);
        if (end == -1) return false;
        m_cmd_buf = m_cmd_buf.mid(start, end - start);
        // qDebug()<<cmd;
    }

    // 指令内容 val1,val2,val3...
    QStringList list = m_cmd_buf.split(',', QString::SkipEmptyParts);
    if (list.length() == 0) return false;

    /*********** 执行指令 ***********/

    // 自动添加曲线

    static Qt::GlobalColor color_of_series[6] = {Qt::red, Qt::darkGreen, Qt::blue, Qt::darkCyan, Qt::magenta, Qt::darkYellow};  // 曲线颜色

    for (int i = m_plot->graphCount(); i < list.length(); ++i) {
        m_plot->addGraph();
        m_plot->graph(i)->setName(QString("line%1").arg(i + 1));
        m_plot->graph(i)->setPen(QPen(QColor(color_of_series[i]), 3));
    }

    // 添加数据

    for (int i = 0; i < list.length(); ++i) {
        m_plot->graph(i)->addData(m_cur_data_idx, list.at(i).toFloat());
    }

    m_plot->xAxis->setRange((m_cur_data_idx > 2000) ? (m_cur_data_idx - 2000) : 0, m_cur_data_idx);

    m_cur_data_idx++;

    /*********** 清除缓冲区 ***********/

    m_cmd_buf.clear();

    return true;
}

bool MainWindow::savefile(QString suffix, std::function<void(QTextStream&)> pFunc) {
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
