#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dkmemmoryview.h"
#include <qextserialenumerator.h>
#include <qextserialport.h>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

namespace Ui {
class MainWindow;
}

struct MemRecord {
    uint32_t ptr;
    uint32_t size;
    QString task;
    QDateTime creationTime;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void processData();
    void showMemInfo(uint32_t addr = 0xFFFFFFFF);
    void deviceListChanged();
    
    void on_pushButtonConn_clicked();

private:
    Ui::MainWindow *ui;
    DKMemmoryView *memView;
    QextSerialPort *port_dev;
    QextSerialEnumerator *port_enum;
    QHash<uint32_t, MemRecord> memData;
    uint32_t div;
    QTimer timerUpdate1;
    QSettings sett;
};

#endif // MAINWINDOW_H
