#ifndef newTaskDialog_H
#define newTaskDialog_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStringList>
#include <QSpinBox>
#include <QUrl>
#include <QStringList>
#include <QMessageBox>
#include "base64.h"
#include <QDebug>

class newTaskDialog: public QDialog
{
    Q_OBJECT
public:
    newTaskDialog(QWidget *parent);
    QLabel *urlPath;
    QLabel *threadCount;
    QSpinBox *threadCountBox;
    QLineEdit *urlLine;
    QLabel *savePath;
    QLineEdit *savePathBox;
    QPushButton *browse;
    QPushButton *ok;
    QPushButton *cancel;
    QStringList saveDir;
    //bool reply;
    QString url, saveFileName, saveDirectory;
    QString saveFile; // 目录+文件名
    QLineEdit *saveFileNameEdit;
    QSize sizeHint() const;
private slots:
    void on_Browse_clicked();
    void on_ok_clicked();
    void on_cancel_clicked();
};

#endif // newTaskDialog_H
