#include "page_tcpclient.h"
#include "ui_page_tcpclient.h"

page_tcpclient::page_tcpclient(QWidget* parent) : datahandler(parent),
                                                  ui(new Ui::page_tcpclient),
                                                  m_Client(new QTcpSocket(this)) {
    ui->setupUi(this);

    ui->cmb_ser_ip->addItems(getlocalip());
    ui->input_ser_port->setValidator(new QRegExpValidator(QRegExp(LIMIT_PORT), this));

    ui->input_log->viewport()->installEventFilter(this);

    // read data

    connect(m_Client, &QTcpSocket::readyRead, [&]() { while(m_Client->bytesAvailable()){recvdata(m_Client->readLine());} });
     connect(m_Client, &QTcpSocket::connected, [&]() {ui->input_cli_port->setText(QString::number(m_Client->localPort()));});
    // connect(m_Client, &QTcpSocket::disconnected, [&](){});

    // handle error
    void (QTcpSocket::*p)(QAbstractSocket::SocketError) = &QTcpSocket::error;
    ui->input_log->setTextColor(QColor("#D64D54"));
    connect(m_Client, p, [&](QAbstractSocket::SocketError) {
        ui->input_log->append(QLatin1String("%1 %2")
                                  .arg(datahandler::timestamp())
                                  .arg(m_Client->errorString()));
        stop();
    });
}

page_tcpclient::~page_tcpclient() {
    delete ui;
}

void page_tcpclient::start() {
    // connect
    m_Client->connectToHost(ui->cmb_ser_ip->currentText(),
                            ui->input_ser_port->text().toUInt(),
                            QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

    m_Client->waitForConnected(1000);
    if (m_Client->state() == QAbstractSocket::ConnectedState) {   
        ui->cmb_ser_ip->setEnabled(false);
        ui->input_ser_port->setEnabled(false);
        datahandler::start();
    } else {
        // fail
        QMessageBox::warning(this, QLatin1String("ERROR"), QLatin1String("fail to connect server"));
        emit datahandler::runstate(false);
    }
}

void page_tcpclient::stop() {
    // disconnect
    m_Client->close();
    ui->cmb_ser_ip->setEnabled(true);
    ui->input_ser_port->setEnabled(true);
    ui->input_cli_port->setText("");
    datahandler::stop();
}

void page_tcpclient::senddata(const QByteArray& bytes) {
    // send data
    m_Client->write(bytes);
    datahandler::senddata(bytes);
}

bool page_tcpclient::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->input_log->viewport() && event->type() != QEvent::MouseButtonDblClick)
        ui->input_log->clear();  // 双击清空日志
    return datahandler::eventFilter(watched, event);
}
