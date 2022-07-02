#include "datahandler.h"

datahandler::datahandler(QWidget* parent) : QWidget(parent), m_TmrSpeedCalc(new QTimer(this)) {
    connect(m_TmrSpeedCalc, &QTimer::timeout, [&]() {  // @ kBps
        m_SpeedOfRecv     = QString("%1 kB/s").arg(QString::number((m_BytesOfRecv - m_LastBytesOfRecv) / 1024.0f, 'f', 2));
        m_SpeedOfSend     = QString("%1 kB/s").arg(QString::number((m_BytesOfSend - m_LastBytesOfSend) / 1024.0f, 'f', 2));
        m_LastBytesOfRecv = m_BytesOfRecv;
        m_LastBytesOfSend = m_BytesOfSend;
        emit updatestat(m_BytesOfRecv, m_BytesOfSend, m_SpeedOfRecv, m_SpeedOfSend);
    });
}

void datahandler::start() {
    m_buffer.clear();
    m_TmrSpeedCalc->start(1000);
    emit runstate(true);
}

void datahandler::stop() {
    m_TmrSpeedCalc->stop();
    m_SpeedOfRecv = m_SpeedOfSend = QLatin1String("0.00 kB/s");
    emit updatestat(m_BytesOfRecv, m_BytesOfSend, m_SpeedOfRecv, m_SpeedOfSend);
    emit runstate(false);
}

void datahandler::recvdata(const QByteArray& bytes /*按行读取*/) {
    m_buffer += bytes;
    m_BytesOfRecv += bytes.length();
    emit readdata(bytes);
    if (bytes.endsWith('\n')) {  // 按行解析
        emit readline(m_buffer);
        // emit readline(m_buffer.remove(bytes.size(),1)); // 移除末尾换行符
        m_buffer.clear();
    }
}

void datahandler::senddata(const QByteArray& bytes) { m_BytesOfSend += bytes.length(); }
