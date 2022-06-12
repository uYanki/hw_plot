#include "uyk_treeitem_common.h"

uyk_formlayout_w::uyk_formlayout_w(QWidget* parent) : QWidget(parent), m_layout(new QFormLayout()) {
    m_layout->setSpacing(5);
    m_layout->setMargin(10);

    m_layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    m_layout->setLabelAlignment(Qt::AlignLeft);
    m_layout->setSizeConstraint(QFormLayout::SetDefaultConstraint);
    m_layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);  // 布局内容对齐
    // m_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint); // 控件宽度适应内容
    m_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    setLayout(m_layout);
    setStyleSheet(QStr("font:normal normal 9pt SimSum;font-weight:400;"));  // 字体
}

QFormLayout* uyk_formlayout_w::formlayout(void) {
    return m_layout;
}

void uyk_formlayout_w::add_hline() {
    QFrame* p = new QFrame();
    p->setFrameShape(QFrame::HLine);
    p->setFrameShadow(QFrame::Sunken);
    p->setStyleSheet(QStr("color:lightGray;"));
    m_layout->setWidget(m_layout->rowCount(), QFormLayout::ItemRole::SpanningRole, p);
}

QDoubleSpinBox* uyk_formlayout_w::add_dspn(const QString& text, double value) {
    QDoubleSpinBox* p = new QDoubleSpinBox(this);
    m_layout->addRow(text, p);
    // 范围调整
    if (p->minimum() > value)
        p->setMinimum(value);
    else if (p->maximum() < value)
        p->setMaximum(value);
    // 初始值设置
    p->setValue(value);
    return p;
}

void uyk_formlayout_w::add_row(QWidget* w) { m_layout->setWidget(m_layout->rowCount(), QFormLayout::ItemRole::SpanningRole, w); }
void uyk_formlayout_w::add_row(const QString& text, QLayout* l) { m_layout->addRow(text, l); }
void uyk_formlayout_w::add_row(const QString& text, QWidget* w) { m_layout->addRow(text, w); }
