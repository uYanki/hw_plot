#ifndef UYK_PLOT_H
#define UYK_PLOT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include <QRubberBand>

#include "QCustomPlot/qcustomplot.h"

// QCPAbstractItem: https://www.cnblogs.com/swarmbees/p/6058263.html

class u_baseplot : public QCustomPlot {
    Q_OBJECT
public:
    explicit u_baseplot(QWidget* parent = nullptr);

public:
    // 右键菜单
    QMenu* m_menu;

private:
    QTimer* m_timer_replot = nullptr;

    // 曲线值和十字线
    QCPItemText*         m_item_tip    = nullptr;
    int                  m_tip_offset  = 20;  // 显示位置相对于鼠标偏移量
    QCPItemStraightLine* m_item_line_x = nullptr;
    QCPItemStraightLine* m_item_line_y = nullptr;

    // 右键框选区域
    QRubberBand* m_selection              = nullptr;
    bool m_enable_scale_selection = true;
    bool event_select_area(uint8_t e, QMouseEvent* event);

protected:
    // 重写鼠标事件
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
};

#endif  // UYK_PLOT_H
