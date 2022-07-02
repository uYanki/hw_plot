#include "formlayout.h"

formlayout::formlayout(QWidget* parent) : QWidget(parent), layout(new QFormLayout()) {
    setLayout(layout);

    // 表格布局

    layout->setSpacing(5);
    layout->setMargin(10);

    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setLabelAlignment(Qt::AlignLeft);
    layout->setSizeConstraint(QFormLayout::SetDefaultConstraint);
    layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);  // 布局内容对齐
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
}

void formlayout::addSeparator() {
    QFrame* separator = new QFrame(this);
    layout->addRow(separator);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
}
