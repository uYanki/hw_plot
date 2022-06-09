#ifndef UYK_TREEITEM_CHAN_H
#define UYK_TREEITEM_CHAN_H

#include <QTreeWidget>
#include <QWidget>

#include <QFormLayout>
#include <QHBoxLayout>

#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

// channel -> chan

class uyk_treeitem_chan: public QObject, public QTreeWidgetItem
{

public:
    uyk_treeitem_chan(QTreeWidget* parent, const QString& title);

public:
    QTreeWidgetItem* m_SubItem;
    QFormLayout*     m_layout;

    QPushButton*    m_btn_SaveToCSV;
    QDoubleSpinBox *m_spn_yScale, *m_spn_xOffset, *m_spn_yOffset;
    QDoubleSpinBox *m_spn_min, *m_spn_max;

    QSlider* m_sld_val;
    QDial*   m_dial_val;
};

#endif // UYK_TREEITEM_CHAN_H
