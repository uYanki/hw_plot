#include "datahandler.h"

QByteArray strMid(const QByteArray& content, const QString& left, const QString& right) {
    int start, end;
    if ((start = content.indexOf(left)) == -1) return "";
    start += left.length();
    if ((end = right.isEmpty() ? content.length() : content.indexOf(right)) == -1) return "";
    return content.mid(start, end - start);
}

datahandler::datahandler(QWidget* parent) : QWidget(parent) {  // @ kBps
    connect(m_TmrSpeedCalc = new QTimer(parent), &QTimer::timeout, [&]() {
        m_SpeedOfRecv     = QString("%1 B/s").arg(m_BytesOfRecv - m_LastBytesOfRecv);
        m_SpeedOfSend     = QString("%1 B/s").arg(m_BytesOfSend - m_LastBytesOfSend);
        m_LastBytesOfRecv = m_BytesOfRecv;
        m_LastBytesOfSend = m_BytesOfSend;
        emit update();
    });
}

datahandler::~datahandler() {}

void datahandler::start() {
    m_TmrSpeedCalc->start(1000);
    emit update();
    emit runstate(true);
}

void datahandler::stop() {
    m_TmrSpeedCalc->stop();
    m_SpeedOfRecv = m_SpeedOfSend = QLatin1String("0 kB/s");
    emit update();
    emit runstate(false);
}

QByteArray substr(const QByteArray& content, const QString& left, const QString& right) {
    int start, end;
    if ((start = content.indexOf(left)) == -1) return "";
    start += left.length();
    if ((end = right.isEmpty() ? content.length() : content.indexOf(right)) == -1) return "";
    return content.mid(start, end - start);
}

void datahandler::recvdata(const QByteArray& bytes) {
    m_buffer += bytes;
    m_BytesOfRecv += bytes.length();
    emit readdata(bytes);
    if (bytes.contains('\n')) {  // 按行解析
        emit readline(m_buffer);
        // emit(m_prefix.isEmpty() && m_suffix.isEmpty()) ? readcmd(m_buffer) : readcmd(substr(m_buffer, m_prefix, m_suffix));
        m_buffer.clear();
    }
}

void datahandler::senddata(const QByteArray& bytes) { m_BytesOfSend += bytes.length(); }
