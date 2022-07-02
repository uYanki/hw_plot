#include "page_tcpserver.h"
#include "ui_page_tcpserver.h"

page_tcpserver::page_tcpserver(QWidget* parent) : datahandler(parent),
                                                  ui(new Ui::page_tcpserver),
                                                  m_Menu(new QMenu(this)),
                                                  m_Server(new QTcpServer(this)) {
    ui->setupUi(this);

    ui->cmb_local_ip->addItems(getlocalip());
    ui->input_port->setValidator(new QRegExpValidator(QRegExp(LIMIT_PORT), this));

    ui->input_log->viewport()->installEventFilter(this);

    // menu

    ui->list_clients->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->list_clients, &QWidget::customContextMenuRequested, [&]() { m_Menu->exec(QCursor::pos()); });

    auto enumItems = [&](std::function<void(QCheckBox * chk, int idx)> cbk) {
        for (int i = ui->list_clients->count() - 1; i >= 0; --i)
            cbk(dynamic_cast<QCheckBox*>(ui->list_clients->itemWidget(ui->list_clients->item(i))), i);
    };  // 需值传递

    m_Menu->addAction(QLatin1String("select all"), [enumItems]() { enumItems([](QCheckBox* chk, int) { chk->setCheckState(Qt::Checked); }); });
    m_Menu->addAction(QLatin1String("unselect all"), [enumItems]() { enumItems([](QCheckBox* chk, int) { chk->setCheckState(Qt::Unchecked); }); });
    m_Menu->addAction(QLatin1String("reverse all"), [enumItems]() { enumItems([](QCheckBox* chk, int) { chk->setCheckState(chk->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked); }); });

    m_Menu->addSeparator();

    m_Menu->addAction(QLatin1String("disconnect selected"), [this, enumItems]() { enumItems([&](QCheckBox* chk, int i) { if (chk->checkState() == Qt::Checked) m_Clients.at(i)->close(); }); });
    m_Menu->addAction(QLatin1String("disconnect unselected"), [this, enumItems]() { enumItems([&](QCheckBox* chk, int i) { if (chk->checkState() == Qt::Unchecked) m_Clients.at(i)->close(); }); });
    m_Menu->addAction(QLatin1String("disconnect all"), [&]() { for(int i = ui->list_clients->count()-1 ;i>=0;--i) m_Clients.at(i)->close(); });

    // handle client

    connect(m_Server, &QTcpServer::newConnection, [&]() {
        while (m_Server->hasPendingConnections()) {
            // new client

            QTcpSocket* client = m_Server->nextPendingConnection();
            m_Clients.append(client);

            // addr:port

            QString info = QLatin1String("%1:%2")
                               .arg(client->peerAddress().toString())  // addr
                               .arg(client->peerPort());               // port

            // append to list

            QListWidgetItem* item = new QListWidgetItem(ui->list_clients);
            ui->list_clients->addItem(item);
            QCheckBox* chk = new QCheckBox(info, this);
            chk->setCheckState(Qt::Checked);

            ui->list_clients->setItemWidget(item, chk);

            // log
            ui->input_log->setTextColor(QColor("#22A3A9"));
            ui->input_log->append(QString("%1 %2 %3").arg(datahandler::timestamp()).arg(info).arg(QLatin1String("connect")));

            /* 注: 以下 client 不可使用引用捕获, 需要进行值捕获, 因为退出该作用域时, client 会被释放, 再使用 client 会导致程序崩溃 */

            // read data
            connect(client, &QTcpSocket::readyRead, [this, client, chk]() {
                while (client->bytesAvailable()) {
                    if (chk->checkState() == Qt::Checked) {
                        recvdata(client->readLine());
                    } else {
                        client->readAll();  // 丢掉数据
                    }
                }
            });

            // detect disconnection (掉线检测)
            connect(client, &QTcpSocket::disconnected, [this, client, chk]() {
                // remove
                int idx = m_Clients.indexOf(client);
                delete ui->list_clients->takeItem(idx);
                m_Clients.takeAt(idx)->deleteLater();  // 类似 go 中的 defer

                // statistics
                ui->tabWidget->setTabText(0, QLatin1String("Clients(%1)").arg(QString::number(m_Clients.count())));

                // log
                ui->input_log->setTextColor(QColor("#753775"));
                ui->input_log->append(QString("%1 %2 %3").arg(datahandler::timestamp()).arg(chk->text()).arg(QLatin1String("disconnect")));
                delete chk;
            });
        }

        // statistics
        ui->tabWidget->setTabText(0, QLatin1String("Clients(%1)").arg(QString::number(m_Clients.count())));
    });

    // handle client

    connect(m_Server, &QTcpServer::acceptError, [&]() {
        ui->input_log->setTextColor(QColor("#D64D54"));
        ui->input_log->append(QString("%1 %2").arg(datahandler::timestamp()).arg(m_Server->errorString()));
        stop();
    });
}

void page_tcpserver::start() {
    // run server
    if (m_Server->listen(QHostAddress(ui->cmb_local_ip->currentText()),
                         ui->input_port->text().toUInt())) {
        ui->cmb_local_ip->setEnabled(false);
        ui->input_port->setEnabled(false);
        datahandler::start();
    } else {
        // fail
        QMessageBox::warning(this, QLatin1String("ERROR"), QLatin1String("fail to listen port"));
        emit datahandler::runstate(false);
    }
}

void page_tcpserver::stop() {
    // stop server
    foreach (auto client, m_Clients)
        client->close();  // remove clients
    m_Clients.clear();
    m_Server->close();
    ui->cmb_local_ip->setEnabled(true);
    ui->input_port->setEnabled(true);
    datahandler::stop();
}

void page_tcpserver::senddata(const QByteArray& bytes) {
    int i = 0;
    foreach (auto client, m_Clients) {
        QCheckBox* chk = dynamic_cast<QCheckBox*>(ui->list_clients->itemWidget(ui->list_clients->item(i++)));
        if (chk->checkState() == Qt::Checked) {
            client->write(bytes);
            datahandler::senddata(bytes);
        }
    }
}

page_tcpserver::~page_tcpserver() { delete ui; }

void page_tcpserver::on_cmb_local_ip_currentIndexChanged(const QString& arg1) { QApplication::clipboard()->setText(arg1); }

bool page_tcpserver::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->input_log->viewport() && event->type() != QEvent::MouseButtonDblClick)
        ui->input_log->clear();  // 双击清空日志
    return datahandler::eventFilter(watched, event);
}
