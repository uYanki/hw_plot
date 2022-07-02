#ifndef FORMLAYOUT_H
#define FORMLAYOUT_H

#include <QFormLayout>
#include <QFrame>

class formlayout : public QWidget {
public:
    formlayout(QWidget* parent = nullptr);

    void addSeparator(void);
    void addWidget(QWidget* widget) { layout->addRow(widget); };
    void addWidget(const QString& label, QWidget* widget) { layout->addRow(label, widget); };

private:
    QFormLayout* layout;
};

#endif  // FORMLAYOUT_H
