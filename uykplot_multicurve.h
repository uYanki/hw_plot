#ifndef UYKPLOT_MULTICURVE_H
#define UYKPLOT_MULTICURVE_H

#include "uykplot_common.h"

class uykplot_multicurve : public uykplot_common {
public:
    uykplot_multicurve(QWidget* parent = nullptr);

private:
#define MAX_COUNT_OF_CURVE 8  // 曲线最大数目

    void initAxis(void);
    void initMenu(void);


public:
    bool addValues(const QVector<double>& values);
};

#endif  // UYKPLOT_MULTICURVE_H
