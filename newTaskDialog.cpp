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
    savePath =new QLabel(tr("save path"));
    savePathBox =new QComboBox(this);
    savePath->setBuddy(savePathBox);
    browse =new QPushButton(tr("&browse..."));
    threadCount =new QLabel("threadcount");
    threadCountBox =new QSpinBox(this);
    threadCountBox->setValue(5);
    threadCountBox->setRange(1,10);
    ok =new QPushButton(tr("&ok"));
    cancel =new QPushButton(tr("&cancel"));
    QHBoxLayout *first =new QHBoxLayout();
    first->addWidget(urlPath);
    first->addWidget(urlLine);
    QHBoxLayout *second =new QHBoxLayout();
    second->addWidget(savePath);
    second->addWidget(savePathBox);
    second->addWidget(browse);
    QHBoxLayout *third =new QHBoxLayout();
    third->addWidget(threadCount);
    third->addWidget(threadCountBox);
    QHBoxLayout *fouth =new QHBoxLayout();
    fouth->addWidget(ok);
    fouth->addWidget(cancel);
    QVBoxLayout *mainLayout =new QVBoxLayout();
    mainLayout->addLayout(first);
    mainLayout->addLayout(second);
    mainLayout->addLayout(third);
    mainLayout->addLayout(fouth);
    this->setWindowTitle(tr("new"));
    setLayout(mainLayout);
    this->savePathBox->setEditable(true);
    this->savePathBox->setEditText(QFSFileEngine::homePath());
    this->ok->setDefault(true);
    connect(browse,SIGNAL(clicked()),this,SLOT(on_Browse_clicked()));
    connect(ok,SIGNAL(clicked()),this,SLOT(on_ok_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
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
        this->savePathBox->setEditText(saveDir.last());
    }
}
void newTaskDialog::on_ok_clicked()
{
    reply=OK;
    this->close();
}
void newTaskDialog::on_cancel_clicked()
{
    reply=CANCEL;
    this->close();
}
