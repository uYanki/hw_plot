#include "widget.h"
#include "ui_widget.h"


#include <QPushButton>
#include <QDial>

#include "u_channeltreeitem.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->tree->setRootIsDecorated(true);
    ui->tree->setHeaderHidden(true);
    ui->tree->setIndentation(0);


   u_ChannelTreeItem *chan = new u_ChannelTreeItem(ui->tree,"hello");

   chan->setCheckState(0,Qt::Unchecked);
   chan->setExpanded(true);
   chan->setBackground(0,QBrush(Qt::lightGray));

    QTreeWidgetItem* channel = new QTreeWidgetItem(ui->tree);

   channel->setCheckState(0,Qt::Unchecked);
   channel->setExpanded(true);
   channel->setBackground(0,QBrush(Qt::lightGray));
    channel->setText(0,"channel 1");
    channel->setBackground(0,QBrush(Qt::lightGray));

    // 置控件
    QTreeWidgetItem* chan_sub = new QTreeWidgetItem(channel,QStringList());

    QWidget* mywidget = new QWidget();
    mywidget->setLayout(ui->formLayout);
    ui->tree->setItemWidget(chan_sub,0,mywidget);

    ui->dial->setMaximumSize(40,40);

}

Widget::~Widget()
{
    delete ui;
}

