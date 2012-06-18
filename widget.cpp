#include "widget.h"

QDataStream &operator<<(QDataStream &out, const TaskInfo &obj)
{
     out << obj.url << obj.file << obj.fileSize << obj.totalSize;
     return out;
}

QDataStream &operator>>(QDataStream &in, TaskInfo &obj)
{
    in >> obj.url >> obj.file >> obj.fileSize >> obj.totalSize;
    return in;
}


Widget::Widget(QWidget *parent): QWidget(parent)
{
    mainLayout = new QVBoxLayout;
    dm = new DownloadManager(this);
    QPushButton *addTaskButton = new QPushButton(tr("Add Task"), this);
    connect(addTaskButton, SIGNAL(clicked()), this, SLOT(addTask()));
    mainLayout->addWidget(addTaskButton);
    this->setLayout(mainLayout);

    qRegisterMetaType<TaskInfo>("TaskInfo");
    qRegisterMetaTypeStreamOperators<TaskInfo>("TaskInfo");

    QSettings settings("TJSSE", "downloader");

    QVariant  tasks;
    tasks = settings.value("tasks");
    if (tasks.isValid())
    {
        QList<QVariant> taskInfoList = tasks.toList();
        TaskInfo taskInfo;
        Task *task;
        for (int i = 0; i < taskInfoList.length(); i++)
        {
            taskInfo = taskInfoList[i].value<TaskInfo>();
            task = new Task(dm, &taskInfo, this);
            mainLayout->addWidget(task);
        }
    }
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
    mainLayout->addStretch();
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
         qDebug() << taskInfo.file << taskInfo.url << taskInfo.fileSize << taskInfo.totalSize;
         taskInfoList.append(qVariantFromValue(taskInfo));
     }
    QSettings settings("TJSSE", "downloader");

    QVariant final(taskInfoList);
    settings.setValue("tasks", final);
    qDebug() << "CloseEvent";
}
