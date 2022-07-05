#include "plot_multicurve.h"

plot_multicurve::plot_multicurve(QWidget* parent) : plot_common(parent) {

    initAxis();
    initMenu();
}

void plot_multicurve::initAxis()
{
    // 配置坐标轴

    auto axrc = axisRect();
    axrc->setupFullAxesBox();

    axrc->addAxis(QCPAxis::atLeft);  // 双y轴

    axrc->setRangeDragAxes(axrc->axes());  // 配置可拖动轴
    axrc->setRangeZoomAxes(axrc->axes());  // 配置可缩放轴

    axrc->axis(QCPAxis::atLeft)->grid()->setZeroLinePen(QPen(Qt::red));  // 坐标轴颜色
    axrc->axis(QCPAxis::atBottom)->grid()->setZeroLinePen(QPen(Qt::red));
}

void plot_multicurve::initMenu() {
    // 配置右键菜单

    QMenu* menu_mode = new QMenu("mode", this);

    m_Menu->addMenu(menu_mode);

    static QAction *actModeSingle,*actModeMulti,*actModeFFT;
    (actModeSingle = menu_mode->addAction(QLatin1String("single"),[&](){
        m_mode=0;
        actModeSingle->setChecked(true);
        actModeMulti->setChecked(false);
        actModeFFT->setChecked(false);
    }))->setCheckable(true);
    actModeSingle->setChecked(true);
    (actModeMulti = menu_mode->addAction(QLatin1String("multi"),[&](){
        m_mode=1;
        actModeSingle->setChecked(false);
        actModeMulti->setChecked(true);
        actModeFFT->setChecked(false);
    }))->setCheckable(true);
    (actModeFFT = menu_mode->addAction(QLatin1String("fft"),[&](){
        m_mode=2;
        actModeSingle->setChecked(false);
        actModeMulti->setChecked(false);
        actModeFFT->setChecked(true);
    }))->setCheckable(true);

    m_Menu->addSeparator();

    // - 坐标轴范围

    auto action_rescaley = [&]() {
        if (mGraphs.isEmpty()) {
            yAxis->setRange(-1, 1);
            return;
        }
        yAxis->rescale();
        double delta = yAxis->range().size() * 0.05;
        yAxis->setRange(yAxis->range().lower - delta, yAxis->range().upper + delta);
    };

    m_Menu->addSeparator();
    m_Menu->addAction("rescale", [&, action_rescaley]() {
        xAxis->rescale();
        action_rescaley();
        // action_rescaley 必须值传递, 否则程序崩溃.可能是 this 指针的问题??
    });

    // - x轴范围

    QMenu* menu_rescalex = new QMenu("rescale x", this);
    m_Menu->addMenu(menu_rescalex);

    menu_rescalex->addAction("auto", [&]() { xAxis->rescale(); });
    menu_rescalex->addSeparator();

    auto action_rescalex = [&](size_t size) { menu_rescalex->addAction(QString::number(size), [&,size]() { xAxis->setRange((m_index < size) ? 0 : (m_index - size), m_index); }); };

    action_rescalex(100);
    action_rescalex(500);
    action_rescalex(1000);
    action_rescalex(5000);
    action_rescalex(10000);
    action_rescalex(20000);
    action_rescalex(50000);

    // - y轴范围

    QMenu* menu_rescaley = new QMenu("rescale y", this);

    m_Menu->addMenu(menu_rescaley);
    menu_rescaley->addAction("auto", action_rescaley);

    menu_rescaley->addSeparator();
    menu_rescaley->addAction("0~1", [&]() { yAxis->setRange(0, 1); });
    menu_rescaley->addAction("±1", [&]() { yAxis->setRange(-1, 1); });
    menu_rescaley->addAction("±100", [&]() { yAxis->setRange(-100, 100); });
    menu_rescaley->addSeparator();
    menu_rescaley->addAction("int8", [&]() { yAxis->setRange(INT8_MIN, INT8_MAX); });
    menu_rescaley->addAction("int10", [&]() { yAxis->setRange(-(1 << 9), (1 << 9) - 1); });
    menu_rescaley->addAction("int12", [&]() { yAxis->setRange(-(1 << 11), (1 << 11) - 1); });
    menu_rescaley->addAction("int16", [&]() { yAxis->setRange(INT16_MIN, INT16_MAX); });
    menu_rescaley->addAction("int32", [&]() { yAxis->setRange(INT_MIN, INT_MAX); });
    menu_rescaley->addAction("uint8", [&]() { yAxis->setRange(0, UINT8_MAX); });
    menu_rescaley->addAction("uint10", [&]() { yAxis->setRange(0, (1 << 10) - 1); });
    menu_rescaley->addAction("uint12", [&]() { yAxis->setRange(0, (1 << 12) - 1); });
    menu_rescaley->addAction("uint16", [&]() { yAxis->setRange(0, UINT16_MAX); });
    menu_rescaley->addAction("uint32", [&]() { yAxis->setRange(0, UINT_MAX); });

    // - 保存数据

    m_Menu->addSeparator();
    m_Menu->addAction("save to csv", [&]() {});
    m_Menu->addAction("save to txt", [&]() {});

    // - 清除曲线

    m_Menu->addSeparator();
    m_Menu->addAction("clear", [&]() { clearGraphs(); m_index = 0;});
}
#include "algorithm_fft.h"
#include <QDebug>
bool plot_multicurve::addValues(const QVector<double>& values) {

    if(m_mode ==0 ){

        // auto add graph
        if(mGraphs.size()==0){
            mGraphs.clear();
            QCPGraph* p = addGraph(xAxis, yAxis);
            p->setName(QLatin1String("line"));
        }

        // add values

        for(int i=0;i<values.size();++i)
            mGraphs.at(0)->data()->add(QCPGraphData(m_index++, values.at(i)));

        xAxis->setRange(m_index > xAxis->range().size() ? (m_index -xAxis->range().size()) : 0,m_index);

        return true;

    }else if(m_mode == 1 ){

    if (values.size() > MAX_COUNT_OF_CURVE) return false;

    // auto append graph
    values.length();
    for (int i = mGraphs.size(); i < values.size(); ++i) {
        QCPGraph* p = addGraph(xAxis, yAxis);
        p->setName(tr("line%1").arg(i + 1));
        p->setPen(QPen(QColor::fromHsv(360 / MAX_COUNT_OF_CURVE * i /*0~360*/, 255 /*0~255*/, 127 /*0~255*/)));
    }

    // add values
    size_t i = 0;
    foreach (auto graph, mGraphs) {
        // addData & setData will alloc new buffer, and then copy old buffer to new buffer, so don't use them
        graph->data()->add(QCPGraphData(m_index, values.at(i++)));
    }

    ++m_index;

    xAxis->setRange(m_index > xAxis->range().size() ? (m_index -xAxis->range().size()) : 0,m_index);
    return true;

    }else if(m_mode ==2){

        if(values.length()>(1<<7)) return false;

        double* fft_ret = q_fft(values,7);

        // auto add graph
        if(mGraphs.size()==0){
            mGraphs.clear();
            QCPGraph* p = addGraph(xAxis, yAxis);
            p->setLineStyle(QCPGraph::lsImpulse);
            p->setName(QLatin1String("line"));
        }

        // add values

        mGraphs.at(0)->data().data()->clear();

        for(int i=0;i<values.size()/2;++i)
            mGraphs.at(0)->data()->add(QCPGraphData(i, fft_ret[i]));

        xAxis->setRange(0,(1<<7)/2);

        delete[] fft_ret;

        return true;
    }
    return false;
}
