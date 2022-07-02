#ifndef TAB_INTERFACES_H
#define TAB_INTERFACES_H

#include <QWidget>

namespace Ui {
    class tab_interfaces;
}

class tab_interfaces : public QWidget {
    Q_OBJECT

public:
    explicit tab_interfaces(QWidget* parent = nullptr);
    ~tab_interfaces();

private:
    bool state = false;

signals:
    void readdata(const QByteArray& recv);
    void readline(const QByteArray& recv);
    void updatestat(size_t         BytesOfRecv,
                    size_t         BytesOfSend,
                    const QString& SpeedOfRecv,
                    const QString& SpeedOfSend);
    void runstate(bool state);

public:
    void senddata(const QByteArray& bytes);

private slots:
    void on_btn_run_clicked();

private:
    Ui::tab_interfaces* ui;
};

#endif  // TAB_INTERFACES_H
