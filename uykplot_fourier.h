#ifndef UYKPLOT_FOURIER_H
#define UYKPLOT_FOURIER_H

#include "uykplot_common.h"

class uykplot_fourier : public uykplot_common
{
public:
    uykplot_fourier(QWidget* parent = nullptr);

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

#endif // UYKPLOT_FOURIER_H
