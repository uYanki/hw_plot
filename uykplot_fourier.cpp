#include "uykplot_fourier.h"

uykplot_fourier::uykplot_fourier(QWidget* parent) :uykplot_common(parent)
{

    // 配置坐标轴

    auto axrc = axisRect();
    axrc->setupFullAxesBox();

    std::swap(yAxis,yAxis2); // 适应框选缩放

    axrc->axis(QCPAxis::atLeft)->setVisible(false); // 隐藏坐标轴
    axrc->axis(QCPAxis::atTop)->setVisible(false);

    axrc->axis(QCPAxis::atRight)->setTickLabels(true); // 显示刻度值
    axrc->axis(QCPAxis::atRight)->grid()->setVisible(true); // 显示网格横线

    // 配置图例

    legend->setVisible(false); // 隐藏图例

    // 折线图

    graph_fft = addGraph(axrc->axis(QCPAxis::atBottom), axrc->axis(QCPAxis::atRight));

    // 曲线样式

    graph_fft->setLineStyle(QCPGraph::lsStepCenter); // 阶梯样式
    graph_fft->setBrush(QBrush(QColor(Qt::red)));

}
