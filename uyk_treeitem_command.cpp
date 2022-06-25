#include "uyk_treeitem_command.h"

uyk_treeitem_command::uyk_treeitem_command(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {

    setText(0, title);

    setCheckState(0, Qt::Unchecked);
    setExpanded(true);

    setBackground(0, QBrush(QColor(218, 218, 218, 100)));

    // 双击编辑
    setFlags(flags() | Qt::ItemIsEditable);

    // 表格布局
    m_subItem = new QTreeWidgetItem(this, QStringList());
    m_layout  = new uyk_formlayout_w();

    parent->setItemWidget(m_subItem, 0, m_layout);

    //

    m_layout->add_row(QStr("times:"),m_spn_times = new QSpinBox(m_layout));

    m_layout->add_hline();

    m_layout->add_row(m_list_cmds = new QListWidget(m_layout));
    m_list_cmds->addItems(QStringList()<<"1"<<"2");

    // 右键菜单
    m_menu = new QMenu(m_layout);
    m_list_cmds->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(m_list_cmds,&QListWidget::customContextMenuRequested,[&](){m_menu->exec(QCursor::pos());});

    m_menu->addAction("new command",[&](){});
    m_menu->addAction("new delay",[&](){
        QListWidgetItem* item= new QListWidgetItem(m_list_cmds);

        m_list_cmds->addItem(item);
    });
    m_menu->addSeparator();
    m_menu->addAction("clear",[&](){m_list_cmds->clear();});

}
