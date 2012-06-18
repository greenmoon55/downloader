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

QSize Widget::sizeHint() const
{
    return QSize(500, 400);
}

Widget::Widget(QWidget *parent): QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    dm = new DownloadManager(this);
    QPushButton *addTaskButton = new QPushButton(tr("Add Task"), this);
    QPushButton *aboutButton = new QPushButton(tr("About"), this);
    connect(addTaskButton, SIGNAL(clicked()), this, SLOT(addTask()));
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(showAbout()));
    QWidget *topButtons = new QWidget(this);
    QHBoxLayout *topButtonsLayout = new QHBoxLayout(topButtons);
    topButtonsLayout->addWidget(addTaskButton);
    topButtonsLayout->addWidget(aboutButton);
    mainLayout->addWidget(topButtons);
    //this->setLayout(mainLayout);

    qRegisterMetaType<TaskInfo>("TaskInfo");
    qRegisterMetaTypeStreamOperators<TaskInfo>("TaskInfo");

    QSettings settings("TJSSE", "downloader");

    QVariant  tasks;
    tasks = settings.value("tasks");


    QWidget *tasksWidget = new QWidget(this);
    tasksLayout = new QVBoxLayout();


    if (tasks.isValid())
    {
        QList<QVariant> taskInfoList = tasks.toList();
        TaskInfo taskInfo;
        Task *task;
        for (int i = 0; i < taskInfoList.length(); i++)
        {
            taskInfo = taskInfoList[i].value<TaskInfo>();
            task = new Task(dm, &taskInfo, this);
            tasksLayout->addWidget(task);
        }
        tasksLayout->addStretch();
    }
    tasksWidget->setLayout(tasksLayout);
    QScrollArea *sa = new QScrollArea;
    sa->setWidget(tasksWidget);
    sa->setWidgetResizable(true);
    mainLayout->addWidget(sa);
}

Widget::~Widget()
{    
}

void Widget::addTask()
{
    newTaskDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        Task *task = new Task(dm, dlg.url, dlg.saveFile, this);
        tasksLayout->takeAt(tasksLayout->count() - 1);
        tasksLayout->addWidget(task);
        tasksLayout->addStretch();
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
         qDebug() << taskInfo.file << taskInfo.url << taskInfo.fileSize << taskInfo.totalSize;
         taskInfoList.append(qVariantFromValue(taskInfo));
     }
    QSettings settings("TJSSE", "downloader");

    QVariant final(taskInfoList);
    settings.setValue("tasks", final);
    qDebug() << "CloseEvent";
}

void Widget::showAbout()
{
    QMessageBox m;
    m.setText(tr("About"));
    m.exec();
}
