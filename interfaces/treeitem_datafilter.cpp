#include "treeitem_datafilter.h"

treeitem_datafilter::treeitem_datafilter(QTreeWidget* parent, const QString& title) : treeitem_common(parent) {
    /* father item */

    setText(0, title);
    setCheckState(0, Qt::Checked);
    setExpanded(true);

    // 双击编辑
    setFlags(flags() | Qt::ItemIsEditable);

    // 背景色
    setBackground(0, QBrush(QColor(218, 218, 218, 100)));

    /* son item */

    // 过滤器类型
    addWidget("Mode:", m_cmb_mode);
    m_cmb_mode->addItem("Include");
    m_cmb_mode->addItem("Exclude");

    // 过滤位置
    addWidget("Pos:", m_cmb_pos);
    m_cmb_pos->addItem("Prefix");
    m_cmb_pos->addItem("Suffix");
    m_cmb_pos->addItem("Contain");

    // 过滤内容
    addWidget("Content: ", m_input_content);
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
