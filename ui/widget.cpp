#include "widget.h"
#include "ui_widget.h"



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_Serial    = new QSerialPort(this);
    m_TcpServer = new QTcpServer(this);
    m_TcpClient = new QTcpSocket(this);
    m_Udp       = new QUdpSocket(this);


    initUI();
    initVal();

}

Widget::~Widget(){ delete ui;}



void Widget::initUI()
{
    /************** window **************/

    setWindowTitle("uykAssist");

    /************** tabbar_interface **************/

    ui->tabbar_interface->setCurrentIndex(0);
    ui->tabbar_interface->tabBar()->hide();// 隐藏夹头

    connect(ui->cmb_interface, SIGNAL(currentIndexChanged(int)), ui->tabbar_interface, SLOT(setCurrentIndex(int)));

    /************** [interface] serial **************/

    // 串口扫描
    ui->cmb_serial_port->installEventFilter(this);

    // 串口描述
    ui->label_serial_port_desc->setVisible(false);
    connect(ui->cmb_serial_port, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { ui->label_serial_port_desc->setText(ui->cmb_serial_port->currentData().toString()); });

    // 参数设置
    connect(ui->cmb_serial_baudrate, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setBaudRate(ui->cmb_serial_baudrate->currentText().toInt()); });                           // 波特率
    connect(ui->cmb_serial_databits, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int) { m_Serial->setDataBits((QSerialPort::DataBits)(ui->cmb_serial_databits->currentText().toInt())); });  //数据位
    connect(ui->cmb_serial_parity, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setParity((QSerialPort::Parity)i); });                 // 校验位
    connect(ui->cmb_serial_flowcontrol, pSIGNAL_COMBOBOX_INDEX_CHANGE, [&](int i) { m_Serial->setFlowControl((QSerialPort::FlowControl)i); });  // 数据流控
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
    QAction* actRepeatSend = m_MenuOfSendBtn->addAction(QStringLiteral("Continuous sending"));
    actRepeatSend->setCheckable(true);
    connect(actRepeatSend,&QAction::toggled,[&](bool i){m_CntrRepeatSend->setStyleSheet(QString("color:%1;").arg(i?"black":"gray"));});

    m_MenuOfSendBtn->addAction(new uyk_action([&](QWidget* parent)->QWidget*{
       m_CntrRepeatSend =  new QWidget(parent);
       m_CntrRepeatSend->setStyleSheet("color:gray;");
       QFormLayout * layout = new QFormLayout(m_CntrRepeatSend);
       layout->setMargin(6);
       // 发送延时
       layout->addRow(QStringLiteral("delay:"),m_SpnRepeatDelay=new QSpinBox(m_CntrRepeatSend));
       m_SpnRepeatDelay->setRange(0,99999);
       m_SpnRepeatDelay->setSuffix(" ms"); // 后缀
       m_SpnRepeatDelay->setValue(1000); // 默认延时
       // 发送次数
       layout->addRow(QStringLiteral("times:"),m_SpnRepeatTimes=new QSpinBox(m_CntrRepeatSend));
       m_SpnRepeatTimes->setToolTip(QStringLiteral("-1: infinity"));
       m_SpnRepeatTimes->setRange(-1,9999);
       return m_CntrRepeatSend;
   },this));


    // 发送区和接收区的宽度调整

    QSplitter* spl1= new QSplitter(Qt::Orientation::Horizontal,this);
    ui->hbox_data->addWidget(spl1);
    spl1->setHandleWidth(1);// 分割条宽度

    spl1->addWidget(ui->group_recv);
    spl1->addWidget(ui->group_send);
    spl1->setStretchFactor(0,3); // 初始比例
    spl1->setStretchFactor(1,2);

    // 发送区和接收区的右键菜单

    m_MenuOfRecv = new QMenu(this);
    m_MenuOfSend = new QMenu(this);

    // menu->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect( ui->input_recv, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfRecv->exec(QCursor::pos()); });  // 弹出菜单
    connect( ui->input_send, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfSend->exec(QCursor::pos()); });

    m_MenuOfRecv->addAction(QStringLiteral("raw data"), [&]() { ui->input_recv->clear(); })->setCheckable(true);
    m_MenuOfRecv->addAction(QStringLiteral("timestamp"), [&]() { ui->input_recv->clear(); })->setCheckable(true);
    m_MenuOfRecv->addSeparator();
    m_MenuOfRecv->addAction(QStringLiteral("save to txt"), [&]() { });
    m_MenuOfRecv->addAction(QStringLiteral("save to csv"), [&]() { });
    m_MenuOfRecv->addAction(QStringLiteral("clear"), [&]() { ui->input_recv->clear(); });

    m_MenuOfSend->addAction(QStringLiteral("clear"), [&]() { ui->input_send->clear(); });

    /******** channel tree *******/

    ui->tree_channel->setRootIsDecorated(true);
    ui->tree_channel->setHeaderHidden(true);
    ui->tree_channel->setIndentation(0);

    new uyk_treeitem_chan(ui->tree_channel,"chan1");
    new uyk_treeitem_chan(ui->tree_channel,"chan2");
    new uyk_treeitem_chan(ui->tree_channel,"chan3");
    new uyk_treeitem_chan(ui->tree_channel,"chan4");
    new uyk_treeitem_chan(ui->tree_channel,"chan5");
    new uyk_treeitem_chan(ui->tree_channel,"chan6");
    new uyk_treeitem_chan(ui->tree_channel,"chan7");
    new uyk_treeitem_chan(ui->tree_channel,"chan8");


    /******** plot & oper *******/

    ui->group_oper->setVisible(false);

    // 图表区和操作区的宽度调整

    QSplitter* spl2= new QSplitter(Qt::Orientation::Horizontal,this);
    ui->hbox_plot->addWidget(spl2);
    spl2->setHandleWidth(1);// 分割条宽度

    spl2->addWidget(ui->group_plot);
    spl2->setCollapsible(0,false); // 禁止完全隐藏
    spl2->addWidget(ui->group_oper);

    spl2->setStretchFactor(0,6); // 初始比例
    spl2->setStretchFactor(1,1);









}

void Widget::initVal()
{

}

