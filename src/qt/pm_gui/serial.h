/***************************************************************************
serial.h: Include file for serial.cpp

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    November 2021
****************************************************************************/


#ifndef SERIAL_H
#define SERIAL_H


#include <QSerialPort>
#include <QWidget>


class QComboBox;
class QGroupBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;


class Serial : public QWidget
{
    Q_OBJECT

public:

    explicit Serial(QWidget *parent = nullptr);
    void write(QString);

signals:

    void signalConnected(void);
    void signalDisconnected(void);
    void signalDataRead(QByteArray);
    void signalDeviceArrival(void);
    void signalDeviceRemoveComplete(void);

protected:

    virtual bool nativeEvent(const QByteArray &, void *, long *);

private slots:

    void slotClearMessages(void);
    void slotConnectSerialPort(void);
    void slotDeviceArrival(void);
    void slotDeviceRemoveComplete(void);
    void slotDisconnectSerialPort(void);
    void slotHandleSerialPortError(QSerialPort::SerialPortError);
    void slotReadyRead(void);
    void slotRefreshSerialPorts(void);

private:

    enum connectedEnum {DISCONNECTED, CONNECTED};

    void addSerialPorts(void);
    void addSerialPortSetup(void);
    QGroupBox *createSerialPortGroupBox(void);
    void setConnectedState(connectedEnum state);
    void setUpDeviceNotifications(void);

    bool bConnected;
    QComboBox *baudRateComboBox;
    QComboBox *dataBitsComboBox;
    QComboBox *flowControlComboBox;
    QComboBox *parityComboBox;
    QComboBox *serialPortComboBox;
    QComboBox *stopBitsComboBox;
    QLineEdit *serialPortStatusLineEdit;
    QPlainTextEdit *serialPortDataPlainTextEdit;
    QPushButton *clearButton;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;
    QSerialPort *serialPort;
    QString portName;

};

#endif // SERIAL_H


