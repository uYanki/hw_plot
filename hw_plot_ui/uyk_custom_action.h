#ifndef MYACTION_H
#define MYACTION_H

#include <QWidget>
#include <QWidgetAction>

// 菜单里添加控件

class uyk_custom_action : public QWidgetAction {
    Q_OBJECT
public:
    explicit uyk_custom_action(std::function<QWidget*(QWidget* parent)> cbk_init /*控件初始化函数*/, QWidget* parent = nullptr);

protected:
    QWidget* createWidget(QWidget* parent);

private:
    std::function<QWidget*(QWidget* parent)> m_cbk_init = nullptr;
};

#endif  // MYACTION_H
