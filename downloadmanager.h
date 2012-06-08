#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTime>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);

signals:
    void downloadComplete();

    void progress( int percentage);

public slots:

    void download(QUrl url);

    void pause();

    void resume();

private slots:

    void download( QNetworkRequest& request );

    void finished();

    void downloadProgress ( qint64 bytesReceived, qint64 bytesTotal );

    void error ( QNetworkReply::NetworkError code );

private:

    QNetworkAccessManager* mManager;
    QNetworkRequest mCurrentRequest;
    QNetworkReply* mCurrentReply;
    QFile* mFile;
    int mDownloadSizeAtPause;

    QTime time;
};

#endif // DOWNLOADMANAGER_H