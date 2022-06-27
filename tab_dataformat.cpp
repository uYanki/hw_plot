#include "tab_dataformat.h"
#include "ui_tab_dataformat.h"

/*****************************************************************/

uyk_treeitem_datafilter::uyk_treeitem_datafilter(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
    setText(0, title);

    setCheckState(0, Qt::Checked);
    setExpanded(true);

    setBackground(0, QBrush(QColor(218, 218, 218, 100)));

    // 双击编辑
    setFlags(flags() | Qt::ItemIsEditable);

    // 表格布局
    m_subItem = new QTreeWidgetItem(this, QStringList());
    m_layout  = new uyk_formlayout_w();

    // 过滤器类型
    m_layout->add_row("Mode:", m_cmb_filterMd = new QComboBox(m_layout));
    m_cmb_filterMd->addItem("Include");
    m_cmb_filterMd->addItem("Exclude");

    // 过滤位置
    m_layout->add_row("Pos:", m_cmb_pos = new QComboBox(m_layout));
    m_cmb_pos->addItem("Prefix");
    m_cmb_pos->addItem("Suffix");
    m_cmb_pos->addItem("Contain");

    // 过滤内容
    m_layout->add_row("Content:", m_input_content = new QLineEdit(m_layout));

    parent->setItemWidget(m_subItem, 0, m_layout);
}

bool uyk_treeitem_datafilter::filter(const QString& text) {
    if (checkState(0) == Qt::Unchecked) return false;
    if (m_input_content->text().length() == 0) return false;

    bool r = false;
    switch (m_cmb_pos->currentIndex()) {
        case 0: r = text.startsWith(m_input_content->text()); break;
        case 1: r = text.endsWith(m_input_content->text()); break;
        case 2: r = text.contains(m_input_content->text()); break;
    }

    return m_cmb_filterMd->currentIndex() == 0 ? !r : r;

    // 真:需被过滤,假:不需过滤
}

/*****************************************************************/

tab_dataformat::tab_dataformat(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::tab_dataformat) {
    ui->setupUi(this);

    // dataformat

    // 指令包头包尾分割符(ASCII)
    connect(ui->input_dataformat_prefix, &QLineEdit::textChanged, [&](const QString& str) { emit prefixChanged(str);});
    connect(ui->input_dataformat_suffix, &QLineEdit::textChanged, [&](const QString& str) { emit suffixChanged(str);});
    connect(ui->input_dataformat_delimiter, &QLineEdit::textChanged, [&](const QString& str) { emit delimiterChanged(str);});

    // menu

    m_Menu = new QMenu(this);
    m_Menu->addAction("new filter", [&]() { m_datafilters.append(new uyk_treeitem_datafilter(ui->tree_datafilter, QStr("filter %1").arg(m_datafilters.size() + 1))); });

    m_Menu->addSeparator();
    m_Menu->addAction("expand all", [&]() {foreach(auto i,m_datafilters) i->setExpanded(true); });
    m_Menu->addAction("collapse all", [&]() {foreach(auto i,m_datafilters) i->setExpanded(false); });

    m_Menu->addSeparator();
    m_Menu->addAction("enable all", [&]() {foreach(auto i,m_datafilters) i->setCheckState(0,Qt::CheckState::Checked); });
    m_Menu->addAction("disable all", [&]() {foreach(auto i,m_datafilters) i->setCheckState(0,Qt::CheckState::Unchecked); });
    m_Menu->addAction("remove all", [&]() { ui->tree_datafilter->clear(); m_datafilters.clear(); });

    m_Menu->addSeparator();
    m_Menu->addAction("load", [&]() {});
    m_Menu->addAction("save", [&]() {});

    // tree

    connect(ui->chk_datafilter_enable, &QCheckBox::stateChanged, [&](int i) { ui->tree_datafilter->setEnabled(i == Qt::CheckState::Checked); });

    ui->tree_datafilter->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tree_datafilter, &QTreeWidget::customContextMenuRequested, [&]() { m_Menu->exec(QCursor::pos()); });

    // ui->tree_datafilter->setEditTriggers(QTreeWidget::EditTrigger::EditKeyPressed); // F2
}

tab_dataformat::~tab_dataformat() {
    delete ui;
}

bool tab_dataformat::filter(const QString &text)
{
    if (ui->chk_datafilter_enable->checkState() == Qt::CheckState::Checked)
        foreach(auto f,m_datafilters)
            if(f->filter(text)) return true;
    return false;
}

 QString tab_dataformat::prefix(){ return ui->input_dataformat_prefix->text();}
 QString tab_dataformat::suffix(){  return ui->input_dataformat_suffix->text();}
 QString tab_dataformat::delimiter(){  return ui->input_dataformat_delimiter->text();}
