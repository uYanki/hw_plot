#include "uyk_treeitem_datafilter.h"

uyk_treeitem_datafilter::uyk_treeitem_datafilter(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
    setText(0, title);

    setCheckState(0, Qt::Checked);
    setExpanded(true);

    setBackground(0,QBrush(QColor(218,218,218,100)));

    // 双击编辑
    setFlags(flags()| Qt::ItemIsEditable);

    // 表格布局
    m_subItem = new QTreeWidgetItem(this, QStringList());
    m_layout = new uyk_formlayout_w();

    // 过滤器类型
    m_layout->add_row("Mode:",m_cmb_filterMd = new QComboBox(m_layout));
    m_cmb_filterMd->addItem("Include");
    m_cmb_filterMd->addItem("Exclude");

    // 过滤位置
    m_layout->add_row("Pos:",m_cmb_pos = new QComboBox(m_layout));
    m_cmb_pos->addItem("Prefix");
    m_cmb_pos->addItem("Suffix");
    m_cmb_pos->addItem("Contain");

    // 过滤内容
    m_layout->add_row("Content:",m_input_content = new QLineEdit(m_layout));

    parent->setItemWidget(m_subItem, 0, m_layout);

}

bool uyk_treeitem_datafilter::filter(const QString &text)
{
    if(checkState(0) == Qt::Unchecked) return false;
    if(m_input_content->text().length()==0) return false;

    bool r=false;
    switch (m_cmb_pos->currentIndex()){
       case 0: r = text.startsWith(m_input_content->text());break;
       case 1: r = text.endsWith(m_input_content->text());break;
       case 2: r = text.contains(m_input_content->text());break;
    }

    return m_cmb_filterMd->currentIndex()==0?!r:r;

    // 真：需被过滤，假：不需过滤
}
