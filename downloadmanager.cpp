#include "downloadmanager.h"

#include <QDebug>


// DownloadManager 继承自 QObject，调用parent的析构函数后这个也会消失
DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

QNetworkReply* DownloadManager::newDownload(QNetworkRequest request)
{
    QNetworkReply *reply = manager->get(request);
    qDebug() << "DownloadManager::newDownload";
    return reply;
}

