#include "downloadmanager.h"

#include <QDebug>


// DownloadManager 继承自 QObject，调用parent的析构函数后这个也会消失
DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),mCurrentReply(0),mFile(0),mDownloadSizeAtPause(0)
{
    mManager = new QNetworkAccessManager(this);
}

void DownloadManager::download( QUrl url)
{
    qDebug() << "download: URL=" <<url.toString();
    QFileInfo fileInfo(url.toString());
    qDebug() << "filename: " << fileInfo.fileName();

    mDownloadSizeAtPause =0;
    mCurrentRequest = QNetworkRequest(url);
    mFile = new QFile("download.part");
    mFile->open(QIODevice::ReadWrite);

    bytesWrittenToFile = 0;

    download(mCurrentRequest);
}

void DownloadManager::download( QNetworkRequest& request)
{
    qDebug() << "download( QNetworkRequest& request )";
    mCurrentReply = mManager->get(request);

    // 这三个信号都是QNetworkReply自带的信号，都不用我们去写。
    connect(mCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    connect(mCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(mCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    connect(mCurrentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));

    //connect(mCurrentReply, SIGNAL(readyRead()), this, SLOT(writeToFile()));

    // 开始计时
    time.start();
    shortTime.start();
    qDebug() << time.toString();
}

void DownloadManager::pause()
{
    qDebug() << "pause()";
    if(mCurrentReply == 0) {
        return;
    }

    // disconnect the signal/slot so we don't get any error signal and mess with our error handling code
    // and most importantly it write data to our IODevice here its file which is storing downloaded data.
    disconnect(mCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    disconnect(mCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    disconnect(mCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    disconnect(mCurrentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged())); // ???
    //disconnect(mCurrentReply, SIGNAL(readyRead()), this, SLOT(writeToFile()));

    mFile->write(mCurrentReply->readAll());
    qDebug() << mFile->size();
    mCurrentReply->abort();    

    mCurrentReply->deleteLater(); // 我加的
    mCurrentReply = 0;
}

void DownloadManager::resume()
{
    qDebug() << "resume()";

    mDownloadSizeAtPause = mFile->size();

    // Http Header 里放上范围信息，http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(mDownloadSizeAtPause) + "-";
    mCurrentRequest.setRawHeader("Range",rangeHeaderValue);

    download(mCurrentRequest);
}



void DownloadManager::finished()
{
    qDebug() << "finished";
    mFile->write(mCurrentReply->readAll());
    mFile->close();
    if (!mFile->rename("download.part","download.complete")) {
        qDebug() << "rename error" << endl;
    }
    mFile = 0;

    mCurrentReply->deleteLater();

    mCurrentReply = 0;
    emit downloadComplete();
}

// bytesTotal indicates the total number of bytes expected to be downloaded
void DownloadManager::downloadProgress (qint64 bytesReceived, qint64 bytesTotal)
{
    //qDebug() << "Download Progress: Received=" << mDownloadSizeAtPause+bytesReceived <<": Total=" << mDownloadSizeAtPause+bytesTotal;
    /*
    if (mDownloadSizeAtPause+bytesReceived - bytesWrittenToFile > 500000)
    {
        mFile->write(mCurrentReply->readAll());
        bytesWrittenToFile = mFile->size();
        qDebug() << "write" << endl;
    }
    else qDebug() << "continue" << endl;
    */
   // qDebug() << mFile->size() << endl;

    if (shortTime.elapsed() > 3000) {
        qDebug() << (mDownloadSizeAtPause + bytesReceived - mFile->size()) / (double)shortTime.elapsed() << "KB/s" <<  endl;
        mFile->write(mCurrentReply->readAll());
        bytesWrittenToFile = mFile->size();
        qDebug() << "write" << endl;
        shortTime.start();
    }
    //else qDebug() << "continue" << endl;
    int percentage = ((mDownloadSizeAtPause+bytesReceived) * 100 )/ (mDownloadSizeAtPause+bytesTotal);
    //qDebug() << percentage;
    emit progress(percentage);
    // qDebug() << bytesReceived / (double)time.elapsed() << "KB/s" << endl;
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    qDebug() << "Error:"<<code;
}

void DownloadManager::metaDataChanged()
{
    qDebug() << "File size: " << QString(mCurrentReply->rawHeader("Content-Length")) << endl;
}
