#ifndef UYK_BASEPLOT_H
#define UYK_BASEPLOT_H

#include <QObject>
#include <QWidget>

#include <QList>
#include <QVector>

#include <QRubberBand>

#include "qcustomplot.h"

#include "fft.h"
#include "uyk_savefile.h"
#include "uyk_custom_action.h"

#define QStr QStringLiteral

class uyk_baseplot : public QCustomPlot {
    Q_OBJECT
public:
    explicit uyk_baseplot(QWidget* parent = nullptr);

private:
    void initAxes(void);
    void initLegend(void);
    void initGraph(void);
    void initAction(void);

public:
    // 右键菜单
    QMenu* m_menu;

    void    refresh();
    QTimer* m_timer_replot = nullptr;

    // **************************************** //

private:
    bool mMode = true;
    /*
      true  = valMode, graph_org -> graph_fft
      false = fftMode, graphs_org -> graph_val
    */

public:
    bool getMode(void);
    void setMode(bool mode);

    // **************************************** //

public:
    /*
        axisrect_org: 时域图 time domain
        axisrect_fft: 频域图 frequency domain
        axisrect_var: 瞬时值 instantaneous value
    */

    QCPLayoutGrid* subLayout;

    QCPLegend* legend_org;
    QCPLegend* legend_fft;

    QCPAxis* yAxisExtra;

    QCPAxisRect* axisrect_org;
    QCPAxisRect* axisrect_fft;
    QCPAxisRect* axisrect_val;

    size_t    m_index1 = 0;
    QCPGraph* graph_org;
    QCPGraph* graph_fft;

    size_t           m_index2 = 0;
    QList<QCPGraph*> graphs_org;
    QCPBars*         graph_val;

    void addVals(const QVector<double>& vals);
    void scaleAxes();

    bool m_autorescale=false;
    size_t m_point_count = 2000; // 显示的点数

    // **************************************** //

public:
    int m_tip_offset = 20;  // 显示位置相对于鼠标偏移量

private:
    // 曲线值
    QCPItemText* m_valtip = nullptr;

    // 十字线 crosshair
    QCPItemStraightLine* m_hLine = nullptr;
    QCPItemStraightLine* m_vLine = nullptr;

    void initTips();

private slots:
    void updateTips();

    // **************************************** //

private:
    // 右键框选区域
    QRubberBand* m_selection              = nullptr;
    bool         m_enable_scale_selection = true;
    bool         event_select_area(uint8_t e, QMouseEvent* event);

protected:
    // 重载鼠标事件
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif  // UYK_BASEPLOT_H
