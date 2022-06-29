#include "tab_dataformat.h"
#include "ui_tab_dataformat.h"

tab_dataformat::tab_dataformat(QWidget* parent) : QWidget(parent), ui(new Ui::tab_dataformat) {
    ui->setupUi(this);

    // menu

    m_Menu->addAction("new filter", [&]() { m_filters.append(new treeitem_datafilter(ui->tree_filters, QStringLiteral("filter %1").arg(m_filters.size() + 1))); });

    m_Menu->addSeparator();
    m_Menu->addAction("expand all", [&]() {foreach(auto i,m_filters) i->setExpanded(true); });
    m_Menu->addAction("collapse all", [&]() {foreach(auto i,m_filters) i->setExpanded(false); });

    m_Menu->addSeparator();
    m_Menu->addAction("enable all", [&]() {foreach(auto i,m_filters) i->setCheckState(0,Qt::CheckState::Checked); });
    m_Menu->addAction("disable all", [&]() {foreach(auto i,m_filters) i->setCheckState(0,Qt::CheckState::Unchecked); });
    m_Menu->addAction("remove all", [&]() { ui->tree_filters->clear(); m_filters.clear(); });

    m_Menu->addSeparator();
    m_Menu->addAction("load", [&]() {});
    m_Menu->addAction("save", [&]() {});

    // tree widget

    connect(ui->chk_enable_filter, &QCheckBox::stateChanged, [&](int i) { ui->tree_filters->setEnabled(i == Qt::CheckState::Checked); });

    ui->tree_filters->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tree_filters, &QTreeWidget::customContextMenuRequested, [&]() { m_Menu->exec(QCursor::pos()); });
}

tab_dataformat::~tab_dataformat() { delete ui; }

bool tab_dataformat::filter(const QByteArray& bytes) {
    if (ui->chk_enable_filter->checkState() == Qt::CheckState::Checked)
        foreach (auto i, m_filters)
            if (i->filter(bytes)) return true;
    return false;
}
