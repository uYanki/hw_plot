#include "uyk_custom_action.h"
#include <QLabel>
#include <QSplitter>

uyk_custom_action::uyk_custom_action(std::function<QWidget*(QWidget* parent)> cbk_init, QWidget* parent) : QWidgetAction(parent), m_cbk_init(cbk_init) {}

QWidget* uyk_custom_action::createWidget(QWidget* parent) {
    if (parent->inherits("QMenu") || parent->inherits("QToolBar"))
        return m_cbk_init(parent);
    // example: cbk = [&](QWidget* parent)->QWidget*{return new QPushButton("hello",parent);}
}
