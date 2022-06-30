#ifndef DATAAREA_H
#define DATAAREA_H

#include <QWidget>

namespace Ui {
class dataarea;
}

class dataarea : public QWidget
{
    Q_OBJECT

public:
    explicit dataarea(QWidget *parent = nullptr);
    ~dataarea();

private:
    Ui::dataarea *ui;
};

#endif // DATAAREA_H
