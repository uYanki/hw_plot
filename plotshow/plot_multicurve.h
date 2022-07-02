#ifndef PLOT_MULTICURVE_H
#define PLOT_MULTICURVE_H

#include "plot_common.h"

class plot_multicurve : public plot_common {
public:
    plot_multicurve(QWidget* parent = nullptr);

private:
#define MAX_COUNT_OF_CURVE 8  // 曲线最大数目

    void initAxis(void);
    void initMenu(void);

public slots:
    bool addValues(const QVector<double>& values);
};

#endif  // PLOT_MULTICURVE_H
