#ifndef SUBFILTER_H
#define SUBFILTER_H

#include <QWidget>

#include "input_common.h"

namespace Ui {
    class subfilter;
}

class subfilter : public QWidget {
    Q_OBJECT

public:
    explicit subfilter(QWidget* parent = nullptr);
    ~subfilter();

public:
    void recvcmd(const QString& recv);

private:
    Ui::subfilter* ui;
};

#endif  // SUBFILTER_H
