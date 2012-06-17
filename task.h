#ifndef FILE_H
#define FILE_H
#include <QtGui/QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QUrl>
#include <QTime>
#include <QMetaType>
#include <QIODevice>
#include <QDebug>
#include "downloadmanager.h"

struct TaskInfo
{
    QString url, path, fileName;
    int size, downloadedSize;
};
Q_DECLARE_METATYPE(TaskInfo)

class Task: public QWidget
{
    Q_OBJECT
private:
    DownloadManager *downloadManager;
    QPushButton *startButton, *stopButton, *removeButton;
    QTime shortTime;
    QUrl url;
    QFile *file;
    QNetworkReply* reply;
    void disconnectSignals();
public:
    Task(DownloadManager* dm, QUrl url, QString path, QWidget *parent = 0);
    TaskInfo getTaskInfo();


private slots:
    void downloadProgress (qint64 bytesReceived, qint64 bytesTotal);
    void startDownload();
    void stopDownload();
    void destructor();
    void error(QNetworkReply::NetworkError code);
};

#endif // FILE_H
