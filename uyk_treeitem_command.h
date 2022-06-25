#ifndef UYK_TREEITEM_COMMAND_H
#define UYK_TREEITEM_COMMAND_H

#include <QTreeWidget>
#include <QWidget>

#include <QTimer>
#include <QMenu>
#include <QListWidget>
#include <QCheckBox>
#include <QSpinBox>

#include "uyk_treeitem_common.h"

class uyk_treeitem_command : public QObject, public QTreeWidgetItem {
public:
    uyk_treeitem_command(QTreeWidget* parent, const QString& title);

public:
    QTreeWidgetItem*  m_subItem;
    uyk_formlayout_w* m_layout;


    QSpinBox* m_spn_times = nullptr;
    QListWidget* m_list_cmds = nullptr;

    QMenu* m_menu = nullptr;


};


#endif // UYK_TREEITEM_COMMAND_H
