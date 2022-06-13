#ifndef UYK_TREEITEM_COMMON_H
#define UYK_TREEITEM_COMMON_H

#define QStr QStringLiteral

#include <QFormLayout>
#include <QFrame>
#include <QWidget>

#include <QDoubleSpinBox>

class uyk_formlayout_w : public QWidget  // 表格布局
{
public:
    uyk_formlayout_w(QWidget* parent = nullptr);

    QFormLayout* formlayout(void);

    void add_hline(void);  // 水平分割线

    void add_row(QWidget* w);
    void add_row(const QString& text, QWidget* w);
    void add_row(const QString& text, QLayout* l);

    QDoubleSpinBox* add_dspn(const QString& text, double value = 0.0);

private:
    QFormLayout* const m_layout;
};

#endif  // UYK_TREEITEM_COMMON_H
