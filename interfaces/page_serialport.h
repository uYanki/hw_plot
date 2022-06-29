#ifndef PAGE_SERIALPORT_H
#define PAGE_SERIALPORT_H

#include <QComboBox>
#include <QMessageBox>
#include <QWidget>

#include <QSerialPort>
#include <QSerialPortInfo>

#include "datahandler.h"

namespace Ui {
    class page_serialport;
}

class page_serialport : public datahandler {
    Q_OBJECT

public:
    explicit page_serialport(QWidget* parent = nullptr);
    ~page_serialport();

public:
    QSerialPort* const m_SerialPort;

    bool start(void) Q_DECL_OVERRIDE;
    void stop(void) Q_DECL_OVERRIDE;
    void senddata(const QByteArray& bytes) Q_DECL_OVERRIDE;

    void scan(void);  // scan available ports

private:
    Ui::page_serialport* ui;

    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
};

#endif  // PAGE_SERIALPORT_H
