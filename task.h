#ifndef FILE_H
#define FILE_H
#include <QtGui/QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QUrl>
#include <QTime>
#include "downloadmanager.h"

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
public:
    Task(DownloadManager* dm, QUrl url, QString path, QWidget *parent = 0);

private slots:
    void downloadProgress (qint64 bytesReceived, qint64 bytesTotal);
    void startDownload();
    void stopDownload();
    void destructor();
};

#endif // FILE_H
