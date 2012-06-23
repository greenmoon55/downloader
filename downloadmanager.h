#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QTime>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    QNetworkReply* newDownload(QNetworkRequest request);

signals:

public slots:


private slots:


public:
    QNetworkAccessManager* manager;
};

#endif // DOWNLOADMANAGER_H
