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
    ~datahandler();

    //////////////////////////////////////////////

protected:
    QTimer* m_TmrSpeedCalc = nullptr;

public:
    // 字节数统计

    size_t m_LastBytesOfRecv = 0;
    size_t m_LastBytesOfSend = 0;
    size_t m_BytesOfRecv     = 0;
    size_t m_BytesOfSend     = 0;

    // 传输速率 (kiloByte per second, kBps)

    QString m_SpeedOfRecv;
    QString m_SpeedOfSend;

    virtual bool start();
    virtual void stop();

signals:
    void update(void);

    //////////////////////////////////////////////

private:
    QByteArray m_buffer;  // 缓冲区

protected:
    void recvdata(const QByteArray& bytes);

signals:
    void readdata(const QByteArray& bytes);
    void readline(const QByteArray& bytes);
    void readcmd(const QByteArray& bytes);

protected:
    virtual void senddata(const QByteArray& bytes);

    //////////////////////////////////////////////

public:
    static QString timestamp() { return QTime::currentTime().toString("[hh:mm:ss]"); }
};

#endif
