#include "uyk_treeitem_channel.h"

uyk_treeitem_channel::uyk_treeitem_channel(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
    setText(0, title);

    setCheckState(0, Qt::Checked);
    setExpanded(true);

    // 表格布局
    m_SubItem = new QTreeWidgetItem(this, QStringList());
    parent->setItemWidget(m_SubItem, 0, m_layout = new uyk_formlayout_w());

    // 曲线增益
    m_spn_yScale = m_layout->add_dspn(QStr("yScale:"), 1.0);

    // 水平分割线
    m_layout->add_hline();

    // 显示/隐藏

   m_layout->add_row(QStr("Visible:"), m_chk_visible = new QCheckBox(m_layout));

   // 水平分割线
   m_layout->add_hline();

    // 曲线数据保存按钮
    m_layout->add_row(QStr(""), m_btn_SaveToCSV = new QPushButton(QStr("save"),m_layout));

}
