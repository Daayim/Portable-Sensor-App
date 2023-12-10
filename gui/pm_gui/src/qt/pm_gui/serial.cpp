/***************************************************************************
serial.cpp: Serial class functions

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    November 2021
****************************************************************************/


#include <QBoxLayout>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QTimer>
#include <windows.h>
#include <initguid.h>
#include <dbt.h>
#include "serial.h"


EXTERN_C const GUID GUID_DEVINTERFACE_USB_DEVICE;
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);


// Local variable(s)
static HDEVNOTIFY hDevNotify;


// Local function(s)
static bool sortPorts(QSerialPortInfo const &, QSerialPortInfo const &);


/***************************************************************************
Function to add the available serial ports to the corresponding GUI control
****************************************************************************/
void Serial::addSerialPorts(void)
{
    qDebug() << "Serial::addSerialPorts";

    // Add the available serial ports
    serialPortComboBox->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    std::sort(ports.begin(), ports.end(), sortPorts);
    foreach (const QSerialPortInfo &info, ports)
    {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        serialPortComboBox->addItem(list.first(), list);
    }

    if (bConnected)
    {
        int i = serialPortComboBox->findText(portName);
        if (i != -1)
        {
            serialPortComboBox->setCurrentIndex(i);
        }
    }
}   // End of Serial::addSerialPorts()


/***************************************************************************
Function to add the serial port setup to the corresponding GUI controls
****************************************************************************/
void Serial::addSerialPortSetup(void)
{
    // Add the baud rates
    baudRateComboBox->clear();
    baudRateComboBox->addItem(QString("4800"), QSerialPort::Baud4800);
    baudRateComboBox->addItem(QString("9600"), QSerialPort::Baud9600);
    baudRateComboBox->addItem(QString("19200"), QSerialPort::Baud19200);
    baudRateComboBox->addItem(QString("38400"), QSerialPort::Baud38400);
    baudRateComboBox->addItem(QString("115200"), QSerialPort::Baud115200);
    baudRateComboBox->setCurrentIndex(3);

    // Add the data bits
    dataBitsComboBox->addItem(QString("5"), QSerialPort::Data5);
    dataBitsComboBox->addItem(QString("6"), QSerialPort::Data6);
    dataBitsComboBox->addItem(QString("7"), QSerialPort::Data7);
    dataBitsComboBox->addItem(QString("8"), QSerialPort::Data8);
    dataBitsComboBox->setCurrentIndex(3);

    // Add the parity
    parityComboBox->addItem(QString(tr("None")), QSerialPort::NoParity);
    parityComboBox->addItem(QString(tr("Even")), QSerialPort::EvenParity);
    parityComboBox->addItem(QString(tr("Odd")), QSerialPort::OddParity);
    parityComboBox->addItem(QString(tr("Mark")), QSerialPort::MarkParity);
    parityComboBox->addItem(QString(tr("Space")), QSerialPort::SpaceParity);

    // Add the stop bits
    stopBitsComboBox->addItem(QString("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    stopBitsComboBox->addItem(QString("1.5"), QSerialPort::OneAndHalfStop);
#endif
    stopBitsComboBox->addItem(QString("2"), QSerialPort::TwoStop);

    // Add the flow control
    flowControlComboBox->addItem(QString(tr("None")), QSerialPort::NoFlowControl);
    flowControlComboBox->addItem(QString(tr("RTS/CTS")), QSerialPort::HardwareControl);
    flowControlComboBox->addItem(QString(tr("XON/XOFF")), QSerialPort::SoftwareControl);

}   // End of Serial::addSerialPortSetup()


/***************************************************************************
Function to create the GUI controls required to connect to a serial device
****************************************************************************/
QGroupBox *Serial::createSerialPortGroupBox(void)
{
    // Create the GUI controls
    QGroupBox *serialPortGroupBox = new QGroupBox("Serial Port");

    QLabel *serialPortLabel = new QLabel("Serial Port");
    serialPortComboBox = new QComboBox();

    refreshButton = new QPushButton("Refresh");

    QLabel *serialPortStatusLabel = new QLabel("Serial Port Status");
    serialPortStatusLineEdit = new QLineEdit();

    QLabel *baudRateLabel = new QLabel("Baud Rate");
    baudRateComboBox = new QComboBox();

    QLabel *dataBitsLabel = new QLabel("Data Bits");
    dataBitsComboBox = new QComboBox();

    QLabel *parityLabel = new QLabel("Parity");
    parityComboBox = new QComboBox();

    QLabel *stopBitsLabel = new QLabel("Stop Bits");
    stopBitsComboBox = new QComboBox();

    QLabel *flowControlLabel = new QLabel("Flow Control");
    flowControlComboBox = new QComboBox();

    serialPortDataPlainTextEdit = new QPlainTextEdit();

    connectButton = new QPushButton("Connect");
    disconnectButton = new QPushButton("Disconnect");
    clearButton = new QPushButton("Clear Messages");

    // Create the layout
    QHBoxLayout *portLayout = new QHBoxLayout;
    portLayout->addWidget(serialPortLabel);
    portLayout->addWidget(serialPortComboBox);
    portLayout->addWidget(refreshButton);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(serialPortStatusLabel);
    statusLayout->addWidget(serialPortStatusLineEdit);

    QGridLayout *gLayout = new QGridLayout;

    int row = 0;
    gLayout->addLayout(portLayout, row, 0);
    gLayout->addWidget(baudRateLabel, row, 1);
    gLayout->addWidget(baudRateComboBox, row, 2);

    row++;
    gLayout->addLayout(statusLayout, row, 0);
    gLayout->addWidget(dataBitsLabel, row, 1);
    gLayout->addWidget(dataBitsComboBox, row, 2);

    row++;
    gLayout->addWidget(serialPortDataPlainTextEdit, row, 0, 6, 1);
    gLayout->addWidget(parityLabel, row, 1);
    gLayout->addWidget(parityComboBox, row, 2);

    row++;
    gLayout->addWidget(stopBitsLabel, row, 1);
    gLayout->addWidget(stopBitsComboBox, row, 2);

    row++;
    gLayout->addWidget(flowControlLabel, row, 1);
    gLayout->addWidget(flowControlComboBox, row, 2);

    row++;
    gLayout->addWidget(connectButton, row, 1, 1, 2);

    row++;
    gLayout->addWidget(disconnectButton, row, 1, 1, 2);

    row++;
    gLayout->addWidget(clearButton, row, 1, 1, 2);

    serialPortGroupBox->setLayout(gLayout);

    // Initialize the controls
    serialPortDataPlainTextEdit->setReadOnly(true);
    serialPortDataPlainTextEdit->setMaximumBlockCount(100);

    // Add the ports
    addSerialPorts();

    // Add the port setup information
    addSerialPortSetup();

    // Initialize the controls
    serialPortStatusLineEdit->setEnabled(false);
    serialPortStatusLineEdit->setText("Disconnected");

    // Connect the signals and slots
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearMessages()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(slotConnectSerialPort()));
    connect(disconnectButton, SIGNAL(clicked()), this, SLOT(slotDisconnectSerialPort()));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(slotRefreshSerialPorts()));

    return(serialPortGroupBox);

}   // End of Serial::createSerialPortGroupBox


/***************************************************************************
Function to override the nativeEvent virtual function in order to intercept
the device change event and thereby handle cable insertion and removal

https://forum.qt.io/topic/19327/detect-usb-drive-and-its-related-information
https://stackoverflow.com/questions/38528684/detected-new-usb-device-connected-disconnected-on-qt
****************************************************************************/
bool Serial::nativeEvent(const QByteArray &, void *messageIn, long *)
{
    MSG *message = static_cast<MSG *>(messageIn);
    bool bCheck = false;
    bool bArrival = false;

    if (message->message == WM_DEVICECHANGE)
    {
        switch(message->wParam)
        {
        case DBT_DEVICEARRIVAL:
            bCheck = true;
            bArrival = true;
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            bCheck = true;
            break;
        }
    }

    if (bCheck)
    {
        DEV_BROADCAST_HDR* pMsgPtr = reinterpret_cast<DEV_BROADCAST_HDR *>(message->lParam);
        if (pMsgPtr)
        {
            switch (pMsgPtr->dbch_devicetype)
            {
            case DBT_DEVTYP_DEVICEINTERFACE:
                qDebug() << "Serial::nativeEvent: DBT_DEVTYP_DEVICEINTERFACE";
                if (bArrival)
                {
                    qDebug() << "Serial::nativeEvent: DBT_DEVICEARRIVAL";
                    emit signalDeviceArrival();
                }
                else
                {
                    qDebug() << "Serial::nativeEvent: DBT_DEVICEREMOVECOMPLETE";
                    emit signalDeviceRemoveComplete();
                }
                break;
            }
        }
    }

    return (false);

}   // End of Serial::nativeEvent


/***************************************************************************
Serial constructor
****************************************************************************/
Serial::Serial(QWidget *parent) : QWidget(parent)
{
    qDebug() << "Serial::Serial";

    bConnected = false;

    serialPort = new QSerialPort(this);

    setUpDeviceNotifications();

    // Create the GUI controls
    QGroupBox *serialPortGroupBox = createSerialPortGroupBox();

    // Create the layout
    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(serialPortGroupBox);

    // Initialize the controls
    setConnectedState(DISCONNECTED);

    // Connect the signals and slots
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(slotHandleSerialPortError(QSerialPort::SerialPortError)));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(this, SIGNAL(signalDeviceArrival()), this, SLOT(slotDeviceArrival()));
    connect(this, SIGNAL(signalDeviceRemoveComplete()), this, SLOT(slotDeviceRemoveComplete()));

}   // End of Serial::Serial


/***************************************************************************
Function to disable/enable the GUI controls when not connected
****************************************************************************/
void Serial::setConnectedState(connectedEnum state)
{
    qDebug() << "Serial::setConnectedState";

    bool bState;

    if (state == CONNECTED)
        bState = true;
    else
        bState = false;

    clearButton->setEnabled(bState);
    connectButton->setEnabled(!bState);
    disconnectButton->setEnabled(bState);
    refreshButton->setEnabled(!bState);

    baudRateComboBox->setEnabled(!bState);
    dataBitsComboBox->setEnabled(!bState);
    flowControlComboBox->setEnabled(!bState);
    parityComboBox->setEnabled(!bState);
    serialPortComboBox->setEnabled(!bState);
    stopBitsComboBox->setEnabled(!bState);

}   // End of Serial::setConnectedState


/***************************************************************************
Function to set up for device notifications
****************************************************************************/
void Serial::setUpDeviceNotifications(void)
{
    // Handle device arrival and removal
    DEV_BROADCAST_DEVICEINTERFACE deviceInterface;
    ZeroMemory(&deviceInterface, sizeof(deviceInterface));
    deviceInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    deviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    deviceInterface.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    hDevNotify = RegisterDeviceNotification(reinterpret_cast<HANDLE>(winId()), &deviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr)
    {
        qDebug() << "Serial::setUpDeviceNotifications: RegisterDeviceNotification failed!";
    }
}   // End of Serial::setUpDeviceNotifications


/***************************************************************************
Slot to clear the messages from the serial port QPlainTextEdit display
****************************************************************************/
void Serial::slotClearMessages(void)
{
    qDebug() << "Serial::slotClearMessages";

    serialPortDataPlainTextEdit->clear();

}   // End of slotClearMessages


/***************************************************************************
Slot to connect to a serial port
****************************************************************************/
void Serial::slotConnectSerialPort(void)
{
    qDebug() << "Serial::slotConnectSerialPort";

    // If there is no serial device connected, don't try to connect
    if (serialPortComboBox->currentText() == "")
    {
        QMessageBox::information(this, "Serial", tr("There is no serial device connected."));
        return;
    }

    qint32 baudRate = static_cast<QSerialPort::BaudRate>(baudRateComboBox->itemData(baudRateComboBox->currentIndex()).toInt());
    QSerialPort::DataBits dataBits = static_cast<QSerialPort::DataBits>(dataBitsComboBox->itemData(dataBitsComboBox->currentIndex()).toInt());
    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(parityComboBox->itemData(parityComboBox->currentIndex()).toInt());
    QSerialPort::StopBits stopBits = static_cast<QSerialPort::StopBits>(stopBitsComboBox->itemData(stopBitsComboBox->currentIndex()).toInt());
    QSerialPort::FlowControl flowControl = static_cast<QSerialPort::FlowControl>(flowControlComboBox->itemData(flowControlComboBox->currentIndex()).toInt());

    serialPort->setPortName(serialPortComboBox->currentText());
    if (!serialPort->setBaudRate(baudRate))
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
        return;
    }
    if (!serialPort->setDataBits(dataBits))
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
        return;
    }
    if (!serialPort->setParity(parity))
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
        return;
    }
    if (!serialPort->setStopBits(stopBits))
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
        return;
    }
    if (!serialPort->setFlowControl(flowControl))
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
        return;
    }

    qDebug() << QString("Serial::slotConnectSerialPort:  Connecting to serial device on %1:  %2 (%3), %4 (%5), %6 (%7), %8 (%9), %10 (%11) ...")
                .arg(serialPortComboBox->currentText())
                .arg(baudRateComboBox->currentText())
                .arg(baudRate)
                .arg(dataBitsComboBox->currentText())
                .arg(dataBits)
                .arg(parityComboBox->currentText())
                .arg(parity)
                .arg(stopBitsComboBox->currentText())
                .arg(stopBits)
                .arg(flowControlComboBox->currentText())
                .arg(flowControl);

    if (serialPort->open(QIODevice::ReadWrite))
    {
        // Atmel-42096-Microcontrollers-Embedded-Debugger_User-Guide.pdf
        // Note that the UART pins of the EDBG are tri-stated when no terminal program is connected to the Virtual
        // COM Port on the computer. This mechanism relies on the terminal program sending a DTR signal.
        serialPort->setDataTerminalReady(true);

        serialPort->clear();
        serialPortStatusLineEdit->setText("Connected");
        setConnectedState(CONNECTED);
        bConnected = true;
        portName = serialPortComboBox->currentText();
        emit signalConnected();
    }
    else
    {
        QMessageBox::warning(this, "Serial", serialPort->errorString());
    }
}   // End of Serial::slotConnectSerialPort


/***************************************************************************
Slot to refresh the list of serial ports when a device is connected
****************************************************************************/
void Serial::slotDeviceArrival(void)
{
    qDebug() << "Serial::slotDeviceArrival";

    QTimer::singleShot(1000, this, SLOT(slotRefreshSerialPorts()));

}   // End of Serial::slotDeviceArrival


/***************************************************************************
Slot to refresh the list of serial ports when a device is disconnected
****************************************************************************/
void Serial::slotDeviceRemoveComplete(void)
{
    qDebug() << "Serial::slotDeviceRemoveComplete";

//    slotSerialPortDisconnect();

    slotRefreshSerialPorts();

}   // End of Serial::slotDeviceRemoveComplete


/***************************************************************************
Slot to disconnect from a serial port
****************************************************************************/
void Serial::slotDisconnectSerialPort(void)
{
    qDebug() << "Serial::slotDisconnectSerialPort";

    if (serialPort->isOpen())
        serialPort->close();
    serialPortStatusLineEdit->setText("Disconnected");
    slotRefreshSerialPorts();
    setConnectedState(DISCONNECTED);
    bConnected = false;
    emit signalDisconnected();

}   // End of Serial::slotDisconnectSerialPort


/***************************************************************************
Slot to display an serial device connection error
****************************************************************************/
void Serial::slotHandleSerialPortError(QSerialPort::SerialPortError error)
{
    bool bError = true;

    switch (error)
    {
    case QSerialPort::NoError:
        bError = false;
        break;
    case QSerialPort::DeviceNotFoundError:
        QMessageBox::warning(this, "Serial", tr("DeviceNotFoundError"));
        break;
    case QSerialPort::PermissionError:
        QMessageBox::warning(this, "Serial", tr("PermissionError"));
        break;
    case QSerialPort::OpenError:
        QMessageBox::warning(this, "Serial", tr("OpenError"));
        break;
    case QSerialPort::ParityError:
        QMessageBox::warning(this, "Serial", tr("ParityError"));
        break;
    case QSerialPort::FramingError:
        break;
    case QSerialPort::BreakConditionError:
        QMessageBox::warning(this, "Serial", tr("BreakConditionError"));
        break;
    case QSerialPort::WriteError:
        QMessageBox::warning(this, "Serial", tr("WriteError"));
        break;
    case QSerialPort::ReadError:
        QMessageBox::warning(this, "Serial", tr("ReadError"));
        break;
    case QSerialPort::ResourceError:
        QMessageBox::warning(this, "Serial", tr("ResourceError"));
        break;
    case QSerialPort::UnsupportedOperationError:
        QMessageBox::warning(this, "Serial", tr("UnsupportedOperationError"));
        break;
    case QSerialPort::UnknownError:
        QMessageBox::warning(this, "Serial", tr("UnknownError"));
        break;
    case QSerialPort::TimeoutError:
        QMessageBox::warning(this, "Serial", tr("TimeoutError"));
        break;
    case QSerialPort::NotOpenError:
        QMessageBox::warning(this, "Serial", tr("NotOpenError"));
        break;
    default:
        QMessageBox::warning(this, "Serial", tr("Unknown QSerialPort::SerialPortError"));
        break;
    }

    if (bError)
        slotDisconnectSerialPort();

}   // End of Serial::slotHandleSerialPortError()


/***************************************************************************
Slot to read data from the connected device
****************************************************************************/
void Serial::slotReadyRead(void)
{
    QByteArray data = serialPort->readAll();

//    qDebug() << "Serial::slotReadyRead: data =" << data;

    QByteArray dataToDisplay = data;
    dataToDisplay.replace('\r', "");
//    serialPortDataPlainTextEdit->appendPlainText(data.data());
    serialPortDataPlainTextEdit->moveCursor(QTextCursor::End);
    serialPortDataPlainTextEdit->insertPlainText(dataToDisplay.data());

    emit signalDataRead(data);

}   // End of Serial::slotReadyRead


/***************************************************************************
Slot to refresh the available serial ports
****************************************************************************/
void Serial::slotRefreshSerialPorts(void)
{
    qDebug() << "Serial::slotRefreshSerialPorts";

    addSerialPorts();

}   // End of Serial::slotRefreshSerialPorts()


/***************************************************************************
Function to send a command via the serial port
****************************************************************************/
void Serial::write(QString command)
{
    serialPort->write(command.toLatin1());
    serialPort->flush();

}   // End of Serial::write


/***************************************************************************
Function required by std::sort to sort serial port names
****************************************************************************/
static bool sortPorts(QSerialPortInfo const &s1, QSerialPortInfo const &s2)
{
    return s1.portName() < s2.portName();

}   // End of sortPorts


