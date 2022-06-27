#ifndef UYKPLOT_COMMON_H
#define UYKPLOT_COMMON_H

#include <QMenu>
#include <QRubberBand>
#include <QTimer>

#include "qcustomplot.h"

#define QStr QStringLiteral

class uykplot_common : public QCustomPlot {
    Q_OBJECT
public:
    explicit uykplot_common(QWidget* parent = nullptr);

    /***********************************/

private:
    QTimer* m_TmrReplot = nullptr;

    void initAxis(void);
    void initLegend(void);

public:
    QMenu* m_Menu = nullptr;

    inline void refresh(void);

    /***********************************/

#define VALSTIP_OFFSET 20 // 位置偏移

private:
    QPointF m_mouse_pos;

    // 十字线 crosshair
    QCPItemStraightLine* m_hLine = nullptr;
    QCPItemStraightLine* m_vLine = nullptr;

    // 值提示 valuestip
    QCPItemText* m_valstip = nullptr;

    void initValuesTip(void);
    void setValuesTipVisible(bool b);

    /***********************************/

private:
    // 右键框选区域进行缩放

    QRubberBand* m_SelectedArea = nullptr;

    void initSelectedAreaMask(void);
    bool updateSelectArea(uint8_t e, QMouseEvent* event);

    /***********************************/

private:
    bool isMouseInGrid(const QPoint&  pos);

protected:
    // 重载鼠标事件
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    /***********************************/

public:
    size_t m_index=0; // for addValues

};

#endif  // UYKPLOT_COMMON_H
