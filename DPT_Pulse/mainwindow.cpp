#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serial/qextserialenumerator.h"
#include "PortListener.h"
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

#define REGKEY  "HKEY_CURRENT_USER\\Software\\OnSemi\\DPT Pluse"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //create port
    port = new QextSerialPort();

    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    ui->btnGo->setStyleSheet("background-color: green; color:red");

    Connected = false;

    //Timer
    Timer_Counter = new QTimer(this);
    connect(Timer_Counter,SIGNAL(timeout()),this,SLOT(Timer_Counter_timeout()));
    Timer_Counter->start(1000); //1 sec

    //tableWidget
    ui->tableWidget->horizontalHeader()->setFixedHeight(32);
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:blue; color:white}");
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    //LineEditor
    QDoubleValidator *dbValidator = new QDoubleValidator;
    dbValidator->setRange(1, 2000);
    ui->edtVbus->setValidator(dbValidator);

    dbValidator->setRange(0,100);
    ui->edtIndCur->setValidator(dbValidator);

    dbValidator->setRange(0,10000);
    ui->edtLoadInd->setValidator(dbValidator);

    //Register
    QSettings *reg = new QSettings(REGKEY,QSettings::NativeFormat);
    ui->edtVbus->setText(reg->value("VBus", "400").toString());
    ui->edtIndCur->setText(reg->value("Current", "160").toString());
    ui->edtLoadInd->setText(reg->value("Inductor", "100").toString());
    ui->tableWidget->item(0,2)->setText(reg->value("T1", "40").toString());
    ui->tableWidget->item(1,2)->setText(reg->value("T2", "10").toString());
    ui->tableWidget->item(2,2)->setText(reg->value("T3", "10").toString());
    ui->tableWidget->item(3,2)->setText(reg->value("TBreak", "1").toString());
    ui->chkRepeat->setChecked(reg->value("Repeat", "0").toBool());
    if(reg->value("Pulse","1").toChar() == 1) ui->rad1Pulse->setChecked(true);
    if(reg->value("Pulse","1").toChar() == 2) ui->rad2Pulse->setChecked(true);
    if(reg->value("Pulse","1").toChar() == 5) ui->rad5Pulse->setChecked(true);
    if(reg->value("Pulse","1").toChar() == 8) ui->rad8Pulse->setChecked(true);
    delete reg;

    init_Port();
    onPortAddedOrRemoved();
}

MainWindow::~MainWindow()
{
    port->close();
    QSettings *reg = new QSettings(REGKEY,QSettings::NativeFormat);
    reg->setValue("VBus", ui->edtVbus->text());
    reg->setValue("Current", ui->edtIndCur->text());
    reg->setValue("Inductor", ui->edtLoadInd->text());
    reg->setValue("T1", ui->tableWidget->item(0,2)->text());
    reg->setValue("T2", ui->tableWidget->item(1,2)->text());
    reg->setValue("T3", ui->tableWidget->item(2,2)->text());
    reg->setValue("TBreak", ui->tableWidget->item(3,2)->text());
    reg->setValue("Repeat", ui->chkRepeat->isChecked());
    if(ui->rad1Pulse->isChecked()) reg->setValue("Pulse", 1);
    if(ui->rad2Pulse->isChecked()) reg->setValue("Pulse", 2);
    if(ui->rad5Pulse->isChecked()) reg->setValue("Pulse", 5);
    if(ui->rad8Pulse->isChecked()) reg->setValue("Pulse", 8);

    delete port;
    delete reg;
    delete Timer_Counter;
    delete ui;
}

void MainWindow::init_Port(void)
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    qDebug("Port size:%d", ports.size());

    //Port setting
    port->setQueryMode(QextSerialPort::EventDriven);
    port->setBaudRate(BAUD9600);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(100);

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();
    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(port, SIGNAL(readyRead()), SLOT(onPortReadyRead()));
}

void MainWindow::onPortAddedOrRemoved()
{
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
    {
        if(info.friendName.contains("USB-SERIAL CH340"))
        {
            qDebug() << "Port add";
            port->setPortName(info.portName);
            if(!port->isOpen()) port->open(QIODevice::ReadWrite);;
            ui->lblConnect->setPixmap(QPixmap(":/Icons/Active.png"));
            ui->statusBar->showMessage("Arduino board connected.");
            return;
        }

    }
    qDebug() << "Port remove";
    ui->lblConnect->setPixmap(QPixmap(":/Icons/Inactive.png"));
    ui->statusBar->showMessage("Arduino board removed.");
    port->close();
}

void MainWindow::onPortReadyRead()
{
    if (port->bytesAvailable()) {
       qDebug(port->readAll().toHex());
    }
}

void MainWindow::on_btnGo_clicked()
{
    if(ui->btnGo->text() == "GO")
    {
        wBuf.buf.header = HD_START;
        wBuf.buf.param.T1 = ui->tableWidget->item(0,2)->text().toFloat() * 10;
        wBuf.buf.param.T2 = ui->tableWidget->item(1,2)->text().toFloat() * 10;
        wBuf.buf.param.T3 = ui->tableWidget->item(2,2)->text().toFloat() * 10;
        wBuf.buf.param.Tb = ui->tableWidget->item(3,2)->text().toFloat() * 10;
        if(ui->rad1Pulse->isChecked()) wBuf.buf.param.pulses = 1;
        else if (ui->rad2Pulse->isChecked()) wBuf.buf.param.pulses = 2;
        else if (ui->rad5Pulse->isChecked()) wBuf.buf.param.pulses = 5;
        else if (ui->rad8Pulse->isChecked()) wBuf.buf.param.pulses = 8;
        if(ui->chkRepeat->isChecked()) wBuf.buf.param.repeat = 1;
        else wBuf.buf.param.repeat = 0;

        qDebug() << "size:" << sizeof(wBuf.buf);
        QString str = "";
        for(int i=0; i<sizeof(wBuf.buf)-2; i++)
        {
            str.append(QString("").sprintf("%02X ",(unsigned char)wBuf.data[2+i]));
        }
        qDebug() << "Data:" << str;

        if(port->write(wBuf.data,sizeof(wBuf.buf)) == -1)
        {
            QMessageBox::critical(this, tr("Error"),
                                 tr("Send data to Arduino board failed!"),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
        }
        else
        {
            ui->btnGo->setText("Stop");
            ui->statusBar->showMessage("Testing is running...");
        }
    } else {
        wBuf.buf.header = HD_STOP;
        if(port->write(wBuf.data,sizeof(wBuf.buf.header)) == -1)
        {
            QMessageBox::critical(this, tr("Error"),
                                 tr("Send data to Arduino board failed!"),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
        }
        else
        {
            ui->btnGo->setText("GO");
            ui->statusBar->showMessage("Testing stop.",3000);
        }
    }
}

void MainWindow::Timer_Counter_timeout()
{

}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    //QRegExp regx("[1-9][0-9]*");
    QRegExp regx("^(([0-9]+\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\.[0-9]+)|([0-9]*[1-9][0-9]*))$");
    QString str=item->text();

    if (item->column()!= 2) return;
    if (regx.exactMatch(str))
    {
        switch(item->row())
        {
        case 0:
        case 1:
        case 2:
            if(item->text().toFloat() < 5.0F) item->setText("5.0");
            else if(item->text().toFloat() > 5000.0) item->setText("5000.0");
            break;
        case 3:
            if(item->text().toFloat() < 0.1F) item->setText("0.1");
            else if(item->text().toFloat() > 1000.0F) item->setText("1000.0");
            break;
        default:
            break;
        }
    } else{
        str = ui->tableWidget->item(item->row(),1)->text();
        str = str.left(str.length()-5);
        item->setText(str);
    }
}

void MainWindow::on_btnCopy_clicked()
{
    QString str;
    for(int i=0; i<4; i++)
    {
        str = ui->tableWidget->item(i,1)->text();
        str = str.left(str.length()-5);
        ui->tableWidget->item(i,2)->setText(str);
    }
}

void MainWindow::on_edtVbus_textChanged(const QString &arg1)
{
    if(arg1 !="" && ui->edtIndCur->text()!="" && ui->edtLoadInd->text()!="")
    {
        int T1 = ui->edtIndCur->text().toInt() * ui->edtLoadInd->text().toInt() / arg1.toInt();
        QString str = QString::number(T1);
        ui->tableWidget->item(0,1)->setText(str.append(" (us)"));
    }
}

void MainWindow::on_edtIndCur_textChanged(const QString &arg1)
{
    if(arg1 !="" && ui->edtVbus->text()!="" && ui->edtLoadInd->text()!="")
    {
        int T1 = arg1.toInt() * ui->edtLoadInd->text().toInt() / ui->edtVbus->text().toInt();
        QString str = QString::number(T1);
        ui->tableWidget->item(0,1)->setText(str.append(" (us)"));
    }
}

void MainWindow::on_edtLoadInd_textChanged(const QString &arg1)
{
    if(arg1 !="" && ui->edtVbus->text()!="" && ui->edtIndCur->text()!="")
    {
        int T1 = arg1.toInt() * ui->edtIndCur->text().toInt() / ui->edtVbus->text().toInt();
        QString str = QString::number(T1);
        ui->tableWidget->item(0,1)->setText(str.append(" (us)"));
    }
}
