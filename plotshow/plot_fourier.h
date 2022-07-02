#ifndef PLOT_FOURIER_H
#define PLOT_FOURIER_H

#include "plot_common.h"

class plot_fourier : public plot_common
{
public:
    plot_fourier(QWidget* parent = nullptr);

public:
    /*
        org: 时域图 time domain
        fft: 频域图 frequency domain
        var: 瞬时值 instantaneous value
    */

    QCPGraph* graph_org;
    QCPGraph* graph_fft;
    QCPBars* graph_var;

};

#endif // PLOT_FOURIER_H
