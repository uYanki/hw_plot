#include "uyk_treeitem_chan.h"

uyk_treeitem_chan::uyk_treeitem_chan(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
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

    // 曲线画笔参数
    m_layout->add_row(QStr("lineColor:"), m_input_LineColor = new QLineEdit(m_layout));
    m_layout->add_row(QStr("lineWidth:"), m_spn_LineWidth = new QDoubleSpinBox(m_layout));

    // 水平分割线
    m_layout->add_hline();

    // 曲线数据保存按钮
    m_layout->add_row(QStr(""), m_btn_SaveToCSV = new QPushButton("save"));
}
