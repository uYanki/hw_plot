#include "u_baseplot.h"

u_baseplot::u_baseplot(QWidget* parent) : QCustomPlot(parent) {
    // 坐标轴拖拽及缩放
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 右键框选区域
    m_selection = new QRubberBand(QRubberBand::Rectangle, this);
    m_selection->setBackgroundRole(QPalette::Light);

    // 自定义右键菜单
    m_menu = new QMenu(this);
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    // 曲线值提示
    m_item_tip = new QCPItemText(this);
    m_item_tip->setPadding(QMargins(6, 6, 6, 6));
    m_item_tip->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_item_tip->setBrush(QBrush(QColor(0, 0, 255, 100)));  // crBackground
    m_item_tip->setText("");
    m_item_tip->setColor(Qt::GlobalColor::white);  // crText
    m_item_tip->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_item_tip->setLayer("overlay");

    // 鼠标十字线
    m_item_line_x = new QCPItemStraightLine(this);
    m_item_line_y = new QCPItemStraightLine(this);
    m_item_line_x->setPen(QPen(Qt::GlobalColor::blue));
    m_item_line_y->setPen(QPen(Qt::GlobalColor::blue));

    connect(this, &QCustomPlot::beforeReplot, [this]() {
        QPoint pos   = mapFromGlobal(QCursor::pos());
        float  x_val = xAxis->pixelToCoord(pos.x());
        float  y_val = yAxis->pixelToCoord(pos.y());

        // 十字线
        m_item_line_x->point1->setCoords(x_val, y_val);
        m_item_line_x->point2->setCoords(x_val + 1, y_val);
        m_item_line_y->point1->setCoords(x_val, y_val);
        m_item_line_y->point2->setCoords(x_val, y_val + 1);

        // 曲线值
        int cnt = graphCount();
        if (cnt == 0) return;
        QString tip = "x: " + QString::number(x_val, 10, 3) + "\n";
        for (int i = 0; i < cnt; ++i) {
            float y_val = graph(i)->data()->at(x_val)->value;
            tip += graph(i)->name() + ": " + QString::number(y_val);
            if (i != cnt - 1) tip += "\n";
        }
        m_item_tip->setText(tip);
        x_val = xAxis->pixelToCoord(pos.x() + m_tip_offset);
        y_val = yAxis->pixelToCoord(pos.y() + m_tip_offset);
        m_item_tip->position->setCoords(x_val, y_val);
    });

    // 绘图时钟
    m_timer_replot = new QTimer(this);
    connect(m_timer_replot, &QTimer::timeout, [&]() { replot(QCustomPlot::RefreshPriority::rpQueuedReplot); });
    m_timer_replot->start(30);  // 30ms 1帧 ≈ 1s 33帧


}

void u_baseplot::refresh(){
     replot(QCustomPlot::RefreshPriority::rpQueuedReplot); }
}

bool u_baseplot::event_select_area(uint8_t e, QMouseEvent* event) {
    static QPoint ptStrat;  // 框选起始点
    if (m_enable_scale_selection && e == QEvent::MouseButtonPress) {
        /* mousePressEvent */
        ptStrat = event->pos();  // 记录框选的起始点
        m_selection->setGeometry(QRect(ptStrat, QSize()));
        m_selection->show();
        return true;
    } else if (m_selection->isVisible() && e == QEvent::MouseMove) {
        /* mouseMoveEvent */
        m_selection->setGeometry(QRect(ptStrat, event->pos()));  // 框选区域可视化
        return true;
    } else if (m_selection->isVisible() && e == QEvent::MouseButtonRelease) {
        /* mouseReleaseEvent */
        m_selection->hide();
        const QRect rc = m_selection->geometry();
        if (rc.width() > 5 && rc.height() > 5) {
            // 将图表的显示区域设置为框选区域
            xAxis->setRange(xAxis->pixelToCoord(rc.left()), xAxis->pixelToCoord(rc.right()));
            yAxis->setRange(yAxis->pixelToCoord(rc.top()), yAxis->pixelToCoord(rc.bottom()));
            replot();  // 刷新图表
            return true;
        }
    }
    return false;
}

void u_baseplot::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton)
        event_select_area(QEvent::MouseButtonPress, event);
    QCustomPlot::mousePressEvent(event);
}

void u_baseplot::mouseMoveEvent(QMouseEvent* event) {
    event_select_area(QEvent::MouseMove, event);
    QCustomPlot::mouseMoveEvent(event);
}

void u_baseplot::mouseReleaseEvent(QMouseEvent* event) {
    if (!event_select_area(QEvent::MouseButtonRelease, event) &&
        event->button() == Qt::RightButton) {
        m_menu->exec(QCursor::pos());  // 弹出右键菜单
    }
    QCustomPlot::mouseReleaseEvent(event);
}
