#include "task.h"

// 显示出错提示，似乎没啥用。。
void Task::errorMsg(QString str)
{
     QMessageBox::warning(this, tr("下载"), str, QMessageBox::Ok, QMessageBox::Ok);
}

// 初始化布局
void Task::initLayout(QString fileName)
{
    QPixmap map1, map2, map3;
    QIcon icon1, icon2, icon3;
    map1.load(":/download.png");
    icon1.addPixmap(map1);
    map2.load(":/pause.png");
    icon2.addPixmap(map2);
    map3.load(":/delete.png");
    icon3.addPixmap(map3);
    startButton = new QPushButton(tr("开始"), this);
    startButton->setIcon(icon1);
    stopButton = new QPushButton(tr("停止"), this);
    stopButton->setIcon(icon2);
    removeButton = new QPushButton(tr("删除"), this);
    removeButton->setIcon(icon3);
    progressBar = new QProgressBar(this);
    //speedLabel = new QLabel(this);
    QHBoxLayout *taskLayout = new QHBoxLayout;
    taskLayout->addWidget(startButton, 0, Qt::AlignLeft);
    taskLayout->addWidget(stopButton, 0, Qt::AlignLeft);
    taskLayout->addWidget(removeButton, 0, Qt::AlignLeft);
    taskLayout->addWidget(progressBar, 1, 0); // fill
    //taskLayout->addWidget(speedLabel, 0, Qt::AlignRight);
    connect(startButton, SIGNAL(clicked()), this, SLOT(startDownload()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDownload()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeTask()));
    stopButton->setEnabled(false);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(taskLayout);
    QHBoxLayout *infoLayout = new QHBoxLayout();
    downloadInfoLabel = new QLabel();
    QLabel *fileNameLabel = new QLabel(fileName);
    infoLayout->addWidget(fileNameLabel, 0, Qt::AlignLeft);
    infoLayout->addWidget(downloadInfoLabel, 0, Qt::AlignRight);
    mainLayout->addLayout(infoLayout);
    this->setLayout(mainLayout);
}

// 用于新建任务
Task::Task(DownloadManager *downloadManager, QUrl url, QString path, qint64 threadCount, QWidget *parent)
    :QWidget(parent), downloadManager(downloadManager), threadCount(threadCount)
{
    initLayout(path);
    qDebug() << "file init"<< endl;
    file = new QFile(path);
    if (!file->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        errorMsg("无法创建文件"+path);
        destructor();
        return;
    }

    for (int i = 0; i < threadCount; i++)
    {
        //stopFileSizes.push_back(0);
        char buf[16];
        sprintf(buf, ".part%d", i);
        QString filenamebuf = file->fileName() + buf;
        qDebug() << filenamebuf;
        files.push_back(new QFile(filenamebuf));
        qDebug()<<"filename:" << files[i]->fileName();
        if (!files.last()->open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            errorMsg("无法创建文件" + files[i]->fileName());
            destructor();
            return;
        }
    }
    this->url = url;
    //this->setToolTip(url.host());
    this->setToolTip(url.toString());
    this->totalSize = 0;
}

// 恢复任务，信息从taskInfo中获得
Task::Task(DownloadManager *downloadManager, TaskInfo *taskInfo, QWidget *parent)
    :QWidget(parent), downloadManager(downloadManager)
{
    initLayout(taskInfo->file);
    qDebug() << "file init"<< endl;
    this->totalSize = taskInfo->totalSize;
    this->threadCount = taskInfo->threadCount;
    this->url = taskInfo->url;
    //this->setToolTip(url.host());
    this->setToolTip(url.toString());

    file = new QFile(taskInfo->file);

    // 下载完了
    qDebug() << "totalsize" << totalSize << "fileSize" << taskInfo->fileSize;
    if (totalSize != 0 && totalSize == taskInfo->fileSize)
    {
        startButton->setEnabled(false);
        progressBar->setValue(100);
        return;
    }

    if (!file->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        errorMsg("无法创建文件" + taskInfo->file);
        destructor();
        return;
    }
    for (int i = 0; i < threadCount; i++)
    {
        //stopFileSizes.push_back(0);
        char buf[16];
        sprintf(buf, ".part%d", i);
        QString filenamebuf = file->fileName() + buf;
        qDebug() << filenamebuf;
        files.push_back(new QFile(filenamebuf));
        qDebug()<<"filename:" << files[i]->fileName();
        if (!files.last()->open(QIODevice::ReadWrite | QIODevice::Append)) // 和上面构造函数的不同点
        {
            errorMsg("无法创建文件" + files[i]->fileName());
            destructor();
            return;
        }
    }    
    //this->fileSize = taskInfo->fileSize;    
    if (totalSize > 0)
    {
        progressBar->setValue(taskInfo->fileSize * 100 / totalSize);
    }
    qDebug() << taskInfo->file << taskInfo->url << taskInfo->fileSize << taskInfo->totalSize;
}

void Task::startDownload()
{
    startButton->setEnabled(false);
    speedTime.start();
    qDebug()<<"startDownload"<<endl;
    //this->fileSize = file->size();
    this->rangeValues.clear();
    this->shortTimes.clear();
    this->fileSizes.clear();
    this->replies.clear();
    this->finisheds.clear();
    this->bytesReceiveds.clear();

    /**
      计算当前已经下载文件大小
      并添加shorttime
      */
    for (int i = 0; i < threadCount; i++)
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

    /**
      记录文件总大小以便分块.
      */
    QNetworkRequest request(url);
    //QNetworkAccessManager manager;
    qDebug() << "Getting the file size...";
    QEventLoop loop;
    QNetworkReply *tmpReply = downloadManager->manager->head(request);

    connect(tmpReply, SIGNAL(finished()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    QVariant var = tmpReply->header(QNetworkRequest::ContentLengthHeader);
    delete tmpReply;
    totalSize = var.toLongLong();
    if (totalSize == 0)
    {
        // 不存在
        errorMsg("无法获取文件");
        startButton->setEnabled(true);
        return;
    }
    qDebug() << "The file size is:" << totalSize;
    if (totalSize == file->size())
    {
        /**
          remove temp files.
          */
        char buf[256];
        sprintf(buf, "rm %s.part*", file->fileName().toUtf8().data());
        system(buf);
        stopButton->setEnabled(false);
        startButton->setEnabled(false);
        progressBar->setValue(100);
        return;
    }
    /**
      计算分块文件大小，开始结束位置，并记录在案
      */
    //设置range value
    for (int i=1; i<threadCount; i++)
    {
        rangeValues.push_back((totalSize/threadCount)*i);
        qDebug() << "totalSize == " << totalSize;
        qDebug() << "rangeValues[" << i-1 << "] == " << rangeValues.last();
        //因为不一定正好整除，rangeValues的最后一个也就没有什么用处。所以少push一个。
    }

    /**
      分块下载
      */
    QVector<QByteArray> rangeHeaderValues;
    QVector<QNetworkRequest> requests;

    // 我整理了一下这里的循环
    if (threadCount == 1)
    {
        rangeHeaderValues.push_back("bytes="+QByteArray::number(fileSizes[0]) + "-");
        qDebug()<<"single thread";
        qDebug()<<"fileSizes[0]==" << fileSizes[0];
    }
    else
    {
        // i == 0
        rangeHeaderValues.push_back("bytes="+QByteArray::number(fileSizes[0]) + "-" + QByteArray::number(rangeValues[0]));
        qDebug()<<"fileSizes[0]==" << fileSizes[0];
        qDebug()<<"rangeValues[0]==" << rangeValues[0];

        /**
          如果不是最后一个线程，从rangeValues中读取数据来setRawHeader
          如果是的话，下载到最后一个字节。
          */
        for (int i = 1; i < threadCount - 1; i++)
        {
            rangeHeaderValues.push_back("bytes="+QByteArray::number(rangeValues[i-1]+fileSizes[i]+1) + "-" + QByteArray::number(rangeValues[i]));
            qDebug()<<"fileSizes["<<i<<"]==" << fileSizes[i];
            qDebug()<<"rangeValues["<<i<<"]==" << rangeValues[i];
        }

        int i = threadCount - 1;
        rangeHeaderValues.push_back("bytes="+QByteArray::number(rangeValues[i - 1]+fileSizes[i]+1) + "-");
        qDebug()<<"fileSizes["<<i<<"]==" << fileSizes[i];
    }

    for (int i = 0; i < threadCount; i++)
    {
        QNetworkRequest tmpRequest(url);
        requests.push_back(tmpRequest);
        requests.last().setRawHeader("Range", rangeHeaderValues[i]);
        MyNetworkReply *tempReply = new MyNetworkReply(i, downloadManager->newDownload(requests[i]));
        replies.push_back(tempReply);
        connect(replies.last(), SIGNAL(myDownloadProgress(qint64, qint64, int)), this, SLOT(myDownloadProgress(qint64,qint64,int)));
        connect(replies.last(), SIGNAL(metaDataChanged(int)), this, SLOT(metaDataChanged(int)));
        connect(replies.last(), SIGNAL(error(QNetworkReply::NetworkError,int)), this, SLOT(error(QNetworkReply::NetworkError,int)));
        connect(replies.last(), SIGNAL(finished(int)), this, SLOT(finished(int)));
        shortTimes[i].start();
    }


    stopButton->setEnabled(true);
//    qDebug()<<"startDownload"<<endl;

    prevAllParts = 0;
}
void Task::stopDownload()
{
    stopButton->setEnabled(false);
    qDebug() << "stopDownload()";

    // disconnect the signal/slot so we don't get any error signal and mess with our error handling code
    // and most importantly it write data to our IODevice here its file which is storing downloaded data.
    disconnectAllSignals();
    for (int i=0; i<threadCount; i++)
    {
        files[i]->write(replies[i]->reply->readAll());
        replies[i]->reply->abort();
        replies[i]->reply->deleteLater();
        //stopFileSizes[i] = files[i]->size();
        qDebug() << i << files[i]->size();
    }
    startButton->setEnabled(true);
}
void Task::myDownloadProgress (qint64 bytesReceived, qint64 bytesTotal, int iPart)
{
    //qDebug()<<"now part: " << iPart << endl;
    //qDebug() << "Download Progress: Received=" <<bytesReceived <<": Total=" << bytesTotal;
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
        //qDebug() << "write" << endl;
        shortTimes[iPart].start();
    }
    if (!totalSize) return; // Avoid "divided by 0"
    qint64 allPart = 0;
    for (int i=0; i<bytesReceiveds.size(); i++)
    {
        allPart += bytesReceiveds[i] + fileSizes[i];
    }
    //qDebug()<<"allPart=" << allPart;
    //qDebug()<<"allTotal=" << allTotal;
    // 计算速度
    qint64 speed;
    if (speedTime.elapsed() > 2000)
    {
        qDebug() << "time elapsed" << speedTime.elapsed();
        speed = (allPart - prevAllParts)/speedTime.elapsed();
        //this->speedLabel->setText(QString::number(speed) + "KB/s");
        /*
        this->downloadInfoLabel->setText(QString::number(allPart/1000) + "KB / " +
                                 QString::number(this->totalSize/1000) + "KB " + QString::number(speed) + "KB/s");
                                 */
        this->downloadInfoLabel->setText(showSize(allPart) + "/" + showSize(totalSize) + " " + showSpeed(speed));
        prevAllParts = allPart;
        speedTime.start();
    }

    //qDebug() << allPart << totalSize;
    int percentage = allPart*100 / totalSize;
    this->progressBar->setValue(percentage);

    if (bytesReceived == bytesTotal)
    {
        qDebug()<<"finish writing";
        files[iPart]->write(replies[iPart]->reply->readAll());
        finisheds[iPart] = true;
        bool allfinished = true;
        for (int i = 0; i < finisheds.size(); i++)
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
}

void Task::destructor()
{
    qDebug() << "deleteLater";
    this->deleteLater();
}

TaskInfo Task::getTaskInfo()
{
    qDebug() << "in getTaskInfo";
    TaskInfo info;
    qDebug() << file->fileName();
    info.file = file->fileName();
    info.fileSize = 0;
    if (file->size()) info.fileSize = file->size();
    else for (int i = 0; i < files.size(); i++) info.fileSize += files[i]->size();
    info.totalSize = this->totalSize;
    info.url = this->url.toString();
    info.threadCount = this->threadCount;
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
    for (int i=0; i<replies.size(); i++)
    {
        replies[iPart]->reply->deleteLater();
    }
    replies.clear();
    removeTempFiles();
    QMessageBox::warning(this, tr("下载"), replies[iPart]->reply->errorString(), QMessageBox::Ok, QMessageBox::Ok);
}

void Task::finished(int iPart)
{
    qDebug() << "finished" << iPart;
    files[iPart]->write(replies[iPart]->reply->readAll()); // 不能省略

    this->disconnectSignals(iPart);
}
void Task::allFinished()
{
    /**
      合并文件
      */
    qDebug() << "all finished";
    this->downloadInfoLabel->setText(tr("下载完成"));
    startButton->setEnabled(false);
    stopButton->setEnabled(false);
    this->progressBar->setValue(100);
    qDebug()<<"allFinished()";
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
        files[i]->close();
    }
    file->close();
    removeTempFiles();
    qDebug() << "totalSize" << totalSize;
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

void Task::removeTempFiles()
{
    char buf[256];
    sprintf(buf, "rm %s.part*", file->fileName().toUtf8().data());
    system(buf);
}

void Task::removeTask()
{
    if (totalSize != this->file->size() || (totalSize==0 && totalSize==0))
    {
        char buf[256];
        sprintf(buf, "rm %s", file->fileName().toUtf8().data());
        system(buf);
    }
    removeTempFiles();
    this->deleteLater();
}

void Task::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu menu;
    menu.addAction(tr("复制下载链接"), this, SLOT(copyURL()));
    menu.exec(event->globalPos());
}

void Task::copyURL()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->disconnect(); // 暂停监控剪切板
    clipboard->setText(this->url.toString());
    emit connectClipboard(); // 恢复监控
}

QString Task::showSize(int bytes)
{
    qDebug() << bytes / 1e9;
    if (bytes / 1000000000 > 0) return QString::number(bytes / 1000000000) + "GB";
    if (bytes / 1000000 > 0) return QString::number(bytes / 1000000) + "MB";
    if (bytes / 1000 > 0) return QString::number(bytes / 1000) + "KB";
    else return QString::number(bytes) + "bytes";
}

QString Task::showSpeed(int bytes)
{
    qDebug() << bytes;
    if (bytes / 1000 > 0) return QString::number(bytes / 1000) + "MB/s";
    else return QString::number(bytes) + "KB/s";
}
