#ifndef DOCK_TABBAR_H
#define DOCK_TABBAR_H

#include <QTabBar>

#include <QApplication>
#include <QMouseEvent>

class dock_tabbar : public QTabBar {
    Q_OBJECT
public:
    explicit dock_tabbar(QWidget* parent = nullptr);

signals:
    void dragout(int index);

private:
    bool m_bDragTab = false;

public:
    QRect boundary(int dx = 100, int dy = 60) { return contentsRect().adjusted(-dx, -dy, dx, dy); }

protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
};

#endif  // DOCK_TABBAR_H
