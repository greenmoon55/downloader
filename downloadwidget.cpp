#include "downloadwidget.h"
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

DownloadWidget::DownloadWidget()
{
    mManager = new DownloadManager(this);

    // 这是 Qt 的 signal-slot 机制
    // connect(Object1, signal1, Object2, slot1);
    // 就是说 Object1 中产生信号 signal1 时，调用 Object2 的这个函数
    // 详见 http://qt-project.org/doc/qt-4.8/signalsandslots.html
    connect(mManager,SIGNAL(downloadComplete()),this,SLOT(finished()));
    connect(mManager,SIGNAL(progress(int)),this,SLOT(progress(int)));

    setupUi();
}

// 下面就是UI了

void DownloadWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mProgressBar = new QProgressBar(this);
    mainLayout->addWidget(mProgressBar);

    mDownloadBtn = new QPushButton("Download",this);
    mPauseBtn = new QPushButton("Pause",this);
    mPauseBtn->setEnabled(false);
    mResumeBtn = new QPushButton("Resume",this);
    mResumeBtn->setEnabled(false);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(mDownloadBtn);
    btnLayout->addWidget(mPauseBtn);
    btnLayout->addWidget(mResumeBtn);

    mainLayout->addLayout( btnLayout);

    connect(mDownloadBtn,SIGNAL(clicked()),this,SLOT(download()));
    connect(mPauseBtn,SIGNAL(clicked()),this,SLOT(pause()));
    connect(mResumeBtn,SIGNAL(clicked()),this,SLOT(resume()));
}

// 此时开始下载
void DownloadWidget::download()
{
    mManager->download(QUrl("http://www.students.uni-marburg.de/~Musicc/media/lt-openmusic/01_open_source__magic_mushrooms.ogg"));
    mDownloadBtn->setEnabled(false);
    mPauseBtn->setEnabled(true);
}

void DownloadWidget::pause()
{
    mManager->pause();
    mPauseBtn->setEnabled(false);
    mResumeBtn->setEnabled(true);
}

void DownloadWidget::resume()
{
    mManager->resume();
    mPauseBtn->setEnabled(true);
    mResumeBtn->setEnabled(false);
}


void DownloadWidget::finished()
{
    mDownloadBtn->setEnabled(true);
    mPauseBtn->setEnabled(false);
    mResumeBtn->setEnabled(false);
}

void DownloadWidget::progress(int percentage)
{
    mProgressBar->setValue( percentage);
}
