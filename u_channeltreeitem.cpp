#include "u_channeltreeitem.h"

u_ChannelTreeItem::u_ChannelTreeItem(QTreeWidget* parent, const QString& title) : QTreeWidgetItem(parent) {
    setText(0, title);

    setCheckState(0, Qt::Unchecked);
    setBackground(0, QBrush(Qt::lightGray));  // crBackground

    // 容器项目
    QWidget* widget = new QWidget(parent);
    widget->setStyleSheet("font:normal normal 9pt SimSum;font-weight:400;");  // 字体

    // 表格布局
    m_layout = new QFormLayout(widget);
    m_layout->setSpacing(10);
    m_layout->setMargin(10);
    m_layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    m_layout->setLabelAlignment(Qt::AlignLeft);
    m_layout->setSizeConstraint(QFormLayout::SetDefaultConstraint);
    m_layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);  // 布局内容对齐
    // m_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint); // 控件宽度适应内容
    m_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    m_SubItem = new QTreeWidgetItem(this, QStringList());
    parent->setItemWidget(m_SubItem, 0, widget);

    // 曲线偏移与增益
    m_layout->addRow(QStringLiteral("yScale:"), m_spn_yScale = new QDoubleSpinBox(widget));
    m_layout->addRow(QStringLiteral("xOffset:"), m_spn_xOffset = new QDoubleSpinBox(widget));
    m_layout->addRow(QStringLiteral("yOffset:"), m_spn_yOffset = new QDoubleSpinBox(widget));

    // 保存曲线数据
    m_layout->addRow(QStringLiteral(""), m_btn_SaveToCSV = new QPushButton("save to csv"));

    // 水平分割线
    QFrame* line;
    m_layout->addWidget(line = new QFrame(widget));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color:lightGray;");
    m_layout->setWidget(m_layout->rowCount() - 1, QFormLayout::ItemRole::SpanningRole, line);  // 整行分布

    // 数值调试
    m_layout->addRow(QStringLiteral("min:"), m_spn_min = new QDoubleSpinBox(widget));
    m_layout->addRow(QStringLiteral("max:"), m_spn_max = new QDoubleSpinBox(widget));
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_sld_val = new QSlider(widget));
    hlayout->addWidget(m_dial_val = new QDial(widget));
    m_layout->addRow(QStringLiteral("value:"), hlayout);
}
