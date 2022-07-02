#include "dock_tabbar.h"

dock_tabbar::dock_tabbar(QWidget* parent) : QTabBar(parent) {
    setMovable(true);
    setTabsClosable(true);
}

void dock_tabbar::mousePressEvent(QMouseEvent* event) {
    m_bDragTab = (event->button() == Qt::LeftButton) && (currentIndex() >= 0);
    QTabBar::mousePressEvent(event);
}

void dock_tabbar::mouseMoveEvent(QMouseEvent* event) {
    // qDebug()<<event->button(); // NoButton
    // qDebug()<<event->buttons(); // LeftButton

    if (m_bDragTab && event->buttons()) {
        if (!contentsRect().contains(event->pos())) {  // 判断是否脱离Tabbar

            // dock_tabwidget 中, 接收信号 dragout 的槽函数里 调用了 drag->exec(), 该函数导致 tabbar 无法触发 release 事件, 所以这里手动触发下
            QMouseEvent* e = new QMouseEvent(QEvent::MouseButtonRelease, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::postEvent(this, e);

            emit dragout(currentIndex());
        }
    }

    QTabBar::mouseMoveEvent(event);
}

void dock_tabbar::mouseReleaseEvent(QMouseEvent* event) {
    m_bDragTab = false;
    QTabBar::mouseReleaseEvent(event);
}
