#include "task.h"

Task::Task(DownloadManager *downloadManager, QUrl url, QString path, QWidget *parent)
    :QWidget(parent), downloadManager(downloadManager) // right?
{
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    removeButton = new QPushButton("Remove", this);
    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(startButton);
    fileLayout->addWidget(stopButton);
    fileLayout->addWidget(removeButton);
    connect(startButton, SIGNAL(clicked()), this, SLOT(startDownload()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDownload()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(destructor()));
    stopButton->setEnabled(false);
    this->setLayout(fileLayout);
    qDebug() << "file init"<< endl;
    file = new QFile(path);
    file->open(QIODevice::ReadWrite);
    this->url = url;
}

void Task::startDownload()
{
    //QUrl url("http://www.students.uni-marburg.de/~Musicc/media/lt-openmusic/01_open_source__magic_mushrooms.ogg");
    qDebug()<<"startDownload"<<endl;
    qint64 size = file->size();
    qDebug() <<"filesize"<<size << endl;

    // Http Header 里放上范围信息，http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    // Start from 0
    // 问题: 为什么用QByteArray，而不用QString...
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(size) + "-";
    QNetworkRequest request(url);
    request.setRawHeader("Range",rangeHeaderValue);
    reply = downloadManager->newDownload(request);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    shortTime.start();
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    qDebug()<<"startDownload"<<endl;

}
void Task::stopDownload()
{
    qDebug() << "stopDownload()";

    // disconnect the signal/slot so we don't get any error signal and mess with our error handling code
    // and most importantly it write data to our IODevice here its file which is storing downloaded data.
    //disconnect(reply,SIGNAL(finished()),this,SLOT(finished()));
    disconnect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    //disconnect(mCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    //disconnect(mCurrentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged())); // ???
    //disconnect(mCurrentReply, SIGNAL(readyRead()), this, SLOT(writeToFile()));

    file->write(reply->readAll());
    qDebug() <<"filesize"<<file->size() << endl;

    reply->abort();
    reply->deleteLater();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}
void Task::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    qDebug()<<"Task::downloadProgress";
    qDebug() << "Download Progress: Received=" <<bytesReceived <<": Total=" << bytesTotal;
    // 每三秒写一次文件
    if (shortTime.elapsed() > 3000) {
        //qDebug() << (mDownloadSizeAtPause + bytesReceived - mFile->size()) / (double)shortTime.elapsed() << "KB/s" <<  endl;
        file->write(reply->readAll());
        //bytesWrittenToFile = mFile->size();
        qDebug() << "write" << endl;
        shortTime.start();
    }
    //else qDebug() << "continue" << endl;
    if (bytesTotal == 0) return;
    int percentage = ((bytesReceived) * 100 )/ (bytesTotal);
    qDebug() << percentage;
    return;
}

void Task::destructor()
{
    qDebug() << "deleteLater";
    this->deleteLater();
}

TaskInfo Task::getTaskInfo()
{
    TaskInfo info = {"ac","bc","cc",0,1};
    return info;
}

void Task::disconnectSignals()
{
    disconnect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    disconnect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
}

void Task::error(QNetworkReply::NetworkError code)
{
    this->disconnectSignals();
    reply->deleteLater();
    qDebug() << reply->errorString();
}
