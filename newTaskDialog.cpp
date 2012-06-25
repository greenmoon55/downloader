#include "newTaskDialog.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QFileDialog"
#include <QFSFileEngine>

newTaskDialog::newTaskDialog(QWidget *parent): QDialog(parent)
{
    urlPath =new QLabel(tr("URL"));
    urlLine =new QLineEdit(this);
    urlPath->setBuddy(urlLine);
    savePath =new QLabel(tr("保存到"));
    savePathBox =new QLineEdit(this);
    savePath->setBuddy(savePathBox);
    browse =new QPushButton(tr("浏览(&B)..."));
    threadCount =new QLabel(tr("块数"));
    threadCountBox =new QSpinBox(this);
    threadCountBox->setValue(5);
    threadCountBox->setRange(1, 5);
    ok = new QPushButton(tr("确定(&O)"));
    cancel = new QPushButton(tr("取消(&C)"));
    QHBoxLayout *first =new QHBoxLayout();
    first->addWidget(urlPath);
    first->addWidget(urlLine);
    QHBoxLayout *second =new QHBoxLayout();
    second->addWidget(savePath);
    second->addWidget(savePathBox);
    second->addWidget(browse);
    QHBoxLayout *third = new QHBoxLayout();
    third->addWidget(threadCount);
    third->addWidget(threadCountBox);
    QHBoxLayout *fouth = new QHBoxLayout();
    fouth->addWidget(ok, 1, Qt::AlignRight);
    fouth->addWidget(cancel, 0, Qt::AlignRight);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(first);
    mainLayout->addLayout(second);

    QHBoxLayout *fileNameLayout = new QHBoxLayout();
    saveFileNameEdit = new QLineEdit(this);
    QLabel *fileNameLabel = new QLabel(tr("文件名"));
    fileNameLayout->addWidget(fileNameLabel);
    fileNameLayout->addWidget(saveFileNameEdit);
    fileNameLayout->addWidget(threadCount);
    fileNameLayout->addWidget(threadCountBox);
    this->setWindowTitle(tr("新建任务"));

    mainLayout->addLayout(fileNameLayout);
    //mainLayout->addLayout(third);
    mainLayout->addLayout(fouth);

    setLayout(mainLayout);
    //this->savePathBox->setEditable(true);
    this->savePathBox->setText(QFSFileEngine::homePath());
    this->ok->setDefault(true);
    connect(browse,SIGNAL(clicked()),this,SLOT(on_Browse_clicked()));
    connect(ok,SIGNAL(clicked()),this,SLOT(on_ok_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));

    connect(urlLine, SIGNAL(textChanged(QString)), this, SLOT(autoCompleteFileName(QString)));
}
void newTaskDialog::on_Browse_clicked()
{
    QFileDialog *dialog=new QFileDialog();
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly);
    if (dialog->exec())
        saveDir = dialog->selectedFiles();
    if(!saveDir.isEmpty())
    {
        this->savePathBox->setText(saveDir.last());
    }
}
void newTaskDialog::on_ok_clicked()
{
    url = urlLine->text();
    qDebug() << url;
    saveFileName = saveFileNameEdit->displayText();
    saveDirectory = savePathBox->text();
    saveFile = saveDirectory + "/" + saveFileName;
    qDebug() << saveFile;
    if (QFile::exists(saveFile))
    {
        QMessageBox::warning(this, tr("下载"),
                                        tr("文件已存在"),
                                        QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else qDebug() << "file not exist";
    QStringList list = url.split("://");
    if (list.size() < 2)
    {
        QMessageBox::warning(this, tr("下载"),
                                        tr("抱歉，无法识别您的下载地址"),
                                        QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString protocol = list.first(); // 如果不存在则返回原string
    qDebug() << protocol;
    QString addr = list.at(1);
    if (protocol == "thunder" || protocol == "Thunder")
        url = base64::thunderURL(addr);
    else if (protocol == "flashget")
        url = base64::flashgetURL(addr);
    else if (protocol == "qqdl")
        url = base64::qqdlURL(addr);
    if (url.split("://").first() == "http")
    {
       this->accept();
    }
    else
    {
        QMessageBox m;
        m.setText("Sorry~We can't download this kind of file...T^T");
        m.exec();
    }
}
void newTaskDialog::on_cancel_clicked()
{
    this->reject();
}

QSize newTaskDialog::sizeHint() const
{
    return QSize(500, 200);
}

void newTaskDialog::autoCompleteFileName(QString str)
{
    QStringList list = str.split("/");
    this->saveFileNameEdit->setText(list.last());
}
