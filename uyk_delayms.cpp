#include "uyk_delayms.h"

void delay_ms(unsigned int msec) {
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}
