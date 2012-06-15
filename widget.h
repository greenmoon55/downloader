#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "task.h"
#include "newTaskDialog.h"

class Widget : public QWidget
{
    Q_OBJECT
private:
    QVBoxLayout *mainLayout;
    DownloadManager *dm;
public:
    Widget(QWidget *parent = 0);
    ~Widget();
public slots:
    void addTask();
};

#endif // WIDGET_H
