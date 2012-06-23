#include "task.h"
#include <QEventLoop>
// 这个参数应该加个totalSize
Task::Task(DownloadManager *downloadManager, QUrl url, QString path, QWidget *parent)
    :QWidget(parent), downloadManager(downloadManager) // right?
{
    iThreads = 3;
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    removeButton = new QPushButton("Remove", this);
    progressBar = new QProgressBar(this);
    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(startButton);
    fileLayout->addWidget(stopButton);
    fileLayout->addWidget(removeButton);
    fileLayout->addWidget(progressBar);
    connect(startButton, SIGNAL(clicked()), this, SLOT(startDownload()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDownload()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(destructor()));
    stopButton->setEnabled(false);
    this->setLayout(fileLayout);
    qDebug() << "file init"<< endl;
    file = new QFile(path);
    int result = file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    files.clear();
    for (int i=0; i<iThreads; i++)
    {
        char filenamebuf[32];
        sprintf(filenamebuf, "download.part%d", i);
        qDebug() << filenamebuf;
        files.push_back(new QFile(filenamebuf));
        qDebug()<<"filename:" << files[i]->fileName();
        files.last()->open(QIODevice::ReadWrite);
    }
    qDebug() << "file open result" << result;
    this->url = url;
    this->totalSize = 0;
}

Task::Task(DownloadManager *downloadManager, TaskInfo *taskInfo, QWidget *parent)
    :QWidget(parent), downloadManager(downloadManager), totalSize(0)
{
    iThreads = 3;
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    removeButton = new QPushButton("Remove", this);
    progressBar = new QProgressBar(this);
    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(startButton);
    fileLayout->addWidget(stopButton);
    fileLayout->addWidget(removeButton);
    fileLayout->addWidget(progressBar);
    connect(startButton, SIGNAL(clicked()), this, SLOT(startDownload()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDownload()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(destructor()));
    stopButton->setEnabled(false);
    this->setLayout(fileLayout);
    qDebug() << "file init"<< endl;
    file = new QFile(taskInfo->file);
    int result = file->open(QIODevice::Append);
    files.clear();
    for (int i=0; i<iThreads; i++)
    {
        char filenamebuf[32];
        sprintf(filenamebuf, "download.part%d", i);
        qDebug() << filenamebuf;
        files.push_back(new QFile(filenamebuf));
        qDebug()<<"filename:" << files[i]->fileName();
        files.last()->open(QIODevice::ReadWrite);
    }
    qDebug() << "file open result" << result;
    this->url = taskInfo->url;
    //this->fileSize = taskInfo->fileSize;
    this->totalSize = taskInfo->totalSize;
//    if (totalSize > 0)
//    {
//        progressBar->setValue(fileSize * 100 / totalSize);
//    }
    qDebug() << taskInfo->file << taskInfo->url << taskInfo->fileSize << taskInfo->totalSize;
}

void Task::startDownload()
{
    //QUrl url("http://www.students.uni-marburg.de/~Musicc/media/lt-openmusic/01_open_source__magic_mushrooms.ogg");
    qDebug()<<"startDownload"<<endl;
    //this->fileSize = file->size();
    this->rangeValues.clear();
    this->shortTimes.clear();
    this->fileSizes.clear();
    this->replies.clear();
    this->finisheds.clear();
    //this->bytesTotals.clear();
    this->bytesReceiveds.clear();

    /**
      计算当前已经下载文件大小
      并添加shorttime
      */
    for (int i=0; i<iThreads; i++)
    {
        qDebug()<< "i=" << i;
        qDebug() << files.size();
        qint64 tmpSize = files[i]->size();
        qDebug()<< tmpSize << endl;
        fileSizes.push_back(tmpSize);
        shortTimes.push_back(*(new QTime()));
        finisheds.push_back(false);
        bytesReceiveds.push_back(0);
        //bytesTotals.push_back(0);
        qDebug() <<"filesize"<<tmpSize << endl;
        //files[i]->seek(files[i]->size());//文件指针移动到末尾
    }

    qDebug()<<"abc";
    /**
      记录文件总大小以便分块.
      */
    QNetworkRequest request(url);
    QNetworkAccessManager manager;
    qDebug() << "Getting the file size...";
    QEventLoop loop;
    QNetworkReply *tmpReply = manager.head(request);
    connect(tmpReply,SIGNAL(finished()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    QVariant var = tmpReply->header(QNetworkRequest::ContentLengthHeader);
    delete tmpReply;
    totalSize = var.toLongLong();
    qDebug() << "The file size is:" << totalSize;
    /**
      计算分块文件大小，开始结束位置，并记录在案
      */
    //设置range value
    for (int i=1; i<iThreads; i++)
    {
        rangeValues.push_back((totalSize/iThreads)*i);
        qDebug() << "totalSize == " << totalSize;
        qDebug() << "rangeValues[" << i-1 << "] == " << rangeValues.last();
        //因为不一定正好整除，所以最后一块可能多下载一个字节，rangeValues的最后一个也就没有什么用处。所以少push一个。
    }
    /**
      分块下载
      */

    QVector<QByteArray> rangeHeaderValues;
    QVector<QNetworkRequest> requests;
    for (int i=0; i<iThreads; i++)
    {   /**
          如果不是最后一个线程，从rangeValues中读取数据来setRawHeader
          如果是的话，下载到最后一个字节。
          */
        if (i==0&&iThreads!=1){
            rangeHeaderValues.push_back("bytes="+QByteArray::number(fileSizes[i]) + "-" + QByteArray::number(rangeValues[i]));
            qDebug()<<"fileSizes[0]==" << fileSizes[0];
            qDebug()<<"rangeValues[0]==" << rangeValues[0];
        }
        else if (i != iThreads-1){
            rangeHeaderValues.push_back("bytes="+QByteArray::number(rangeValues[i-1]+fileSizes[i]+1) + "-" + QByteArray::number(rangeValues[i]));
            qDebug()<<"fileSizes["<<i<<"]==" << fileSizes[i];
            qDebug()<<"rangeValues["<<i<<"]==" << rangeValues[i];
        }
        else{
            if (i == 0){//单线程
                rangeHeaderValues.push_back("bytes="+QByteArray::number(fileSizes[0]) + "-");
                qDebug()<<"single thread";
                qDebug()<<"fileSizes[0]==" << fileSizes[0];
            }
            else{
                rangeHeaderValues.push_back("bytes="+QByteArray::number(rangeValues[i-1]+fileSizes[i]+1) + "-");
                qDebug()<<"fileSizes["<<i<<"]==" << fileSizes[i];
            }
        }
        QNetworkRequest tmpRequest(url);
        requests.push_back(tmpRequest);
        requests.last().setRawHeader("Range", rangeHeaderValues.last());
        MyNetworkReply *tempReply = new MyNetworkReply(i, downloadManager->newDownload(requests.last()));
        replies.push_back(tempReply);//这句话可能有问题，是否应该为每一块下载都新建一个downloadmanager？
        connect(replies.last(), SIGNAL(myDownloadProgress(qint64, qint64, int)), this, SLOT(myDownloadProgress(qint64,qint64,int)));
        connect(replies.last(), SIGNAL(metaDataChanged(int)), this, SLOT(metaDataChanged(int)));
        connect(replies.last(), SIGNAL(error(QNetworkReply::NetworkError,int)), this, SLOT(error(QNetworkReply::NetworkError,int)));
        connect(replies.last(), SIGNAL(finished(int)), this, SLOT(finished(int)));
        shortTimes[i].start();
    }

    // Http Header 里放上范围信息，http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    // Start from 0
    // 问题: 为什么用QByteArray，而不用QString...
//    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(fileSize) + "-";
//    QNetworkRequest request(url);
//    request.setRawHeader("Range",rangeHeaderValue);
//    reply = downloadManager->newDownload(request);

//    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
//    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
//    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
//    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
//    shortTime.start();
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
//    qDebug()<<"startDownload"<<endl;

}
void Task::stopDownload()
{
    qDebug() << "stopDownload()";

    // disconnect the signal/slot so we don't get any error signal and mess with our error handling code
    // and most importantly it write data to our IODevice here its file which is storing downloaded data.
    disconnectAllSignals();
    for (int i=0; i<iThreads; i++)
    {
        files[i]->write(replies[i]->reply->readAll());
        replies[i]->reply->abort();
        replies[i]->reply->deleteLater();
    }
    //file->write(reply->readAll());
    //qDebug() <<"filesize"<<file->size() << endl;

//    reply->abort();
//    reply->deleteLater();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}
void Task::myDownloadProgress (qint64 bytesReceived, qint64 bytesTotal, int iPart)
{
    qDebug()<<"now part: " << iPart << endl;
    qDebug() << "Download Progress: Received=" <<bytesReceived <<": Total=" << bytesTotal;
   // qDebug() << "fileSize" << file->size();
   // qDebug() << "origionalfileSize + Received" << this->fileSize + bytesReceived;
    //qDebug() << "totalSize" << this->totalSize;
    bytesReceiveds[iPart] = bytesReceived;
    //bytesTotals[iPart] = bytesTotal;
    // 每三秒写一次文件
    if (shortTimes[iPart].elapsed() > 3000) {
        //qDebug() << (mDownloadSizeAtPause + bytesReceived - mFile->size()) / (double)shortTime.elapsed() << "KB/s" <<  endl;
        files[iPart]->write(replies[iPart]->reply->readAll());
        //bytesWrittenToFile = mFile->size();
        qDebug() << "write" << endl;
        shortTimes[iPart].start();
    }
    //else qDebug() << "continue" << endl;
//    for (int i=0; i<iThreads; i++)
//    {
//        allTotal += bytesTotals[iPart];
//    }
    if (!totalSize) return; // Avoid "divided by 0"
    //if (bytesTotal + fileSizes[iPart] == 0) return; // Avoid "divide by 0"
    qint64 allPart = 0;
    qint64 allTotal = totalSize;
    for (int i=0; i<iThreads; i++)
    {
        allPart += bytesReceiveds[iPart];
    }
    qDebug()<<"allPart=" << allPart;
    qDebug()<<"allTotal=" << allTotal;
    int percentage = allPart*100 / allTotal;
    //int percentage = ((bytesReceiveds[iPart] + fileSizes[iPart]) * 100 )/ (bytesTotals[iPart] + fileSizes[iPart]);


    /**
      TODO:progressBar's algorithm should be changed in multi-thread downloading
      */
    this->progressBar->setValue(percentage);
    qDebug() << percentage;
    if (bytesReceived == bytesTotal)
    {
        qDebug()<<"finish writing";
        files[iPart]->write(replies[iPart]->reply->readAll());
    }
    return;
}

void Task::destructor()
{
    qDebug() << "deleteLater";
    this->deleteLater();
}

TaskInfo Task::getTaskInfo()
{
    TaskInfo info;
    qDebug() << file->fileName();
    info.file = file->fileName();
    info.fileSize = file->size();
    info.totalSize = this->totalSize;
    info.url = this->url.toString();
    return info;
}
void Task::disconnectSignals(int iPart)
{
    disconnect(replies[iPart], SIGNAL(metaDataChanged(int)), this, SLOT(metaDataChanged(int)));
    disconnect(replies[iPart], SIGNAL(myDownloadProgress(qint64,qint64,int)), this, SLOT(myDownloadProgress(qint64,qint64,int)));
    disconnect(replies[iPart], SIGNAL(error(QNetworkReply::NetworkError,int)), this, SLOT(error(QNetworkReply::NetworkError,int)));

}

void Task::disconnectAllSignals()
{
    for (int i=0; i<replies.size(); i++)
    {
        disconnect(replies[i], SIGNAL(metaDataChanged(int)), this, SLOT(metaDataChanged(int)));
        disconnect(replies[i], SIGNAL(myDownloadProgress(qint64,qint64,int)), this, SLOT(myDownloadProgress(qint64,qint64,int)));
        disconnect(replies[i], SIGNAL(error(QNetworkReply::NetworkError,int)), this, SLOT(error(QNetworkReply::NetworkError,int)));

    }
}

void Task::error(QNetworkReply::NetworkError code, int iPart)
{
    this->disconnectSignals(iPart);
  //  for (int i=0; i<replies.size(); i++){
        replies[iPart]->reply->deleteLater();
  //  }
  //  replies.clear();
    //reply->deleteLater();
    QMessageBox::warning(this, tr("下载"), replies[iPart]->reply->errorString(), QMessageBox::Ok, QMessageBox::Ok);
}

void Task::finished(int iPart)
{
    /**
      Here we have a problem, at the time when one part is finished and the
      others are not, finisheds cannot do the finished judging job
      TODO
      */
    finisheds[iPart] = true;
    //startButton->setEnabled(false);
    //stopButton->setEnabled(false);
    qDebug() << "finished" << iPart;
    files[iPart]->write(replies[iPart]->reply->readAll()); // 不能省略
    this->disconnectSignals(iPart);
    bool allfinished = true;
    for (int i=0; i<finisheds.size(); i++)
    {
        if (finisheds[i] == false)
        {
            allfinished = false;
            break;
        }
    }
    if (allfinished)
        allFinished();
}
void Task::allFinished()
{
    /**
      We have another problem here, that if we click stop button
      this function would be executed.
      Don't know how to do this job.
      TODO.
      */
    /**
      We do the file concation job and all the finished jobs here.
      */
    //this->progressBar->setValue(100);
    qDebug()<<"allFinished()";
    //QFile* theFile = new QFile("download.result");
    //theFile->open(QIODevice::ReadWrite);
    file->resize(0);
    file->seek(0);
    for (int i=0; i<files.size(); i++)
    {
        //first, every temp file seek to the head;
        files[i]->seek(0);
        //second, cycling read to buffer and write to the finished file
        while (!files[i]->atEnd())
        {
            const int iMaxLength = 0x10000;
            QByteArray tmpArray =
            files[i]->read( iMaxLength );
            file->write(tmpArray);
        }
        //files[i]->close();
    }
    //file->close();
    /**
      remove temp files. File names need to be changed
      in order to download many files at the same time.
      TODO.
      */
    //system("rm download.part*");
}

void Task::metaDataChanged(int iPart)
{
//    disconnect(replies[iPart], SIGNAL(metaDataChanged(int)), this, SLOT(metaDataChanged(int)));
//    qDebug() << replies[iPart]->rawHeader("Content-Length");
//    int contentLength = reply->rawHeader("Content-Length").toLongLong();
//    qDebug() << "length" << contentLength;
//    if (this->totalSize == 0) this->totalSize = contentLength;
//    else if (totalSize - fileSize != contentLength)
//    {
//        disconnectSignals();
//        reply->deleteLater();
//        QMessageBox::warning(this, tr("下载"), tr("服务器文件大小已改变，不能继续下载。"),
//                             QMessageBox::Ok, QMessageBox::Ok);
//    }
//    qDebug() << "metaDataChanged" << totalSize;
}
