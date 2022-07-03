#ifndef PAGE_DOCUMENT_H
#define PAGE_DOCUMENT_H

#include <QWidget>

#include "datahandler.h"

namespace Ui {
class page_document;
}

class page_document : public datahandler
{
    Q_OBJECT

public:
    explicit page_document(QWidget *parent = nullptr);
    ~page_document();

    void start(void) Q_DECL_OVERRIDE;
    void stop(void) Q_DECL_OVERRIDE;


private:
    Ui::page_document *ui;

    void dragEnterEvent(QDragEnterEvent*event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent*event) Q_DECL_OVERRIDE;

};

#endif // PAGE_DOCUMENT_H
