#ifndef DOCK_POPUP_H
#define DOCK_POPUP_H

#include <QDialog>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>

class dock_popup : public QDialog {
    Q_OBJECT
public:
    explicit dock_popup(QWidget* content, QWidget* parent = nullptr);

    QWidget* const m_content;

protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

signals:
    void dragRelease(const QPoint& pos);
    void closePopup(QWidget*);
};

#endif  // dock_popup_H
