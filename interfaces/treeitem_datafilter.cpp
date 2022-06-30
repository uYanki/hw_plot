#include "treeitem_datafilter.h"

treeitem_datafilter::treeitem_datafilter(QTreeWidget* parent, const QString& title) :
    QTreeWidgetItem(parent),
    m_container(new formlayout(parent)),
    m_cmb_mode(new QComboBox(m_container)),
    m_cmb_pos(new QComboBox(m_container)),
    m_input_content(new QLineEdit(m_container))
{

    /* father item */

    setText(0, title);
    setCheckState(0, Qt::Checked);
    setExpanded(true);

    // F2 编辑名称
    setFlags(flags() | Qt::ItemIsEditable);

    // 背景色
    setBackground(0, QBrush(QColor(218, 218, 218, 100)));

    /* son item */

    // 过滤器类型
    m_container->addWidget(QLatin1String("Mode:"), m_cmb_mode);
    m_cmb_mode->addItem(QLatin1String("Include"));
    m_cmb_mode->addItem(QLatin1String("Exclude"));

    // 过滤位置
    m_container->addWidget(QLatin1String("Pos:"), m_cmb_pos);
    m_cmb_pos->addItem(QLatin1String("Prefix"));
    m_cmb_pos->addItem(QLatin1String("Suffix"));
    m_cmb_pos->addItem(QLatin1String("Contain"));

    // 过滤内容
    m_container->addWidget(QLatin1String("Content: "), m_input_content);

    /* show */

    auto sub = new QTreeWidgetItem(this, QStringList());
    parent->setItemWidget(sub,0,m_container);

}

bool treeitem_datafilter::filter(const QString& text) {
    if (checkState(0) == Qt::Unchecked) return false;
    if (m_input_content->text().length() == 0) return false;

    bool b = false;
    switch (m_cmb_pos->currentIndex()) {
        case 0: b = text.startsWith(m_input_content->text()); break;
        case 1: b = text.endsWith(m_input_content->text()); break;
        case 2: b = text.contains(m_input_content->text()); break;
    }

    return m_cmb_mode->currentIndex() == 0 ? !b : b;

    // 真:需被过滤,假:不需过滤
}
