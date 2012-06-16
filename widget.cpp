#include "widget.h"
#include "QDebug"
#include <QMessageBox>
#include <QFileDialog>
#include "base64.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout;
    dm = new DownloadManager(this);
    QPushButton *addTaskButton = new QPushButton(tr("Add Task"), this);
    connect(addTaskButton, SIGNAL(clicked()), this, SLOT(addTask()));
    mainLayout->addWidget(addTaskButton);
    this->setLayout(mainLayout);
}

Widget::~Widget()
{    
}

void Widget::addTask()
{
    newTaskDialog dlg(this);
    //Shows the dialog as a modal dialog, blocking until the user closes it.
    dlg.exec();
       QString url=dlg.urlLine->text();
   QString aa=url.split("://").first();
   if(dlg.reply==OK)
    {
        if(aa=="thunder" || aa=="Thunder")
            url=base64::thunderURL(url);
        if(aa=="flashget")
            url=base64::flashgetURL(url);
        if(aa=="qqdl")
            url=base64::qqdlURL(url);
    }
    if(dlg.reply==OK)
    {
        if(url.split("://").first()=="http")
        {
           qDebug() << dlg.url << dlg.saveFileName;
           Task *file = new Task(dm, dlg.url, dlg.saveFileName, this);
           mainLayout->addWidget(file);
           this->setLayout(mainLayout);
        }
        else
        {
            QMessageBox m;
            m.setText("Sorry~We can't download this kind of file...T^T    ");
            m.exec();
        }
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    QList<Task *> tasks = this->findChildren<Task *>();
    qDebug() << tasks.length();
    /*
    QList<Task *>::iterator i;
     for (i = tasks.begin(); i != tasks.end(); ++i)
     {
         (*i)->toString();
     }
     */
    qDebug() << "CloseEvent";
}
