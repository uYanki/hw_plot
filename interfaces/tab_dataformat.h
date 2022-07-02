#ifndef TAB_DATAFORMAT_H
#define TAB_DATAFORMAT_H

#include <QMenu>
#include <QWidget>

#include "treeitem_datafilter.h"

namespace Ui {
    class tab_dataformat;
}

class tab_dataformat : public QWidget {
    Q_OBJECT

public:
    explicit tab_dataformat(QWidget* parent = nullptr);
    ~tab_dataformat();

private:
    Ui::tab_dataformat* ui;

private:
    QMenu* const m_Menu = new QMenu(this);

    QList<treeitem_datafilter*> m_filters;

     QString m_prefix;
     QString m_suffix;
     QString m_delimiter;

signals:
    void readcmd(const QByteArray& cmd);
    void readvals(const QVector<double>& values);
public slots:
    void readline(const QByteArray& bytes);
private:
    void handleCmd(const QByteArray& bytes);

private:
    bool filter(const QByteArray& bytes);


};

#endif  // TAB_DATAFORMAT_H
