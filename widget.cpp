#include "widget.h"

QDataStream &operator<<(QDataStream &out, const TaskInfo &obj)
{
     out << obj.url << obj.file << obj.fileSize << obj.totalSize << obj.threadCount;
     return out;
}

QDataStream &operator>>(QDataStream &in, TaskInfo &obj)
{
    in >> obj.url >> obj.file >> obj.fileSize >> obj.totalSize >> obj.threadCount;
    return in;
}

QSize Widget::sizeHint() const
{
    return QSize(500, 400);
}

Widget::Widget(QWidget *parent): QWidget(parent)
{
    this->setWindowTitle(tr("下载管理器"));
    // 界面及读取设置
    mainLayout = new QVBoxLayout(this);
    dm = new DownloadManager(this);
    QPushButton *addTaskButton = new QPushButton(tr("新建任务"), this);
    QPushButton *aboutButton = new QPushButton(tr("关于"), this);
    QPushButton *allBeginButton = new QPushButton(tr("全部开始"), this);
    QPushButton *allPauseButton = new QPushButton(tr("全部暂停"));
    connect(addTaskButton, SIGNAL(clicked()), this, SLOT(addTask()));
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(showAbout()));    
    connect(allBeginButton, SIGNAL(clicked()), this, SLOT(allBegin()));
    connect(allPauseButton, SIGNAL(clicked()), this, SLOT(allPause()));


    QWidget *topButtons = new QWidget(this);
    QHBoxLayout *topButtonsLayout = new QHBoxLayout(topButtons);
    topButtonsLayout->addWidget(addTaskButton);
    topButtonsLayout->addWidget(allBeginButton);
    topButtonsLayout->addWidget(allPauseButton);
    topButtonsLayout->addStretch();
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
            connect(task, SIGNAL(connectClipboard()), this, SLOT(connectClipboard()));
        }
        tasksLayout->addStretch();
    }
    tasksWidget->setLayout(tasksLayout);
    QScrollArea *sa = new QScrollArea;
    sa->setWidget(tasksWidget);
    sa->setWidgetResizable(true);
    mainLayout->addWidget(sa);

    // 剪切板
    clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(addTask(QClipboard::Mode)));

    // 支持拖拽
    setAcceptDrops(true);

    // 系统托盘
    QIcon icon(":/download.png");
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(icon, this);
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("新建下载"), this, SLOT(addTask()));
    menu->addAction(tr("退出"), this, SLOT(quit()));
    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

Widget::~Widget()
{    
}

void Widget::showNewTaskDialog(QString str)
{
    //if (str.isEmpty())
    newTaskDialog dlg(this);
    dlg.urlLine->setText(str);
    if (dlg.exec() == QDialog::Accepted)
    {
        Task *task = new Task(dm, dlg.url, dlg.saveFile, dlg.threadCountBox->value(), this); // 暂时定为5块
        connect(task, SIGNAL(connectClipboard()), this, SLOT(connectClipboard()));
        tasksLayout->takeAt(tasksLayout->count() - 1);
        tasksLayout->addWidget(task);
        tasksLayout->addStretch();
    }
}

void Widget::addTask()
{
    newTaskDialog dlg(this);
    showNewTaskDialog();
}

void Widget::addTask(QClipboard::Mode mode)
{
    if (mode == QClipboard::Clipboard && clipboard->text().contains("://"))
    {
        showNewTaskDialog(clipboard->text());
    }
}


void Widget::closeEvent(QCloseEvent *event)
{
    QList<Task *> tasks = this->findChildren<Task *>();
    qDebug() << tasks.length();
    qDebug() << "closeEvent";
    QList<Task *>::iterator i;
    TaskInfo taskInfo;
    QList<QVariant> taskInfoList;
     for (i = tasks.begin(); i != tasks.end(); ++i)
     {
         qDebug() << "in loop";
         taskInfo = (*i)->getTaskInfo();
         qDebug() << "afterTaskInfo";
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
    m.setWindowTitle(tr("下载管理器"));
    m.setText(tr("下载管理器V1.1\n作者：董金、辛田、杨柳杉、段兆宇"));
    m.exec();
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void Widget::dropEvent(QDropEvent *event)
{
    showNewTaskDialog(event->mimeData()->text());
}

void Widget::quit()
{
    this->close();
}

void Widget::connectClipboard()
{
    connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(addTask(QClipboard::Mode)));
}

void Widget::allBegin()
{
    QList<Task *> tasks = this->findChildren<Task *>();
    qDebug() << "allBegin";
    QList<Task *>::iterator i;
     for (i = tasks.begin(); i != tasks.end(); ++i)
     {
         if((*i)->startButton->isEnabled())
              (*i)->startDownload();
     }
    qDebug() << "allBegin";
}
void Widget::allPause()
{

    QList<Task *> tasks = this->findChildren<Task *>();
    qDebug() << "allPause";
    QList<Task *>::iterator i;
     for (i = tasks.begin(); i != tasks.end(); ++i)
     {
         qDebug() << "task i";
         if((*i)->stopButton->isEnabled())
             (*i)->stopDownload();
     }
    qDebug() << "allPause";

}
