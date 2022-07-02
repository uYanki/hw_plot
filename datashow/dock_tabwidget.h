#ifndef DOCK_TABWIDGET_H
#define DOCK_TABWIDGET_H

#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>

#include <QTabWidget>

#include "dock_popup.h"
#include "dock_tabbar.h"

class dock_tabwidget : public QTabWidget {
    Q_OBJECT
public:
    explicit dock_tabwidget(QWidget* parent = nullptr);

private:
    dock_tabbar* const m_tabbar;

    QList<QWidget*> m_FixedTabs;

public:
    int add(QWidget* widget, const QString& title, bool fixed = false);

signals:
    void dblBlank(void);  // 双击tabbar右边的空白处

private slots:
    void dargtab(int index);

protected:
    bool event(QEvent*) override;
};

#endif  // DOCK_TABWIDGET_H
