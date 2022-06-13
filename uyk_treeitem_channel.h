#ifndef UYK_TREEITEM_CHAN_H
#define UYK_TREEITEM_CHAN_H

#include <QTreeWidget>
#include <QWidget>

#include <QHBoxLayout>

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "uyk_treeitem_common.h"

// channel -> chan

class uyk_treeitem_channel : public QObject, public QTreeWidgetItem {
public:
    uyk_treeitem_channel(QTreeWidget* parent, const QString& title);

public:
    QTreeWidgetItem*  m_SubItem;
    uyk_formlayout_w* m_layout;

    QPushButton*    m_btn_SaveToCSV;
    QDoubleSpinBox* m_spn_yScale;
    QDoubleSpinBox* m_spn_yOffset;
    QCheckBox*      m_chk_visible;
};

#endif  // UYK_TREEITEM_CHAN_H
