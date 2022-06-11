#ifndef UYK_TREEITEM_OPER_H
#define UYK_TREEITEM_OPER_H

#include <QTreeWidget>
#include <QWidget>

#include <QHBoxLayout>

#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

#include "uyk_treeitem_common.h"

// oper -> operation

class uyk_treeitem_oper : public QObject, public QTreeWidgetItem {
public:
    uyk_treeitem_oper(QTreeWidget* parent, const QString& title);

public:
    QTreeWidgetItem*  m_SubItem;
    uyk_formlayout_w* m_layout;

    QPushButton* m_btn_SaveToCSV;

    QDoubleSpinBox* m_spn_min;
    QDoubleSpinBox* m_spn_max;

    QSlider* m_sld_val;
    QDial*   m_dial_val;
};

#endif  // UYK_TREEITEM_OPER_H
