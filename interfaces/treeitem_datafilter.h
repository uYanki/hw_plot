#ifndef DATAFILTER_H
#define DATAFILTER_H

#include <QComboBox>
#include <QLineEdit>

#include "savefile.h"
#include "treeitem_common.h"

class treeitem_datafilter : public treeitem_common {
public:
    treeitem_datafilter(QTreeWidget* parent, const QString& title);

public:
    QComboBox* const m_cmb_mode      = new QComboBox(container);
    QComboBox* const m_cmb_pos       = new QComboBox(container);
    QLineEdit* const m_input_content = new QLineEdit(container);

    bool filter(const QString& text);
};

#endif  // DATAFILTER_H
