#include "widget.h"
#include "ui_widget.h"

#define QStr QStringLiteral

Widget::Widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowTitle(QStr("assist"));

    initInterfaces();

    // shortcut ( show/hide tab widget )
    QShortcut* hkTabWidget = new QShortcut(QKeySequence("A"), this);
    connect(hkTabWidget, &QShortcut::activated, [&]() { ui->sidebar->setVisible(!ui->sidebar->isVisible()); });

    initUI();

    initChanTree();
}

Widget::~Widget() { delete ui; }

void Widget::initUI() {
    // always at the top
    connect(ui->chk_wnd_at_top, &QCheckBox::stateChanged, [&](int i) {
        if (i == Qt::CheckState::Checked)
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    });

    /************** tab widget **************/

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

    spl2->addWidget(ui->plot_multicurve);
    spl2->addWidget(ui->tabWidget_2);

    spl2->setSizes(QList<int>() << 1 << 0);  // 隐藏收发区

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
    },
                                                     this));

    // 发送区和接收区的右键菜单

    m_MenuOfRecv = new QMenu(this);
    m_MenuOfSend = new QMenu(this);

    // menu->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->input_recv, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfRecv->exec(QCursor::pos()); });  // 弹出菜单
    connect(ui->input_send, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfSend->exec(QCursor::pos()); });

    m_MenuOfRecv->addAction(QStr("raw data"), [&]() { m_RawDataMd = !m_RawDataMd; })->setCheckable(true);
    m_MenuOfRecv->addAction(QStr("timestamp"), [&]() { m_TimestampMd = !m_TimestampMd; })->setCheckable(true);
    m_MenuOfRecv->addSeparator();
    m_MenuOfRecv->addAction(QStr("save to txt"), [&]() { savefile("txt", [&](QTextStream& s) { s << ui->input_recv->toPlainText(); }); });
    m_MenuOfRecv->addAction(QStr("save to csv"), [&]() { savefile("csv", [&](QTextStream& s) { s << ui->input_recv->toPlainText(); }); });
    m_MenuOfRecv->addAction(QStr("clear"), [&]() { ui->input_recv->clear(); });

    m_MenuOfSend->addAction(QStr("clear"), [&]() { ui->input_send->clear(); });

    // conmands

    new uyk_treeitem_command(ui->tree_commands, "1");
}

void Widget::initChanTree() {
    // menu

    m_MenuChannel = new QMenu(this);

    m_MenuChannel->addAction("expand all", [&]() {foreach(auto i,m_channels) i->setExpanded(true); });
    m_MenuChannel->addAction("collapse all", [&]() {foreach(auto i,m_channels) i->setExpanded(false); });

    m_MenuChannel->addSeparator();
    m_MenuChannel->addAction("clear", [&]() {foreach(auto i,m_channels) i->setExpanded(false); });

    m_MenuChannel->addSeparator();
    m_MenuChannel->addAction("save all", [&]() {});

    m_MenuChannel->addAction("clear", [&]() {foreach(auto i,m_channels) i->setExpanded(false); });

    // tree

    ui->tree_channel->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tree_channel, &QTreeWidget::customContextMenuRequested, [&]() { m_MenuChannel->exec(QCursor::pos()); });
}

void Widget::initInterfaces() {
    ui->tab_interfaces->layout()->addWidget(m_interfaces = new tab_interfaces(ui->tab_interfaces));
    ui->tab_dataformat->layout()->addWidget(m_dataformat = new tab_dataformat(ui->tab_dataformat));

    // 指令包头包尾(ASCII)
    connect(m_dataformat, &tab_dataformat::prefixChanged, [&](const QString& str) { m_interfaces->m_prefix = str; });
    connect(m_dataformat, &tab_dataformat::suffixChanged, [&](const QString& str) { m_interfaces->m_suffix = str; });

    // 数据接收
    connect(m_interfaces, &tab_interfaces::recvData, [&](const QByteArray& recv) {
        // 总接收字节数
        ui->label_bytes_of_recv->setText(QString("Recv: %1").arg(m_interfaces->m_BytesOfRecv));

        // 显示接收的原内容
        if (m_RawDataMd) appendText(recv);
    });

    // 收发速率更新
    connect(m_interfaces, &tab_interfaces::update_kBps, [&](const QString& send, const QString& recv) {
        ui->label_speed_of_send->setText(send);
        ui->label_speed_of_recv->setText(recv);
    });

    // 指令接收
    connect(m_interfaces, &tab_interfaces::recvCmd, [&](const QByteArray& recv) {
        // 指令过滤器
        if (m_dataformat->filter(recv)) return;

        // 显示接收的指令
        if (!m_RawDataMd) appendText(recv);

        // tcp服务器转发
        //    if(ui->chk_tcp_server_transmit->checkState()==Qt::CheckState::Checked)
        //        sendData(m_CmdBuf);

        // 添加曲线点

        QVector<double> vals;
        QStringList     strlist = QString(recv).split(m_dataformat->delimiter(), Qt::KeepEmptyParts);
        foreach (auto s, strlist) vals.append(s.toDouble());

        ui->plot_multicurve->addValues(vals);
        ui->plot_multicurve->xAxis->setRange(
            ui->plot_multicurve->m_index > ui->plot_multicurve->xAxis->range().size() ? (ui->plot_multicurve->m_index - ui->plot_multicurve->xAxis->range().size()) : 0, ui->plot_multicurve->m_index);

        //        QStringList vals_recv;
        //        if (ui->input_dataformat_delimiter->text().isEmpty()) {
        //            vals_recv << recv;
        //        } else {
        //            vals_recv = QString(recv).split(ui->input_dataformat_delimiter->text(), QString::SkipEmptyParts);
        //        }

        //        // vals_recv.removeLast();
        //        QVector<double> vals;
        //        for (int i = 0; i < vals_recv.size(); ++i) {
        //            if (m_channels.size() < i + 1) m_channels.append(new uyk_treeitem_channel(ui->tree_channel, QStr("channel %1").arg(m_channels.size() + 1)));

        //            if (m_channels.at(i)->checkState(0) == Qt::CheckState::Checked) {
        //                vals << (vals_recv.at(i).toDouble() * m_channels.at(i)->m_spn_yScale->value() + m_channels.at(i)->m_spn_yOffset->value());
        //            } else {
        //                vals << vals_recv.at(i).toDouble();
        //            }
        //        }

        //        ui->plot->addVals(vals);
    });
}

void Widget::appendText(const QString& content) {
    if (m_TimestampMd) {
        ui->input_recv->moveCursor(QTextCursor::End);
        ui->input_recv->insertPlainText(QTime::currentTime().toString("[hh:mm:ss]"));
    }
    ui->input_recv->moveCursor(QTextCursor::End);
    ui->input_recv->insertPlainText(content);
}
