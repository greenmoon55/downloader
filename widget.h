#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QMetaType>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>
#include <QSize>
#include <QClipboard>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QSystemTrayIcon>
#include <QMenu>

#include "task.h"
#include "newTaskDialog.h"

class Widget : public QWidget
{
    Q_OBJECT
private:
    QVBoxLayout *mainLayout, *tasksLayout;
    DownloadManager *dm;
    QClipboard *clipboard;
    void closeEvent(QCloseEvent *event);
    void showNewTaskDialog(QString str = "");
public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QSize sizeHint() const;
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private slots:
    void showAbout();
    void addTask();
    void addTask(QClipboard::Mode mode);
    void quit();
};

#endif // WIDGET_H
