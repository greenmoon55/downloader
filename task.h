#ifndef FILE_H
#define FILE_H
#include <QtGui/QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QUrl>
#include <QTime>
#include <QMetaType>
#include <QIODevice>
#include <QDebug>
#include "downloadmanager.h"

struct TaskInfo
{
    QString url, file;
    qint64 fileSize, totalSize;
};
Q_DECLARE_METATYPE(TaskInfo)

class Task: public QWidget
{
    Q_OBJECT
private:
    DownloadManager *downloadManager;
    QPushButton *startButton, *stopButton, *removeButton;
    QProgressBar *progressBar;
    QTime shortTime;
    QUrl url;
    QFile *file;
    QNetworkReply* reply;
    qint64 fileSize, totalSize;
    void disconnectSignals();
public:
    Task(DownloadManager* dm, QUrl url, QString path, QWidget *parent = 0);
    Task(DownloadManager *downloadManager, TaskInfo *taskInfo, QWidget *parent = 0);
    TaskInfo getTaskInfo();


private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void startDownload();
    void stopDownload();
    void destructor();
    void error(QNetworkReply::NetworkError code);
    void finished();
    void metaDataChanged();
};

#endif // FILE_H
