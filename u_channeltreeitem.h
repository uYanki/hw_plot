#ifndef U_CHANNELTREEITEM_H
#define U_CHANNELTREEITEM_H

#include <QTreeWidget>
#include <QWidget>

#include <QFormLayout>
#include <QHBoxLayout>

#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

// 【QFormLayout】
// https://blog.csdn.net/liang19890820/article/details/51539339
// https://blog.csdn.net/hitzsf/article/details/113516877

class u_ChannelTreeItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
public:
    u_ChannelTreeItem(QTreeWidget* parent, const QString& title);

private:
public:
    QTreeWidgetItem* m_SubItem;
    QFormLayout*     m_layout;

    QPushButton*    m_btn_SaveToCSV;
    QDoubleSpinBox *m_spn_yScale, *m_spn_xOffset, *m_spn_yOffset;
    QDoubleSpinBox *m_spn_min, *m_spn_max;

    QSlider* m_sld_val;
    QDial*   m_dial_val;
};

#endif  // U_CHANNELTREEITEM_H
