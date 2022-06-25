#ifndef UYK_TREEITEM_DATAFILTER_H
#define UYK_TREEITEM_DATAFILTER_H

#include <QTreeWidget>
#include <QWidget>

#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>

#include "uyk_treeitem_common.h"

class uyk_treeitem_datafilter : public QObject, public QTreeWidgetItem {
public:
    uyk_treeitem_datafilter(QTreeWidget* parent, const QString& title);

private:
    QTreeWidgetItem*  m_subItem;
    uyk_formlayout_w* m_layout;

    QComboBox* m_cmb_filterMd  = nullptr;
    QComboBox* m_cmb_pos       = nullptr;
    QLineEdit* m_input_content = nullptr;

public:
    bool filter(const QString& text);
};

#endif  // UYK_TREEITEM_DATAFILTER_H
