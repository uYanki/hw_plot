#include "page_document.h"
#include "ui_page_document.h"


#include <QDebug>
page_document::page_document(QWidget *parent) :
    datahandler(parent),
    ui(new Ui::page_document)
{
    ui->setupUi(this);


}

page_document::~page_document()
{
    delete ui;
}

void page_document::start()
{
    setAcceptDrops(true);
    datahandler::start();
}

void page_document::stop()
{
    setAcceptDrops(false);
    datahandler::stop();
}


#include <QFile>
#include <QMimeData>
#include <QDragEnterEvent>
void page_document::dragEnterEvent(QDragEnterEvent*event)
{
    auto filename = event->mimeData()->urls()[0].fileName();
    // 仅接收指定后缀文件
   if(filename.endsWith(".txt") || filename.endsWith(".csv"))
       event->acceptProposedAction();
    else
       event->ignore();
}
void page_document::dropEvent(QDropEvent*event)
{
    const QMimeData*qm=event->mimeData();


    QFile file(qm->urls()[0].toLocalFile().toUtf8());

    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        connect(&file, &QFile::readyRead, [&]() {});
        while(!file.atEnd()) recvdata(file.readLine());
        file.close();
    }

}


