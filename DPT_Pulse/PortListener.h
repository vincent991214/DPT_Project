/**
 * @file PortListener.h
 * @brief Port Listener.
 * @see PortListener
 */

#ifndef PORTLISTENER_H_
#define PORTLISTENER_H_

#include <QObject>
#include <QTimer>

class QextSerialPort;


/**
 * Port Listener.
 */
class PortListener : public QObject
{
    Q_OBJECT

        QextSerialPort * port;

    private:
        QTimer *timer_serial;
        void read_serial(void);

    public:
    /**
     * Constructor.
     *  @param parent parent object.
     */
    PortListener(QextSerialPort * port, QObject * parent = 0);
    ~PortListener();

    public slots:
        /**
         * Receive data from serial port.
         */
        void receive();
        void timer_serial_timeout(void);

    /**
     * Report written bytes.
     * 	@param bytes number of written bytes.
     */
    void reportWritten(qint64 bytes);

    /**
     * Report port closing.
     */
    void reportClose();

    /**
     * Report DSR line.
     * 	@param status line status.
     */
    void reportDsr(bool status);

};


#endif /*PORTLISTENER_H_*/
