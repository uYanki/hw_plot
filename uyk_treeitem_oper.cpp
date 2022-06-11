#include "uyk_treeitem_oper.h"

uyk_treeitem_oper::uyk_treeitem_oper(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
    setText(0, title);
    setExpanded(true);

    // 表格布局
    m_SubItem = new QTreeWidgetItem(this, QStringList());
    parent->setItemWidget(m_SubItem, 0, m_layout = new uyk_formlayout_w());

    // 数值调试
    m_spn_min = m_layout->add_dspn(QStr("min:"), 0.0);
    m_spn_max = m_layout->add_dspn(QStr("max:"), 4096.0);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_sld_val = new QSlider(m_layout));
    hlayout->addWidget(m_dial_val = new QDial(m_layout));
    m_layout->add_row(QStr("value:"), hlayout);
}
