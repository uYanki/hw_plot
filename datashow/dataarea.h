#ifndef DATAAREA_H
#define DATAAREA_H

#include <QWidget>

#include <QInputDialog>
#include <QMenu>

#include <QSpinBox>
#include <QTimer>

#include <QFormLayout>
#include <QWidgetAction>

#include "input_common.h"
#include "savefile.h"
#include "subfilter.h"

namespace Ui {
    class dataarea;
}

class dataarea : public QWidget {
    Q_OBJECT

public:
    explicit dataarea(QWidget* parent = nullptr);
    ~dataarea();

private:
    Ui::dataarea* ui;

    QMenu* const m_MenuOfRecv;
    QMenu* const m_MenuOfSend;

    input_common* const m_input_recv;

    bool m_TimestampMd = false;
    bool m_RawDataMd   = false;

    // 子过滤器
    QList<subfilter*> m_SubFilters;

    // 自动发送
    QMenu*  m_MenuOfSendBtn= nullptr;
    QWidget*  m_CntrAutoSend= nullptr;  // container -> cntr
    QSpinBox* m_SpnInterval = nullptr; // 时间间隔
    QSpinBox* m_SpnTimes = nullptr; // 重复次数
    QTimer*   m_TmrAutoSend  = nullptr;
    void initAutoSend(void);


signals:
    void senddata(const QByteArray& bytes);

public slots:
    void readdata(const QByteArray& recv);
    void readcmd(const QString& cmd);
    void updatestat(size_t         BytesOfRecv,
                    size_t         BytesOfSend,
                    const QString& SpeedOfRecv,
                    const QString& SpeedOfSend);
    void runstate(bool state);



private slots:
    void on_btn_send_clicked();
};

#endif  // DATAAREA_H
