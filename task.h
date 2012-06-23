#ifndef TASK_H
#define TASK_H
#include <QtGui/QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QUrl>
#include <QTime>
#include <QMetaType>
#include <QIODevice>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QEventLoop>
#include <QLabel>
#include "MyNetworkReply.h"
#include "downloadmanager.h"

struct TaskInfo
{
    QString url, file;
    qint64 fileSize, totalSize, threadCount;
};
Q_DECLARE_METATYPE(TaskInfo)

class Task: public QWidget
{
    Q_OBJECT
private:
    DownloadManager *downloadManager;
    QPushButton *startButton, *stopButton, *removeButton;
    QProgressBar *progressBar;
    QLabel *speedLabel;

    qint64 threadCount;
    qint64 prevAllParts;
    QTime speedTime;
    QVector<QTime> shortTimes;
    //QTime shortTime;
    QUrl url;
    QVector<QFile*> files;
    QFile *file;
    QVector<bool> finisheds;
    QVector<MyNetworkReply*> replies;
    QVector<QNetworkRequest> requests;
    /**
      Progress bar algorithm handlers
      both
      */
    QVector<qint64> bytesReceiveds;
    //QVector<qint64> stopFileSizes;
    //QNetworkReply* reply;
    QVector<qint64> fileSizes; // 在 startDownload() 更新
    //qint64 fileSize
    qint64 totalSize;
    QVector<qint64> rangeValues;
    void disconnectAllSignals();
    void disconnectSignals(int iPart);
    void allFinished();
    void initLayout();
    void errorMsg(QString str);
    void removeTempFiles();
public:
    Task(DownloadManager *downloadManager, QUrl url, QString path, qint64 threadCount, QWidget *parent = 0);
    Task(DownloadManager *downloadManager, TaskInfo *taskInfo, QWidget *parent = 0);
    TaskInfo getTaskInfo();


private slots:
    void myDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, int iPart);
    //void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void startDownload();
    void stopDownload();
    void destructor();
    void error(QNetworkReply::NetworkError code, int iPart);
    void finished(int iPart);
    void metaDataChanged(int iPart);
};

#endif // TASK_H
