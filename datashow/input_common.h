#ifndef INPUT_COMMON_H
#define INPUT_COMMON_H

#include <QMenu>
#include <QTextEdit>

#include "savefile.h"

class input_common : public QTextEdit {
    Q_OBJECT
public:
    explicit input_common(QWidget* parent = nullptr);

public:
    static QString timestamp() { return QTime::currentTime().toString("[hh:mm:ss]"); }

    // 绑定右键菜单
    void bindMenu(QMenu* menu = nullptr);

    // 文本追加
    void append(const QString& text, const QColor& color = QColor(Qt::black));
    void append(const QByteArray& text, const QColor& color = QColor(Qt::black));

    // 文本保存
    void saveToTXT(void);
    void saveToCSV(void);
};

#endif  // INPUT_COMMON_H
