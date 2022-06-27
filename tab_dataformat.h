#ifndef TAB_DATAFORMAT_H
#define TAB_DATAFORMAT_H

#include <QWidget>

/*****************************************************************/

#include <QTreeWidget>

#include <QCheckBox>
#include <QComboBox>
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

/*****************************************************************/

#include <QMenu>

namespace Ui {
    class tab_dataformat;
}

// 数据过滤器

class tab_dataformat : public QWidget {
    Q_OBJECT

public:
    explicit tab_dataformat(QWidget* parent = nullptr);
    ~tab_dataformat();
private:
    Ui::tab_dataformat* ui;

private:
    QMenu* m_Menu = nullptr;
    QVector<uyk_treeitem_datafilter*> m_datafilters;

public:
    bool filter(const QString& text);

     QString prefix() ;
     QString suffix() ;
     QString delimiter() ;

signals:
    void prefixChanged(const QString& str);
    void suffixChanged(const QString& str);
    void delimiterChanged(const QString& str);

};

#endif  // TAB_DATAFORMAT_H
