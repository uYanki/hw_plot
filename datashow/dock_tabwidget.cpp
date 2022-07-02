#include "dock_tabwidget.h"

dock_tabwidget::dock_tabwidget(QWidget* parent) : QTabWidget(parent),
                                                  m_tabbar(new dock_tabbar(parent)) {
    setTabBar(m_tabbar);

    connect(m_tabbar, &QTabBar::tabCloseRequested, [&](int index) {
        widget(index)->close();
        removeTab(index);  // removeTab 并不会触发 delete
    });

    connect(m_tabbar, SIGNAL(dragout(int)), this, SLOT(dargtab(int)));
}

int dock_tabwidget::add(QWidget* widget, const QString& title, bool fixed) {
    if (widget == nullptr) return -1;
    widget->setWindowTitle(title);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    int index = QTabWidget::addTab(widget, title);
    if (fixed) {
        m_FixedTabs.append(widget);
        m_tabbar->setTabButton(index, QTabBar::RightSide, nullptr);  // 移除关闭按钮
    }
    return index;
}

void dock_tabwidget::dargtab(int index) {
    QWidget* content = widget(index);

    if (m_FixedTabs.contains(content)) return;

    // 显示被拖拽的Tab缩略图

    QPixmap thumbnail = content->grab().scaled(200, 200, Qt::KeepAspectRatio);  // 缩放
    QPixmap snapshot(thumbnail.size() + QSize(2, 21));

    QPainter painter(&snapshot);
    if (painter.isActive()) {
        // 标题
        QRect rcTitle(0, 0, snapshot.width(), 20);
        painter.fillRect(rcTitle, Qt::white);
        painter.drawText(rcTitle.adjusted(5, 2, -5, -2), Qt::AlignLeft | Qt::AlignVCenter, content->windowTitle());
        // 预览图
        painter.drawPixmap(QPoint(1, 20), thumbnail);
        // 边框
        painter.drawRect(snapshot.rect().adjusted(0, 0, -1, -1));
    }
    painter.end();

    QMimeData* mime = new QMimeData;
    QDrag*     drag = new QDrag(m_tabbar);
    drag->setMimeData(mime);
    drag->setPixmap(snapshot);
    drag->setHotSpot(QPoint(10, 10));

    // 隐藏被拖拽的Tab
    removeTab(index);

    connect(drag, &QDrag::destroyed, [=]() {
        if (!m_tabbar->boundary().contains(m_tabbar->mapFromGlobal(QCursor::pos()))) {
            dock_popup* popup = new dock_popup(content, this);
            connect(popup, &dock_popup::dragRelease, [=](const QPoint& pos) {  // 归位
                if (pos == QPoint(-1, -1) || m_tabbar->boundary().contains(m_tabbar->mapFromGlobal(pos))) {
                    auto index = m_tabbar->tabAt(m_tabbar->mapFromGlobal(pos));
                    insertTab(index, content, content->windowTitle());
                    delete popup;  // 这里不能调用 popup->close(), 会触发 dock_popup::endPopup
                }
            });
            connect(popup, &dock_popup::closePopup, [=]() { content->close(); });  // 移除
        } else {
            // 显示先前被隐藏的Tab
            insertTab(index, content, content->windowTitle());
        }
    });

    drag->exec();
}

bool dock_tabwidget::event(QEvent* event) {
    switch (event->type()) {
        case QEvent::MouseButtonDblClick: {
            // 双击Tabbar旁的空白处添加新Tab (双击Tabbar上的Tab时不会触发MouseButtonDblClick)
            if (m_tabbar->contentsRect().height() >=
                mapFromGlobal(QCursor::pos()).y()) {
                emit dblBlank();
            }
            break;
        }
        default: break;
    }

    return QTabWidget::event(event);
}
