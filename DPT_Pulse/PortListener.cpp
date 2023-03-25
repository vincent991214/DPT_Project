/**
 * @file PortListener.cpp
 * @brief PortListener Implementation.
 * @see PortListener.h
 */


#include <QTextStream>
#include "serial/qextserialport.h"
#include "PortListener.h"

//serial read buffer
#define TIMEOUT_FRAME   50  //50ms


PortListener::PortListener(QextSerialPort * port, QObject * parent):
    QObject(parent)
{
    this->port = port;
    timer_serial = new QTimer(this);
    connect(timer_serial,SIGNAL(timeout()),this,SLOT(timer_serial_timeout()));
    timer_serial->setInterval(TIMEOUT_FRAME);
    timer_serial->setSingleShot(true);
}

PortListener::~PortListener(void)
{
    delete timer_serial;
}

void PortListener::receive()
{
    timer_serial->start();
    read_serial();
}

void PortListener::timer_serial_timeout(void)
{ 
    //read_serial();
    //plc_frame_reset();
}

void PortListener::read_serial(void)
{
    char *data;
    short bytesCnt;

    if (port->isReadable())
    {
         bytesCnt = port->bytesAvailable();
         /*
         if(bytesCnt > MAX_READ_SZ)
                bytesCnt = MAX_READ_SZ;
         */
         data = (char *)malloc(bytesCnt);

         int bytes = port->read(data, bytesCnt);         
         if(bytes > 0)
         {
             for(int i = 0; i < bytes; i++)
             {
                 //plc_byterecv_cb((byte)data[i]);
             }
         }
         free(data);
    }
}

void PortListener::reportWritten(qint64 bytes)
{
    //QTextStream out(stdout);

    //out << bytes << " bytes written" << endl;
}

void PortListener::reportClose()
{
    QTextStream out(stdout);

    out << "closing port" << endl;	
}

void PortListener::reportDsr(bool status)
{
    QTextStream out(stdout);

    if (status)
        out << "device was turned on" << endl;
    else
        out << "device was turned off" << endl;
}


