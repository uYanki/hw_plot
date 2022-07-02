#include "page_serialport.h"
#include "ui_page_serialport.h"

void delay_ms(unsigned int msec) {
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

page_serialport::page_serialport(QWidget* parent) : datahandler(parent),
                                                    ui(new Ui::page_serialport),
    m_SerialPort(new QSerialPort(this)){
    ui->setupUi(this);

    ui->label_port_desc->setVisible(false);

    // default params
    m_SerialPort->setBaudRate(QSerialPort::BaudRate::Baud115200);
    m_SerialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_SerialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_SerialPort->setParity(QSerialPort::Parity::NoParity);
    m_SerialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    // configure params

    void (QComboBox::*p)(int) = &QComboBox::currentIndexChanged;
    connect(ui->cmb_port, p, [&](int) { ui->label_port_desc->setText(ui->cmb_port->currentData().toString()); });
    connect(ui->cmb_baudrate, p, [&](int) { m_SerialPort->setBaudRate(ui->cmb_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_databits, p, [&](int) { m_SerialPort->setDataBits((QSerialPort::DataBits)(ui->cmb_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_parity, p, [&](int i) { m_SerialPort->setParity((QSerialPort::Parity)((i == 0) ? 0 : (i + 1))); });                       // 校验位
    connect(ui->cmb_flowcontrol, p, [&](int i) { m_SerialPort->setFlowControl((QSerialPort::FlowControl)i); });                               // 数据流控
    connect(ui->cmb_stopbits, p, [&](int i) {
        switch (i) {  //停止位
            case 0: m_SerialPort->setStopBits(QSerialPort::OneStop); break;
            case 1: m_SerialPort->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: m_SerialPort->setStopBits(QSerialPort::TwoStop); break;
        }
    });

    // set flow control signals
    connect(ui->chk_signal_DTR, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setDataTerminalReady(i == Qt::CheckState::Checked); });
    connect(ui->chk_signal_RTS, &QCheckBox::stateChanged, [&](int i) { m_SerialPort->setRequestToSend(i == Qt::CheckState::Checked); });

    // scan serial port
    scan();
    ui->cmb_port->installEventFilter(this);

    // read data
    connect(m_SerialPort, &QSerialPort::readyRead, [&]() { recvdata(m_SerialPort->readLine()); });

    // handle error
    connect(m_SerialPort, &QSerialPort::errorOccurred, [&](QSerialPort::SerialPortError e) {
        if (e != QSerialPort::SerialPortError::NoError) {
            m_SerialPort->clearError();
            stop();
            // 延时再扫描
            delay_ms(1000);
            scan();
        }
    });
}

page_serialport::~page_serialport() { delete ui; }

void page_serialport::start() {
    // open serialport
    if (ui->cmb_port->currentIndex()== -1) {
        emit datahandler::runstate(false);
    }else{
        m_SerialPort->setPortName(ui->cmb_port->currentText());
        if (m_SerialPort->open(QIODevice::ReadWrite)) {
            ui->cmb_port->setEnabled(false);
            ui->chk_signal_DTR->setEnabled(true);
            ui->chk_signal_RTS->setEnabled(true);
            datahandler::start();
        }else{
            // fail
            emit datahandler::runstate(false);
            QMessageBox::warning(this, QLatin1String("ERROR"), QLatin1String("fail to open serial port"));
        }
    }
}

void page_serialport::stop() {
    // close serialport
    if (!m_SerialPort->isOpen()) return;
    m_SerialPort->close();
    ui->cmb_port->setEnabled(true);
    ui->chk_signal_DTR->setEnabled(false);
    ui->chk_signal_RTS->setEnabled(false);
    datahandler::stop();
}

void page_serialport::senddata(const QByteArray& bytes) {
    // send data
    m_SerialPort->write(bytes);
    datahandler::senddata(bytes);
}

void page_serialport::scan() {
    // scan serialport
    ui->cmb_port->clear();
    auto infos = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo& info, infos)
        ui->cmb_port->addItem(info.portName(), info.description());
    ui->label_port_desc->setVisible(infos.count() != 0);  // 串口描述
}

bool page_serialport::eventFilter(QObject* watched, QEvent* event) {
    // scan available serial ports when hover enter
    if (watched == ui->cmb_port && event->type() == QEvent::HoverEnter) scan();
    return datahandler::eventFilter(watched, event);
}
