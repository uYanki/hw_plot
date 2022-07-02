#include "input_common.h"

QMenu*     m_defaultMenu = nullptr;
QTextEdit* m_focusInput  = nullptr;

input_common::input_common(QWidget* parent) : QTextEdit(parent) {
    if (m_defaultMenu == nullptr) {
        m_defaultMenu = new QMenu();
        m_defaultMenu->addAction("save to txt", [&]() { saveToTXT(); });
        m_defaultMenu->addAction("save to csv", [&]() { saveToCSV(); });
        m_defaultMenu->addSeparator();
        m_defaultMenu->addAction("clear", [&]() { m_focusInput->clear(); });
    }

    setStyleSheet(
        "QTextEdit{ border:0;}\n"
        "QTextEdit:hover{ border:1px solid gray;}\n"
        "QTextEdit:pressed{ border:1px solid gray;}");
}

void input_common::bindMenu(QMenu* menu) {
    QMenu* p = (menu == nullptr) ? m_defaultMenu : menu;
    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &QTextEdit::customContextMenuRequested, [=]() {
        m_focusInput = this;
        p->exec(QCursor::pos());
    });
}

void input_common::saveToTXT() {
    const QString& text = toPlainText();
    if (!text.isEmpty()) savefile(QLatin1String(".txt"), [&](QTextStream& s) { s << text; });
}

void input_common::saveToCSV() {
    const QString& text = toPlainText();
    if (!text.isEmpty()) savefile(QLatin1String(".csv"), [&](QTextStream& s) { s << text; });
}

// 解决默认的 append 自动换行的问题

void input_common::append(const QString& text, const QColor& color) {
    moveCursor(QTextCursor::End);
    setTextColor(color);
    insertPlainText(text);
}

void input_common::append(const QByteArray& text, const QColor& color) {
    insertPlainText(text);
    setTextColor(color);
    moveCursor(QTextCursor::End);
}
