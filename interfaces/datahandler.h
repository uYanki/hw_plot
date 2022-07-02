#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QByteArray>
#include <QTime>
#include <QTimer>
#include <QWidget>

class datahandler : public QWidget {
    Q_OBJECT

public:
    explicit datahandler(QWidget* parent = nullptr);

    //////////////////////////////////////////////

protected:
    QTimer* const m_TmrSpeedCalc;

private:
    // 字节数统计

    size_t m_LastBytesOfRecv = 0;
    size_t m_LastBytesOfSend = 0;
    size_t m_BytesOfRecv     = 0;
    size_t m_BytesOfSend     = 0;

    // 传输速率 (kiloByte per second, kBps)

    QString m_SpeedOfRecv;
    QString m_SpeedOfSend;

public:
    virtual void start();
    virtual void stop();

signals:

    // 统计信息

    void updatestat(size_t         BytesOfRecv,
                    size_t         BytesOfSend,
                    const QString& SpeedOfRecv,
                    const QString& SpeedOfSend);

    // 运行状态
    void runstate(bool state);

    //////////////////////////////////////////////

private:
    QByteArray m_buffer;  // 缓冲区

protected:
    void recvdata(const QByteArray& bytes);

signals:
    void readdata(const QByteArray& bytes);
    void readline(const QByteArray& bytes);

protected:
    virtual void senddata(const QByteArray& bytes);

    //////////////////////////////////////////////

public:
    static QString timestamp() { return QTime::currentTime().toString("[hh:mm:ss]"); }
};

#endif
