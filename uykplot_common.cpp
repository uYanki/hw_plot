#include "uykplot_common.h"

uykplot_common::uykplot_common(QWidget* parent) : QCustomPlot(parent) {
    initAxis();
    initLegend();
    initValuesTip();
    initSelectedAreaMask();

    // 右键菜单
    m_Menu = new QMenu(this);
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    // 绘图时钟
    m_TmrReplot = new QTimer(this);
    connect(m_TmrReplot, &QTimer::timeout, [&]() { refresh(); });
    m_TmrReplot->start(1000 / 30.0);  // 30帧/秒
}

void uykplot_common::initAxis() {
    setInteraction(QCP::Interaction::iSelectAxes, true);  // 坐标轴可选
    setInteraction(QCP::Interaction::iRangeDrag, true);   // 允许鼠标拖拽坐标轴
    setInteraction(QCP::Interaction::iRangeZoom, true);   // 允许滚轮拖拽坐标轴

    // 设置轴拖动/缩放方向
    connect(this, &QCustomPlot::selectionChangedByUser, [&]() {
        // when an axis is selected, only that direction can be dragged and zoomed

        auto axrc     = axisRect();
        auto testFlag = [&](QCPAxis::AxisTypes types) -> int {
            foreach (auto axis, axrc->axes(types)) {
                if (axis->selectedParts().testFlag(QCPAxis::spAxis) ||
                    axis->selectedParts().testFlag(QCPAxis::spTickLabels)) {
                    // 轴和刻度同步选中
                    axis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
                    return axis->axisType();
                }
            }
            return 0;
        };

        if (testFlag(QCPAxis::atLeft | QCPAxis::atRight)) {
            axrc->setRangeZoom(Qt::Vertical);
            axrc->setRangeDrag(Qt::Vertical);
        } else if (testFlag(QCPAxis::atTop | QCPAxis::atBottom)) {
            axrc->setRangeZoom(Qt::Horizontal);
            axrc->setRangeDrag(Qt::Horizontal);
        } else {
            axrc->setRangeDrag(Qt::Horizontal | Qt::Vertical);
            axrc->setRangeZoom(Qt::Horizontal | Qt::Vertical);
        }
    });
}

void uykplot_common::initLegend() {
    legend->setVisible(true);  // 图例可视

    setInteraction(QCP::Interaction::iSelectLegend, true);      // 图例可选
    setInteraction(QCP::Interaction::iSelectPlottables, true);  // 曲线可选
    legend->setSelectableParts(QCPLegend::spItems);             // 设置允许选中的部分

    // 图例字体

    QFont legendFont = font();
    legend->setFont(legendFont);  // 默认字体
    legendFont.setPointSize(10);
    legend->setSelectedFont(legendFont);  // 选中字体

    // 双击重命名

    connect(this, &QCustomPlot::legendDoubleClick, [&](QCPLegend*, QCPAbstractLegendItem* item, QMouseEvent*) {
        if (item) {
            QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);

            bool    ok;
            QString newName = QInputDialog::getText(this, "Plot", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
            if (ok) plItem->plottable()->setName(newName);
        }
    });

    // 同步选中(图例&曲线)

    connect(this, &QCustomPlot::selectionChangedByUser, [&]() {
        // synchronize selection of graphs with selection of corresponding legend items
        for (int i = 0; i < mGraphs.size(); ++i) {
            QCPGraph*               graph = mGraphs.at(i);
            QCPPlottableLegendItem* item  = legend->itemWithPlottable(graph);
            if (item->selected() || graph->selected()) {
                item->setSelected(true);
                graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            }
        }
    });

    // 右键菜单

    connect(this, &QCustomPlot::legendClick, [&](QCPLegend*, QCPAbstractLegendItem* item, QMouseEvent* event) {
        if (item && event->type() == QEvent::MouseButtonRelease && event->button() == Qt::MouseButton::RightButton) {
            QCPPlottableLegendItem* pLegendItem = qobject_cast<QCPPlottableLegendItem*>(item);
            QCPGraph*               pPlottable  = qobject_cast<QCPGraph*>(pLegendItem->plottable());

            // 置被点击的图列和相应的曲线的选中
            deselectAll();
            pLegendItem->setSelected(true);
            pPlottable->setSelection(QCPDataSelection(pPlottable->data()->dataRange()));

            // 右键菜单

            QMenu* legendMenu = new QMenu(this);

            // - name

            QWidgetAction* pWdtAct = new QWidgetAction(legendMenu);
            QLabel*        pLabel  = new QLabel(pPlottable->name(), legendMenu);
            pWdtAct->setDefaultWidget(pLabel);
            legendMenu->addAction(pWdtAct);

            QFont labelFont = font();
            labelFont.setPointSize(10);
            labelFont.setBold(true);
            pLabel->setFont(labelFont);

            legendMenu->addSeparator();

            // - visible

            QAction* action = legendMenu->addAction(QStr("visible"), [&](bool checked) {
                pPlottable->setVisible(checked);
                pLegendItem->setTextColor(QColor(checked ? Qt::black : Qt::gray));
            });
            action->setCheckable(true);
            action->setChecked(pPlottable->visible());

            // - style

            // - save

            legendMenu->addAction(QStr("save"), [&]() {

            });

            legendMenu->exec(QCursor::pos());

            delete legendMenu;
        }
    });
}

inline void uykplot_common::refresh() {
    replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

/*********************************************************/

void uykplot_common::initValuesTip() {
    // 十字线
    m_hLine = new QCPItemStraightLine(this);
    m_vLine = new QCPItemStraightLine(this);
    m_hLine->setPen(QPen(Qt::GlobalColor::blue));
    m_vLine->setPen(QPen(Qt::GlobalColor::blue));
    m_hLine->setSelectable(false);
    m_vLine->setSelectable(false);

    addLayer(QStr("crosshair"), layer("legend"), QCustomPlot::LayerInsertMode::limBelow);
    m_hLine->setLayer(QStr("crosshair"));
    m_vLine->setLayer(QStr("crosshair"));

    /* layer 默认层, 低->顶: background -> grid -> main -> axes -> legend -> overlay
       自定义的 "crosshair" 层需在 "main" 层的下方, 否则十字线会导致 "main" 层及以下层无法响应部分鼠标事件 */

    // 值提示
    m_valstip = new QCPItemText(this);
    m_valstip->setPadding(QMargins(6, 6, 6, 6));
    m_valstip->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_valstip->setBrush(QBrush(QColor(0, 0, 255, 100)));  // crBackground
    m_valstip->setText("");
    m_valstip->setColor(Qt::GlobalColor::white);  // crText
    m_valstip->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_valstip->setLayer(QStr("overlay"));

    connect(this, &QCustomPlot::beforeReplot, this, [&]() {
        if (!m_valstip->visible()) return;

        float x_val = xAxis->pixelToCoord(m_mouse_pos.x());
        float y_val = yAxis->pixelToCoord(m_mouse_pos.y());

        // 十字线
        m_hLine->point1->setCoords(x_val, y_val);
        m_hLine->point2->setCoords(x_val + 1, y_val);
        m_vLine->point1->setCoords(x_val, y_val);
        m_vLine->point2->setCoords(x_val, y_val + 1);

        // 值提示
        int cnt = graphCount();
        if (cnt == 0) return;
        QString tip = "x: " + QString::number(x_val, 10, 3);
        if (!(x_val < 0 || x_val > m_index)) {
            // 在[0,m_index]范围内
            foreach (auto graph, mGraphs) {
                float y_val = graph->data()->at(x_val)->value;
                tip += "\n" + graph->name() + ": " + QString::number(y_val);
            }
        }
        m_valstip->setText(tip);
        x_val = xAxis->pixelToCoord(m_mouse_pos.x() + VALSTIP_OFFSET);
        y_val = yAxis->pixelToCoord(m_mouse_pos.y() + VALSTIP_OFFSET);
        m_valstip->position->setCoords(x_val, y_val);
    });

    m_mouse_pos = mapFromGlobal(QCursor::pos());

    // 默认隐藏
    setValuesTipVisible(false);
}

void uykplot_common::setValuesTipVisible(bool b) {
    if (m_valstip->visible() == b)
        return;
    if (mGraphs.isEmpty())
        b = false;
    m_hLine->setVisible(b);
    m_vLine->setVisible(b);
    m_valstip->setVisible(b);
}

/*********************************************************/

void uykplot_common::initSelectedAreaMask() {
    // 框选区域的掩膜
    m_SelectedArea = new QRubberBand(QRubberBand::Rectangle, this);
    m_SelectedArea->setBackgroundRole(QPalette::Light);
}

bool uykplot_common::updateSelectArea(uint8_t e, QMouseEvent* event) {
    static QPoint ptStrat;  // 框选起始点
    if (e == QEvent::MouseButtonPress) {
        /* mousePressEvent */
        ptStrat = event->pos();  // 记录框选的起始点
        m_SelectedArea->setGeometry(QRect(ptStrat, QSize()));
        m_SelectedArea->show();
        return true;
    } else if (m_SelectedArea->isVisible() && e == QEvent::MouseMove) {
        /* mouseMoveEvent */
        m_SelectedArea->setGeometry(QRect(ptStrat, event->pos()));  // 框选区域可视化
        return true;
    } else if (m_SelectedArea->isVisible() && e == QEvent::MouseButtonRelease) {
        /* mouseReleaseEvent */
        m_SelectedArea->hide();
        const QRect rc = m_SelectedArea->geometry();
        if (rc.width() > 5 && rc.height() > 5) {
            // 将图表的显示区域设置为框选区域
            xAxis->setRange(xAxis->pixelToCoord(rc.left()), xAxis->pixelToCoord(rc.right()));
            yAxis->setRange(yAxis->pixelToCoord(rc.top()), yAxis->pixelToCoord(rc.bottom()));
            return true;
        }
    }
    return false;
}

/*********************************************************/

bool uykplot_common::isMouseInGrid(const QPoint& pos) {
    // in axisRect

    bool bNotInAxisRect =
        axisRect()->left() > pos.x() ||
        axisRect()->right() < pos.x() ||
        axisRect()->top() > pos.y() ||
        axisRect()->bottom() < pos.y();

    if (bNotInAxisRect) return false;

    // in legend

    return !legend->rect().contains(pos);
}

void uykplot_common::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton)
        updateSelectArea(QEvent::MouseButtonPress, event);
    QCustomPlot::mousePressEvent(event);
}

void uykplot_common::mouseMoveEvent(QMouseEvent* event) {
    updateSelectArea(QEvent::MouseMove, event);

    // 鼠标在非网格区域时隐藏提示
    bool b = isMouseInGrid(event->pos());
    setValuesTipVisible(b);
    if (b) m_mouse_pos = event->pos();

    QCustomPlot::mouseMoveEvent(event);
}

void uykplot_common::mouseReleaseEvent(QMouseEvent* event) {
    if (!updateSelectArea(QEvent::MouseButtonRelease, event) &&
        event->button() == Qt::RightButton &&
        isMouseInGrid(event->pos())) {
        m_Menu->exec(QCursor::pos());  // 弹出右键菜单
    }
    QCustomPlot::mouseReleaseEvent(event);
}
