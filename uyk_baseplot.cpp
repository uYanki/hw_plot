#include "uyk_baseplot.h"

uyk_baseplot::uyk_baseplot(QWidget* parent) : QCustomPlot(parent) {
    initAxes();
    initLegend();
    initGraph();
    initAction();

    // right-click context menu 右键菜单
    m_menu = new QMenu(this);
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    // 右键框选区域
    m_selection = new QRubberBand(QRubberBand::Rectangle, this);
    m_selection->setBackgroundRole(QPalette::Light);

    initTips();

    // 绘图时钟
    m_timer_replot = new QTimer(this);
    connect(m_timer_replot, &QTimer::timeout, [&]() { replot(QCustomPlot::RefreshPriority::rpQueuedReplot); });
    m_timer_replot->start(35);  // 30ms 1帧 ≈ 1s 28帧
}

void uyk_baseplot::initAxes() {
    /************ clear default ************/

    clearGraphs();
    clearPlottables();
    plotLayout()->clear();

    /************ create axes ************/

    plotLayout()->addElement(0, 0, axisrect_org = new QCPAxisRect(this));  // in first row
    plotLayout()->addElement(1, 0, subLayout = new QCPLayoutGrid());       // in second row

    subLayout->addElement(0, 0, axisrect_val = new QCPAxisRect(this, false));  // at left
    subLayout->addElement(0, 1, axisrect_fft = new QCPAxisRect(this, false));  // at right

    plotLayout()->setRowStretchFactor(0, 0.75);
    plotLayout()->setRowStretchFactor(1, 0.25);
    subLayout->setColumnStretchFactor(0, 0.3);
    subLayout->setColumnStretchFactor(1, 0.7);

    /************ setup axes ************/

    axisrect_org->setupFullAxesBox(true);
    axisrect_org->axis(QCPAxis::atBottom)->setLabel("t");
    axisrect_org->addAxis(QCPAxis::atLeft);  // 双y轴

    QList<QCPAxis*> axes;  // 配置可拖动和可缩放的轴
    axes << axisrect_org->axis(QCPAxis::atLeft)
         << axisrect_org->axis(QCPAxis::atLeft, 1)
         << axisrect_org->axis(QCPAxis::atBottom);
    axisrect_org->setRangeZoomAxes(axes);
    axisrect_org->setRangeDragAxes(axes);
    // axisrect_org->setRangeZoomFactor()

    axisrect_org->axis(QCPAxis::atLeft)->grid()->setZeroLinePen(QPen(Qt::red));  // 坐标轴颜色
    axisrect_org->axis(QCPAxis::atBottom)->grid()->setZeroLinePen(QPen(Qt::red));

    axisrect_fft->addAxes(QCPAxis::atRight | QCPAxis::atBottom);
    axisrect_fft->axis(QCPAxis::atBottom)->setLabel("hz");
    axisrect_fft->axis(QCPAxis::atRight)->grid()->setVisible(true);
    axisrect_fft->axis(QCPAxis::atBottom)->grid()->setVisible(true);

    axes.clear();
    axes << axisrect_fft->axis(QCPAxis::atRight)
         << axisrect_fft->axis(QCPAxis::atBottom);
    axisrect_fft->setRangeZoomAxes(axes);
    axisrect_fft->setRangeDragAxes(axes);

    axisrect_val->addAxes(QCPAxis::atLeft | QCPAxis::atBottom);
    axisrect_val->axis(QCPAxis::atBottom)->setLabel("index");
    axisrect_val->axis(QCPAxis::atLeft)->ticker()->setTickCount(3);

    /************ setup variables ************/

    xAxis  = axisrect_org->axis(QCPAxis::atBottom);
    yAxis  = axisrect_org->axis(QCPAxis::atLeft);
    xAxis2 = axisrect_org->axis(QCPAxis::atTop);
    ;
    yAxis2     = axisrect_org->axis(QCPAxis::atRight);
    yAxisExtra = axisrect_org->axis(QCPAxis::atLeft, 1);

    /************ setup ticker ************/

    // int ticker for axisX_val

    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    intTicker->setTickStep(1.0);
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    axisrect_val->axis(QCPAxis::atBottom)->setSubTicks(false);    // hide default ticker
    axisrect_val->axis(QCPAxis::atBottom)->setTicker(intTicker);  // set int ticker

    /************ reset layer ************/

    foreach (QCPAxisRect* rect, axisRects()) {
        foreach (QCPAxis* axis, rect->axes()) {
            axis->setLayer(QStr("axes"));          // axis -> axes layer
            axis->grid()->setLayer(QStr("grid"));  // grid -> grid layer
        }
    }
}

void uyk_baseplot::initLegend() {
    /************ setup legend ************/

    // 在调用 legend.addItem() 之前, 需调用 legend.setLayer("legend"), 否则会导致先前添加的图例区域是空白的. (猜测和zOrder有关)

    axisrect_org->insetLayout()->addElement(legend_org = new QCPLegend(), Qt::AlignTop | Qt::AlignRight);
    axisrect_fft->insetLayout()->addElement(legend_fft = new QCPLegend(), Qt::AlignTop | Qt::AlignRight);

    /************ reset layer ************/

    legend_org->setLayer(QStr("legend"));
    legend_fft->setLayer(QStr("legend"));
}

void uyk_baseplot::initGraph() {
    // create graphs

    /************ create graphs ************/

    graph_org = addGraph(axisrect_org->axis(QCPAxis::atBottom), axisrect_org->axis(QCPAxis::atLeft));  // for fft
    graph_fft = addGraph(axisrect_fft->axis(QCPAxis::atBottom), axisrect_fft->axis(QCPAxis::atRight));
    graph_val = new QCPBars(axisrect_val->axis(QCPAxis::atBottom), axisrect_val->axis(QCPAxis::atLeft));

    /************ setup graphs ************/

    graph_org->setName(QStr("Origin"));
    graph_fft->setName(QStr("FFT"));
    // graph_org->setVisible(false);
    // graph_fft->setVisible(false);

    /************ setup style ************/

    graph_org->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    graph_fft->setLineStyle(QCPGraph::lsImpulse);

    /************ add to legend ************/

    legend_org->clearItems();
    legend_fft->clearItems();
    legend_org->addItem(new QCPPlottableLegendItem(legend_org, graph_org));
    legend_fft->addItem(new QCPPlottableLegendItem(legend_fft, graph_fft));
}

void uyk_baseplot::initAction() {
    setInteractions(QCP::Interaction::iRangeDrag       // 坐标轴拖拽
                    | QCP::Interaction::iRangeZoom     // 坐标轴缩放
                    | QCP::Interaction::iSelectAxes    // 坐标轴可选
                    | QCP::Interaction::iSelectLegend  // 图例可选
                    | QCP::Interaction::iSelectPlottables // 曲线可选
    );

    // 不要设置 QCP::iSelectPlottables, 会卡顿
    // setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);  // 多选 ctrl + lbutton

    /************ setup legend ************/

    // font
    QFont legendFont = font();
    legend_org->setFont(legendFont);
    legendFont.setPointSize(10);
    legend_org->setSelectedFont(legendFont);

    // items are selectable, but box is unselectable
    legend_org->setSelectableParts(QCPLegend::spItems);
    legend_fft->setSelectableParts(QCPLegend::spItems);

    /************ slots ************/

    //connect(this, &QCustomPlot::plottableClick, [&](QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent*){
    //    qDebug()<<plottable->interface1D()->dataMainValue(dataIndex); // 鼠标取值
    //});

    connect(this, &QCustomPlot::legendDoubleClick, [&](QCPLegend*, QCPAbstractLegendItem* item, QMouseEvent*) {
        // rename  graph

        if (item)  // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
        {
            QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);

            bool    ok;
            QString newName = QInputDialog::getText(this, "Plot", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
            if (ok) plItem->plottable()->setName(newName);
        }
    });

    connect(this, &QCustomPlot::selectionChangedByUser, [&]() {
        // when an axis is selected, only that direction can be dragged and zoomed

        if (xAxis->selectedParts().testFlag(QCPAxis::spAxis) || xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
            xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
            xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

            axisrect_org->setRangeDrag(xAxis->orientation());
            axisrect_org->setRangeZoom(xAxis->orientation());

        } else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) || yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
                   yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) ||
                   yAxisExtra->selectedParts().testFlag(QCPAxis::spAxis) || yAxisExtra->selectedParts().testFlag(QCPAxis::spTickLabels)) {
            if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) || yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
                yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
            else if (yAxisExtra->selectedParts().testFlag(QCPAxis::spAxis) || yAxisExtra->selectedParts().testFlag(QCPAxis::spTickLabels))
                yAxisExtra->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
            else if (yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
                yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

            yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

            axisrect_org->setRangeDrag(yAxis->orientation());
            axisrect_org->setRangeZoom(yAxis->orientation());

        } else {
            axisrect_org->setRangeDrag(Qt::Horizontal | Qt::Vertical);
            axisrect_org->setRangeZoom(Qt::Horizontal | Qt::Vertical);
        }

        // synchronize selection of graphs with selection of corresponding legend items:
        for (int i = 0; i < graphs_org.length(); ++i) {
            // 需配合 QCP::Interaction::iSelectPlottables 一起使用
            QCPGraph*               graph = graphs_org.at(i);
            QCPPlottableLegendItem* item  = legend_org->itemWithPlottable(graph);
            if (item->selected() || graph->selected()) {
                item->setSelected(true);
                graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            }
        }
    });
}

void uyk_baseplot::refresh() { replot(QCustomPlot::RefreshPriority::rpQueuedReplot); }

bool uyk_baseplot::getMode() { return mMode; }

void uyk_baseplot::setMode(bool mode) {
    if (mMode == mode) return;
    mMode = mode;
    graph_org->setVisible(!mode);
    foreach (auto graph, graphs_org) graph->setVisible(mode);
}


void uyk_baseplot::addVals(const QVector<double>& values) {
    static Qt::GlobalColor color_of_series[6] = {Qt::red, Qt::darkGreen, Qt::blue, Qt::darkCyan, Qt::magenta, Qt::darkYellow};  // 曲线颜色

    // addData & setData will alloc new buffer, and then copy old buffer to new buffer, so don't use them
    if (mMode) {
        for (int i = graphs_org.length(); i < values.length(); ++i) {  // auto add graph
            QCPGraph* p = addGraph(axisrect_org->axis(QCPAxis::atBottom), axisrect_org->axis(QCPAxis::atLeft));
            p->setName(tr("line%1").arg(i + 1));
            p->setPen(QPen(QColor(color_of_series[i])));
            graphs_org.append(p);
        }

        graph_val->data().data()->clear();
        for (int i = 0; i < values.length(); ++i) {
            graphs_org.at(i)->data()->add(QCPGraphData(m_index1, values.at(i)));
            graph_val->data()->add(QCPBarsData(i + 1, values.at(i)));
        }

        ++m_index1;

    } else {
        for (int i = 0; i < values.length(); ++i)
            graph_org->data()->add(QCPGraphData(++m_index2, values.at(i)));
    }
}

void uyk_baseplot::scaleAxes(bool keyAxis, bool valAxis) {
    if (mMode) {
//        if (keyAxis)
//            axisrect_org->axis(QCPAxis::atBottom)->rescale();

//        if (valAxis)
//            foreach (auto graph, graphs_org)
//                graph->rescaleValueAxis(true);


        // graph_val->valueAxis()->setRange(range.lower * 1.2, range.upper * 1.2);


    } else {
        graph_org->rescaleAxes(true);

        if (keyAxis) graph_fft->rescaleKeyAxis();
        if (valAxis) graph_fft->rescaleValueAxis(true);
    }
}

void uyk_baseplot::initTips() {
    // mouse crosshair 鼠标十字线
    m_hLine = new QCPItemStraightLine(this);
    m_vLine = new QCPItemStraightLine(this);
    m_hLine->setPen(QPen(Qt::GlobalColor::blue));
    m_vLine->setPen(QPen(Qt::GlobalColor::blue));
    m_hLine->setSelectable(false);
    m_vLine->setSelectable(false);

    // 自定义层 (默认层(低层->顶层):background -> grid -> main -> axes -> legend -> overlay)
    addLayer(QStr("crosshair"), layer("legend"), QCustomPlot::LayerInsertMode::limBelow);
    // "tipLayer" 需在 "main" 层的下方, 否则十字线会导致 "main" 层无法响应部分鼠标事件 (eg: main->plottableClick,legend->legendDoubleClick)
    m_hLine->setLayer(QStr("crosshair"));
    m_vLine->setLayer(QStr("crosshair"));

    // values tip 曲线值提示
    m_valtip = new QCPItemText(this);
    m_valtip->setPadding(QMargins(6, 6, 6, 6));
    m_valtip->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_valtip->setBrush(QBrush(QColor(0, 0, 255, 100)));  // crBackground
    m_valtip->setText("");
    m_valtip->setColor(Qt::GlobalColor::white);  // crText
    m_valtip->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_valtip->setLayer(QStr("overlay"));

    // connect slot
    connect(this, &QCustomPlot::beforeReplot, this, &uyk_baseplot::updateTips);

}

void uyk_baseplot::updateTips() {
    QPoint pos = mapFromGlobal(QCursor::pos());

    if (!geometry().contains(pos)) {  // 鼠标不在控件内
        // 隐藏十字线
        m_hLine->setVisible(false);
        m_vLine->setVisible(false);
        m_valtip->setVisible(false);
        return;
    }

    m_hLine->setVisible(true);
    m_vLine->setVisible(true);
    m_valtip->setVisible(true);

    // axis_org_x->setRange(i-axis_org_x->range().size(), axis_org_x->range().size(), Qt::AlignmentFlag::AlignRight);

    float x_val = xAxis->pixelToCoord(pos.x());
    float y_val = yAxis->pixelToCoord(pos.y());

    // 十字线
    m_hLine->point1->setCoords(x_val, y_val);
    m_hLine->point2->setCoords(x_val + 1, y_val);
    m_vLine->point1->setCoords(x_val, y_val);
    m_vLine->point2->setCoords(x_val, y_val + 1);

    // 曲线值
    int cnt = graphCount();
    if (cnt == 0) return;
    QString tip = "x: " + QString::number(x_val, 10, 3) + "\n";

    if (mMode) {

        graph_val->rescaleValueAxis(true);
        graph_val->keyAxis()->setRange(0, graphs_org.length() + 1);

        foreach (auto graph, graphs_org) {
            float y_val = graph->data()->at(x_val)->value;
            tip += graph->name() + ": " + QString::number(y_val);
            if (graph != graphs_org.last()) tip += "\n";
        }
    } else {
        float y_val = graph_org->data()->at(x_val)->value;
        tip += graph_org->name() + ": " + QString::number(y_val);
    }

    m_valtip->setText(tip);
    x_val = xAxis->pixelToCoord(pos.x() + m_tip_offset);
    y_val = yAxis->pixelToCoord(pos.y() + m_tip_offset);
    m_valtip->position->setCoords(x_val, y_val);

}

bool uyk_baseplot::event_select_area(uint8_t e, QMouseEvent* event) {
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
            yAxisExtra->setRange(yAxisExtra->pixelToCoord(rc.top()), yAxisExtra->pixelToCoord(rc.bottom()));
            return true;
        }
    }
    return false;
}

void uyk_baseplot::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton)
        event_select_area(QEvent::MouseButtonPress, event);
    QCustomPlot::mousePressEvent(event);
}

void uyk_baseplot::mouseMoveEvent(QMouseEvent* event) {
    event_select_area(QEvent::MouseMove, event);
    QCustomPlot::mouseMoveEvent(event);
}

void uyk_baseplot::mouseReleaseEvent(QMouseEvent* event) {
    if (!event_select_area(QEvent::MouseButtonRelease, event) &&
        event->button() == Qt::RightButton) {
        m_menu->exec(QCursor::pos());  // 弹出右键菜单
    }
    QCustomPlot::mouseReleaseEvent(event);
}


//void MainWindow::contextMenuRequest(QPoint pos) {
//    QMenu* menu = new QMenu(this);
//    menu->setAttribute(Qt::WA_DeleteOnClose);

//    if (ui->customPlot->legend->selectTest(pos, false) >= 0)  // context menu on legend requested
//    {
//        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
//        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
//        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
//        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
//        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
//    } else  // general context menu on graphs requested
//    {
//        menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
//        if (ui->customPlot->selectedGraphs().size() > 0)
//            menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
//        if (ui->customPlot->graphCount() > 0)
//            menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
//    }

//    menu->popup(ui->customPlot->mapToGlobal(pos));
//}
