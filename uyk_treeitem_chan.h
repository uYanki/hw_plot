#ifndef UYK_TREEITEM_CHAN_H
#define UYK_TREEITEM_CHAN_H

#include <QTreeWidget>
#include <QWidget>

#include <QHBoxLayout>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "uyk_treeitem_common.h"

// channel -> chan

class uyk_treeitem_chan : public QObject, public QTreeWidgetItem {
public:
    uyk_treeitem_chan(QTreeWidget* parent, const QString& title);

public:
    QTreeWidgetItem*  m_SubItem;
    uyk_formlayout_w* m_layout;

    QPushButton*    m_btn_SaveToCSV;
    QDoubleSpinBox* m_spn_yScale;

    QLineEdit*      m_input_LineColor;
    QDoubleSpinBox* m_spn_LineWidth;
};

#endif  // UYK_TREEITEM_CHAN_H
