#ifndef TAB_DATAFORMAT_H
#define TAB_DATAFORMAT_H

#include <QMenu>
#include <QWidget>

#include "treeitem_datafilter.h"

namespace Ui {
    class tab_dataformat;
}

class tab_dataformat : public QWidget {
    Q_OBJECT

public:
    explicit tab_dataformat(QWidget* parent = nullptr);
    ~tab_dataformat();

private:
    Ui::tab_dataformat* ui;

private:
    QMenu* const m_Menu = new QMenu(this);

    QList<treeitem_datafilter*> m_filters;

public:
    bool filter(const QByteArray& bytes);
};

#endif  // TAB_DATAFORMAT_H