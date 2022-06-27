#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QTimer>
#include <QShortcut>

#include <QSplitter>

#include <QMenu>
#include <QComboBox>



#include "uyk_savefile.h"
#include "uyk_treeitem_channel.h"
#include "uyk_treeitem_command.h"

#include "uyk_custom_action.h"

#include "tab_interfaces.h"
#include "tab_dataformat.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


QString strmid(const QByteArray& text,const QString& left,const QString& right);

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initUI(void);

    /************** interfaces **************/



    // 图表

    QVector<uyk_treeitem_channel*> m_channels;
    QMenu* m_MenuChannel = nullptr; // @ tree_channel
    void initChanTree(void);



    QMenu* m_MenuOfRecv    = nullptr;  // @ input_recv
    QMenu* m_MenuOfSend    = nullptr;  // @ input_send
    QMenu* m_MenuOfSendBtn = nullptr;  // @ btn_send


    // 连续发送
    QWidget*  m_CntrRepeatSend;  // container -> cntr
    QSpinBox* m_SpnRepeatDelay = nullptr;
    QSpinBox* m_SpnRepeatTimes = nullptr;

    bool m_RawDataMd=false;


    tab_interfaces* m_interfaces;
    void initInterfaces(void);

    tab_dataformat* m_dataformat;


    // @ input_recv
    bool m_TimestampMd=false; // 时间戳
    void appendText(const QString& content);

private:
    Ui::Widget *ui;
};


#endif // WIDGET_H
