#include "tab_dataformat.h"
#include "ui_tab_dataformat.h"

#include <QDebug>
#include <QLineEdit>

tab_dataformat::tab_dataformat(QWidget* parent) : QWidget(parent), ui(new Ui::tab_dataformat) {
    ui->setupUi(this);

    // menu

    m_Menu->addAction(QLatin1String("new filter"), [&]() { m_filters.append(new treeitem_datafilter(ui->tree_filters, QString("filter %1").arg(m_filters.size() + 1))); });

    m_Menu->addSeparator();
#define ReverseCheckState(state) ((state==Qt::Checked)? Qt::Unchecked:Qt::Checked)
    m_Menu->addAction(QLatin1String("selected all"), [&]() {foreach(auto i,m_filters) i->setCheckState(0,Qt::CheckState::Checked); });
    m_Menu->addAction(QLatin1String("unselected all"), [&]() {foreach(auto i,m_filters) i->setCheckState(0,Qt::CheckState::Unchecked); });
    m_Menu->addAction(QLatin1String("reverse all"), [&]() {foreach(auto i,m_filters) i->setCheckState(0, ReverseCheckState(i->checkState(0)));});

    m_Menu->addSeparator();

    m_Menu->addAction(QLatin1String("expand all"), [&]() {foreach(auto i,m_filters) i->setExpanded(true); });
    m_Menu->addAction(QLatin1String("collapse all"), [&]() {foreach(auto i,m_filters) i->setExpanded(false); });

    auto expandItems = [&](bool expand,Qt::CheckState state){
        foreach(auto i,m_filters)
            if(i->checkState(0) == state)
                i->setExpanded(expand);
    };

    m_Menu->addAction(QLatin1String("expand selected"), [expandItems]() {expandItems( true,Qt::Checked); });
    m_Menu->addAction(QLatin1String("collapse selected"), [expandItems]() {expandItems( false,Qt::Checked); });

    m_Menu->addAction(QLatin1String("expand unselected"), [expandItems]() {expandItems( true,Qt::Unchecked); });
    m_Menu->addAction(QLatin1String("collapse unselected"), [expandItems]() {expandItems( false,Qt::Unchecked); });

    m_Menu->addSeparator();
    m_Menu->addAction(QLatin1String("remove all"), [&]() { ui->tree_filters->clear(); m_filters.clear(); });

    auto removeItems = [&](Qt::CheckState state){
        for(int i=m_filters.count()-1;i>=0;--i){
            auto p = m_filters.at(i);
            if(p->checkState(0) == state)
                 m_filters.takeAt(i)->deleteLater();
        }
    };

    m_Menu->addAction(QLatin1String("remove selected"), [removeItems](){removeItems(Qt::Checked);});
    m_Menu->addAction(QLatin1String("remove unselected"),  [removeItems](){removeItems(Qt::Unchecked);});

    m_Menu->addSeparator();
    m_Menu->addAction(QLatin1String("load"), [&]() {});
    m_Menu->addAction(QLatin1String("save"), [&]() {});

    // tree widget

    connect(ui->chk_enable_filter, &QCheckBox::stateChanged, [&](int i) { ui->tree_filters->setEnabled(i == Qt::CheckState::Checked); });

    ui->tree_filters->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tree_filters, &QTreeWidget::customContextMenuRequested, [&]() { m_Menu->exec(QCursor::pos()); });
}

tab_dataformat::~tab_dataformat() { delete ui; }

QByteArray substr(const QByteArray& content, const QString& left, const QString& right) {
    int start, end;
    if ((start = content.indexOf(left)) == -1) return "";
    start += left.length();
    if ((end = right.isEmpty() ? content.length() : content.indexOf(right)) == -1) return "";
    return content.mid(start, end - start);
}

void tab_dataformat::readline(const QByteArray& bytes)
{
    const QString& prefix =  ui->input_prefix->text();
    const QString& suffix =  ui->input_suffix->text();

    if(prefix.isEmpty() && suffix.isEmpty()){
        // 减少深度拷贝次数
        if(!filter(bytes)) {
            emit readcmd(bytes);
            handleCmd(bytes);
        }
    }else{
        const QByteArray& cmd = substr(bytes, prefix, suffix);
        if(!cmd.isEmpty() && !filter(cmd)) {
            emit readcmd(cmd);
            handleCmd(cmd);
        }
    }
}

bool tab_dataformat::filter(const QByteArray& bytes) {
    if (ui->chk_enable_filter->checkState() == Qt::CheckState::Checked)
        foreach (auto i, m_filters)
            if (i->filter(bytes)) return true;
    return false;
}

void tab_dataformat::handleCmd(const QByteArray &bytes){

    QVector<double> values;

    if(!ui->input_delimiter->text().isEmpty()){
        auto vals = QString::fromLatin1(bytes).split(ui->input_delimiter->text(), Qt::KeepEmptyParts);
        foreach(auto val,vals) values.append(val.toDouble());
    }else{
        values<< bytes.toDouble();
    }

    emit readvals(values);

}
