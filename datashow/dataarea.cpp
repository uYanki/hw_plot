#include "dataarea.h"
#include "ui_dataarea.h"

#include <QDebug>

dataarea::dataarea(QWidget* parent) : QWidget(parent),
                                      ui(new Ui::dataarea),
                                      m_MenuOfRecv(new QMenu(this)),
                                      m_MenuOfSend(new QMenu(this)),
                                      m_input_recv(new input_common(ui->tabwidget_recv)) {
    ui->setupUi(this);

    // 默认宽度占比
    ui->splitter->setStretchFactor(0, 7);
    ui->splitter->setStretchFactor(1, 3);

    // 接收区选项卡

    m_input_recv->setReadOnly(true);
    m_input_recv->setPlaceholderText("waiting for data to arrive");

    ui->tabwidget_recv->add(m_input_recv, "Recv", true);

    connect(ui->tabwidget_recv, &dock_tabwidget::dblBlank, [&]() {  // 子过滤器
        bool    ok;
        QString name = QInputDialog::getText(this, "Tab", "new tab name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            auto filter = new subfilter(ui->tabwidget_recv);
            m_SubFilters.append(filter);
            ui->tabwidget_recv->add(filter, name);
            connect(filter, &subfilter::destroyed, [=]() { m_SubFilters.removeOne(filter); });
        }
    });

    // 接收区菜单

    m_input_recv->bindMenu(m_MenuOfRecv);
    m_MenuOfRecv->addAction(QLatin1String("raw data"), [&](bool b) { m_RawDataMd = b; })->setCheckable(true);
    m_MenuOfRecv->addAction(QLatin1String("timestamp"), [&](bool b) { m_TimestampMd = b; })->setCheckable(true);
    m_MenuOfRecv->addSeparator();
    m_MenuOfRecv->addAction(QLatin1String("save to txt"), [&]() { m_input_recv->saveToTXT(); });
    m_MenuOfRecv->addAction(QLatin1String("save to csv"), [&]() { m_input_recv->saveToCSV(); });
    m_MenuOfRecv->addSeparator();
    m_MenuOfRecv->addAction(QLatin1String("clear"), [&]() { m_input_recv->clear(); });

    // 发送区菜单

    connect(ui->input_send, &QPlainTextEdit::customContextMenuRequested, [&]() { m_MenuOfSend->exec(QCursor::pos()); });

    m_MenuOfSend->addAction(QLatin1String("clear"), [&]() { ui->input_send->clear(); });

    // 自动发送

    initAutoSend();

    // 历史发送记录(双击发送)

    connect(ui->list_histroy, &QListWidget::itemDoubleClicked, [&](QListWidgetItem* item) { emit senddata(item->text().toUtf8()); });
}

dataarea::~dataarea() {
    delete ui;
}

void dataarea::initAutoSend() {
    m_TmrAutoSend = new QTimer(this);

    QFormLayout* layout = new QFormLayout(m_CntrAutoSend = new QWidget(this));
    layout->addRow(QLatin1String("interval:"), m_SpnInterval = new QSpinBox(m_CntrAutoSend));  // 时间间隔
    layout->addRow(QLatin1String("times:"), m_SpnTimes = new QSpinBox(m_CntrAutoSend));        // 发送次数
    layout->setMargin(6);

#define cfgSpn(spn, min, max, val)                    \
    spn->setFocusPolicy(Qt::FocusPolicy::ClickFocus); \
    spn->setRange(min, max);                          \
    spn->setValue(val);                               \
    spn

    cfgSpn(m_SpnInterval, 1, 9999, 1000)->setSuffix(QLatin1String(" ms"));
    cfgSpn(m_SpnTimes, -1, 9999, -1)->setToolTip(QLatin1String("-1: infinity"));

    // 置右键菜单

    static int times = 0;

    ui->btn_send->setMenu(m_MenuOfSendBtn = new QMenu(this));

    QAction* actRunAutoSend;
    (actRunAutoSend = m_MenuOfSendBtn->addAction(QLatin1String("Auto send"), [=](bool b) {
        if(b){
            m_TmrAutoSend->start(m_SpnInterval->value());
            times= m_SpnTimes->value();
        }else{
            m_TmrAutoSend->stop();
        }
    }))->setCheckable(true);
    ;

    QWidgetAction* wgtAct = new QWidgetAction(this);
    wgtAct->setDefaultWidget(m_CntrAutoSend);
    m_MenuOfSendBtn->addAction(wgtAct);

    // 动态更改发送间隔

    void (QSpinBox::*p)(int) = &QSpinBox::valueChanged;
    connect(m_SpnInterval,p,[=](int v){m_TmrAutoSend->setInterval(v);});

    // 自动发送时钟

    connect(m_TmrAutoSend, &QTimer::timeout, [=]() {
        const QString& text = ui->input_send->toPlainText();
        if (text.isEmpty()) return;  // 有内容发送才计次

        if(times == -1 || times-- > 0){
            emit senddata(ui->input_send->toPlainText().toUtf8());
        }else{
            // 结束发送
            m_TmrAutoSend->stop();
            actRunAutoSend->setChecked(false);
        }

    });
}

void dataarea::readdata(const QByteArray& recv) {
    if (m_RawDataMd) m_input_recv->append(recv);
}

void dataarea::readcmd(const QString& recv) {
    if (!m_RawDataMd) {
        if (m_TimestampMd) m_input_recv->append(input_common::timestamp(), QColor("#22A3A9"));
        m_input_recv->append(recv);
    }
    foreach (auto filter, m_SubFilters) filter->recvcmd(recv);
}

void dataarea::updatestat(size_t BytesOfRecv, size_t BytesOfSend, const QString& SpeedOfRecv, const QString& SpeedOfSend) {
    ui->label_count_of_recv->setText(QString::number(BytesOfRecv));
    ui->label_count_of_send->setText(QString::number(BytesOfSend));
    ui->label_speed_of_recv->setText(SpeedOfRecv);
    ui->label_speed_of_send->setText(SpeedOfSend);
}

void dataarea::runstate(bool state) {
    ui->btn_send->setEnabled(state);
    m_TmrAutoSend->stop(); // 停止自动发送
}

void dataarea::on_btn_send_clicked() {
    const QString& text = ui->input_send->toPlainText();
    if (text.isEmpty()) return;
    emit senddata(ui->input_send->toPlainText().toUtf8());
    ui->list_histroy->insertItem(0, text);  // 加入至列表最上方
}
