#ifndef MYNETWORKREPLY_H
#define MYNETWORKREPLY_H
/**
  为了知道每一个信号是第几个part发送的，所以需要一个新类
  */
#include <QNetworkReply>
class MyNetworkReply : public QObject
{
    Q_OBJECT
public:
    QNetworkReply* reply;
    explicit MyNetworkReply(int iPart, QNetworkReply* reply): iPart(iPart),reply(reply)
    {
        connect(reply,
                SIGNAL(downloadProgress(qint64,qint64)),
                this,
                SLOT(myDownloadProgress(qint64,qint64)));
        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(finished()));
        connect(reply,
                SIGNAL(metaDataChanged()),
                this,
                SLOT(metaDataChanged()));
        connect(reply,
                SIGNAL(error(QNetworkReply::NetworkError)),
                this,
                SLOT(error(QNetworkReply::NetworkError)));
    }
    ~MyNetworkReply(){
        disconnect(reply,
                   SIGNAL(downloadProgress(qint64,qint64)),
                   this,
                   SLOT(myDownloadProgress(qint64,qint64)));
        reply->deleteLater();
    }

    void setiPart(int iPart)
    {
        this->iPart = iPart;
    }

signals:
    void myDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, int iPart);
    void finished(int iPart);
    void metaDataChanged(int iPart);
    void error(QNetworkReply::NetworkError err, int iPart);
public slots:
    void myDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
    {
        emit myDownloadProgress(bytesReceived, bytesTotal, this->iPart);
    }
    void finished(){
        emit finished(this->iPart);
    }
    void metaDataChanged(){
        emit metaDataChanged(this->iPart);
    }
    void error(QNetworkReply::NetworkError err){
        emit (error(err, this->iPart));
    }

    //    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    //    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    //    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
private slots:

private:
    int iPart;
};

#endif // MYNETWORKREPLY_H
