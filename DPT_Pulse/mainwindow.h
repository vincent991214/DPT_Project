#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include "serial/qextserialport.h"
#include "serial/qextserialenumerator.h"
#include "DataFormat.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onPortAddedOrRemoved();
    void onPortReadyRead();

    void on_btnGo_clicked();

    void Timer_Counter_timeout(void);

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_btnCopy_clicked();

    void on_edtVbus_textChanged(const QString &arg1);

    void on_edtIndCur_textChanged(const QString &arg1);

    void on_edtLoadInd_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    QextSerialEnumerator *enumerator;
    QextSerialPort *port;
    QTimer *Timer_Counter;
    bool Connected;
    Buf_u wBuf;

    void init_Port(void);
};

#endif // MAINWINDOW_H
