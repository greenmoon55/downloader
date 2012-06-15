#include "widget.h"
#include "QDebug"

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
    Task *file = new Task(dm, this);
    mainLayout->addWidget(file);
    this->setLayout(mainLayout);
}