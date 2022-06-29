#include "page_tcpserver.h"
#include "ui_page_tcpserver.h"

page_tcpserver::page_tcpserver(QWidget* parent) : datahandler(parent), m_Server(new QTcpServer(this)), ui(new Ui::page_tcpserver) {
    ui->setupUi(this);

    // get local ip
    QHostInfo host = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress address, host.addresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol){
            ui->label_local_ip->setText(address.toString());
            break;
        }

     ui->label_local_ip->installEventFilter(this);



}


bool page_tcpserver::start() {
    return datahandler::start();
}

void page_tcpserver::stop() {
    datahandler::stop();
}

void page_tcpserver::senddata(const QByteArray& bytes) {
    return datahandler::senddata(bytes);
}


bool page_tcpserver::eventFilter(QObject* watched, QEvent* event) {
    // copy local ip to clipboard
    if (watched == ui->label_local_ip) {
        // 左键双击复制本地IP至剪辑版
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->button() == Qt::LeftButton) {
                QApplication::clipboard()->setText(ui->label_local_ip->text());
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}


page_tcpserver::~page_tcpserver() {
    delete ui;
}
