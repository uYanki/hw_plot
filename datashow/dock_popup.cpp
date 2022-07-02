#include "dock_popup.h"

dock_popup::dock_popup(QWidget* content, QWidget* parent) : QDialog(parent), m_content(content) {
    // 去掉标题栏上的问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    // 置窗体内容
    QGridLayout* layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(content);
    content->show();

    // 窗体属性配置
    setAttribute(Qt::WA_DeleteOnClose);  // 窗口关闭时销毁
    setWindowTitle(content->windowTitle());
    resize(content->size());
    move(QCursor::pos() - QPoint(10, 10));
    show();
}

bool dock_popup::event(QEvent* event) {
    switch (event->type()) {
        case QEvent::NonClientAreaMouseButtonDblClick: {  // 双击标题栏时归位

            emit dragRelease(QPoint(-1, -1));
            break;
        }
        case QEvent::NonClientAreaMouseButtonRelease: {  // 拖回tabbar时归位
            QMouseEvent* e = static_cast<QMouseEvent*>(event);
            if (e->button() == Qt::LeftButton)
                emit dragRelease(e->globalPos());
            break;
        }
        case QEvent::Close: {
            emit closePopup(m_content);
            break;
        }
        default: break;
    }
    return QDialog::event(event);
}

void dock_popup::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape: emit closePopup(m_content); break;
        default: QDialog::keyPressEvent(event);
    }
}
