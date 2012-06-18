#include "widget.h"

QDataStream &operator<<(QDataStream &out, const TaskInfo &obj)
{
     out << obj.url << obj.path << obj.fileName << obj.size << obj.fileSize;
     return out;
}

QDataStream &operator>>(QDataStream &in, TaskInfo &obj)
{
    in >> obj.url >> obj.path >> obj.fileName >> obj.size >> obj.fileSize;
    return in;
}


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    qRegisterMetaType<TaskInfo>("TaskInfo");
    qRegisterMetaTypeStreamOperators<TaskInfo>("TaskInfo");

    QSettings settings("TJSSE", "downloader");
    //settings.setValue("tasks", 1);

    QVariant  final;
    final = settings.value("custom");
    if (final.isValid())
    {
        QList<QVariant> test = final.toList();
        if (test.length()>2)
        {
            qDebug() << test.length();
            TaskInfo contact = test[0].value<TaskInfo>();
                    qDebug() << contact.size << contact.fileSize;
            contact = test[1].value<TaskInfo>();
            qDebug() << contact.size << contact.fileSize;
        }
    }


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
    if (dlg.exec() == QDialog::Accepted)
    {
        Task *task = new Task(dm, dlg.url, dlg.saveFile, this);
        mainLayout->addWidget(task);
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    QList<Task *> tasks = this->findChildren<Task *>();
    qDebug() << tasks.length();
    QList<Task *>::iterator i;
    TaskInfo taskInfo;
    QList<QVariant> taskInfoList;
     for (i = tasks.begin(); i != tasks.end(); ++i)
     {
         taskInfo = (*i)->getTaskInfo();
         taskInfoList.append(qVariantFromValue(taskInfo));
     }
    QSettings settings("TJSSE", "downloader");

    QVariant final(taskInfoList);
    settings.setValue("custom", final);
    qDebug() << "CloseEvent";
}
