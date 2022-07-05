#ifndef FFT_H
#define FFT_H

#include <cmath>

#include <QObject>

void fft(const double* src_x, const double* src_y, double* dst_x, double* dst_y, int k);
double* q_fft(const QVector<double>& in,size_t k);

#endif // FFT_H
