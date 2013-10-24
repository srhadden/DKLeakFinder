#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    div = 0x20000000;
    ui->setupUi(this);
    memView = new DKMemmoryView(this);
    ui->scrollArea->setWidget(memView);
    memView->setMemSize(65536);
    memView->setRectSize(QSize(7, 7));
    port_enum = new QextSerialEnumerator(this);
    port_dev = new QextSerialPort(QextSerialPort::EventDriven, this);
    port_dev->setBaudRate(BAUD115200);
    port_dev->setDataBits(DATA_8);
    port_dev->setParity(PAR_NONE);
    port_dev->setStopBits(STOP_1);
    port_dev->setFlowControl(FLOW_OFF);
    port_dev->setTimeout(500);
    port_enum->setUpNotifications();
    connect(port_dev, SIGNAL(readyRead()), this, SLOT(processData()));
    timerUpdate1.setInterval(1000);
    timerUpdate1.setSingleShot(false);
    timerUpdate1.start();
    deviceListChanged();

    connect(memView, SIGNAL(selctionChanged(uint32_t)), this, SLOT(showMemInfo(uint32_t)));
    connect(&timerUpdate1, SIGNAL(timeout()), this, SLOT(showMemInfo()));
    connect(port_enum, SIGNAL(deviceListChanged()), this, SLOT(deviceListChanged()));

    this->setGeometry(sett.value("geometry").toRect());

    ui->statusBar->showMessage(tr("Made by Damian Kmiecik <d0zoenator@gmail.com>"));
}

MainWindow::~MainWindow()
{
    sett.setValue("geometry", this->geometry());

    delete ui;
    delete port_enum;
    delete port_dev;
    delete memView;
}

void MainWindow::processData()
{
    QByteArray data(port_dev->readAll());
    //
    static QString out;
    out += data;
    if (!out.endsWith("\r\n"))
        return;
    int m = 0;
    while ((m = out.indexOf("\r\n")) != -1)
    {
        QString line = out.left(m);
        out.remove(0, m+2);
        QRegExp exp("^M (\\d+) (\\d+) (\\w*)$");
        if (!exp.isValid())
            continue;
        if ((m = exp.indexIn(line)) != -1)
        {
            qDebug() << "MALLOC" << (exp.cap(2).toULong()-div) << exp.cap(1).toULong() << exp.cap(3);
            memView->DoMalloc(exp.cap(2).toULong()-div, exp.cap(1).toULong());
            MemRecord record;
            record.ptr = exp.cap(2).toULong();
            record.size = exp.cap(1).toULong();
            record.task = exp.cap(3);
            record.creationTime = QDateTime::currentDateTime();
            memData[record.ptr] = record;
        }
        QRegExp exp2("^F (\\d+) (\\w*)$");
        if (!exp2.isValid())
            continue;
        if ((m = exp2.indexIn(line)) != -1)
        {
            qDebug() << "FREE" << (exp2.cap(2).toULong()-div) << exp2.cap(1).toULong() << exp2.cap(3);
            memView->DoFree(exp2.cap(1).toULong()-div);
            if (memData.contains(exp2.cap(1).toULong()))
                memData.remove(exp2.cap(1).toULong());
        }
        QRegExp exp3("^S (\\d+) (\\d+)$");
        if (!exp3.isValid())
            continue;
        if ((m = exp3.indexIn(line)) != -1)
        {
            qDebug() << "SIZE" << exp3.cap(1).toULong() << exp3.cap(2).toULong();
            memView->setMemSize(exp3.cap(2).toULong());
            div = exp3.cap(1).toULong();
        }
    }
    out = "";
}

void MainWindow::showMemInfo(uint32_t addr)
{
    static uint32_t addrTmp;
    if (addr == 0xFFFFFFFF)
        addr = addrTmp;
    else
        addr += div;
    if (memData.contains(addr))
    {
        ui->labelMemSize->setText(QString("%1B").arg(QString::number(memData[addr].size)));
        ui->labelMemAddr->setText(QString("0x%1").arg(QString::number(memData[addr].ptr, 16)));
        ui->labelMemTime->setText(memData[addr].creationTime.toString("hh:mm:ss"));
        ui->labelMemLife->setText(QString("%1s").arg(QDateTime::currentDateTime().toTime_t() - memData[addr].creationTime.toTime_t()));
        ui->labelMemThread->setText(memData[addr].task);
        addrTmp = addr;
    } else {
        ui->labelMemTime->setText("0:00:00");
        ui->labelMemLife->setText("0s");
        ui->labelMemThread->setText("unknown");
        ui->labelMemSize->setText("0B");
        ui->labelMemAddr->setText("0x00000000");
    }
}

void MainWindow::deviceListChanged()
{
    ui->comboBoxPort->clear();
    QList<QextPortInfo> ports = port_enum->getPorts();
    for (int i = 0; i < ports.size(); ++i)
    {
        const QextPortInfo *port = &ports.at(i);
        ui->comboBoxPort->addItem(port->portName);
    }
}

void MainWindow::on_pushButtonConn_clicked()
{
    if (!port_dev->isOpen())
    {
        port_dev->setPortName(ui->comboBoxPort->currentText());
        port_dev->setBaudRate((BaudRateType)ui->comboBoxSpeed->currentText().toULongLong());
        if (port_dev->open(QextSerialPort::ReadWrite))
        {
            ui->pushButtonConn->setText(tr("Disconnect"));
        }
    } else {
        port_dev->close();
        ui->pushButtonConn->setText(tr("Connect"));
    }
}
