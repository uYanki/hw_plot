#ifndef DATAFILTER_H
#define DATAFILTER_H

#include <QTreeWidget>

#include <QComboBox>
#include <QLineEdit>

#include "savefile.h"
#include "formlayout.h"
#include <QDebug>

class treeitem_datafilter : public QObject, public QTreeWidgetItem {
public:
    treeitem_datafilter(QTreeWidget* parent, const QString& title);

private:
    // 构造函数在初始化成员变量后调用
    formlayout* const m_container;

public:
    QComboBox* const m_cmb_mode     ;
    QComboBox* const m_cmb_pos     ;
    QLineEdit* const m_input_content ;

    bool filter(const QString& text);

};

#endif  // DATAFILTER_H
