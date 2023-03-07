/***************************************************************************
pm_gui.cpp:  Marine Mammal Detection Power Module GUI Widget class functions

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    November 2021

Note(s):
- The Marine Mammal Detection Power Module GUI Widget is the main widget for
the application.
****************************************************************************/


#include <QBoxLayout>
#include <QDebug>
#include <QFile>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include "digital_input.h"
#include "digital_output.h"
#include "pm_gui.h"
#include "serial.h"


// Local function(s)
QString getDegreeSymbol(void);


/***************************************************************************
Local function to return a QString containing the degree symbol
****************************************************************************/
QString getDegreeSymbol(void)
{
    QString string;

    QChar ch(0xB0);
    string.append(ch);

    return (string);

}   // End of getDegreeSymbol()


/***************************************************************************
Function to create the GUI controls required to control the power digital
outputs
****************************************************************************/
QGroupBox *Widget::createPowerGroupBox(void)
{
    qDebug() << "Widget::createPowerGroupBox";

    // Create the GUI controls
    QGroupBox *groupBox = new QGroupBox("Power");

    en_3v3va = new DigitalOutput("+3V3VA_EN", 0);
    batt_sel = new DigitalOutput("BATT_SEL", 0);
/*
    batt_ser_pwr_en = new DigitalOutput("BATT_SER_PWR_EN", 0);
    ctd_pwr_en = new DigitalOutput("CTD_PWR_EN", 0);
*/
    driver_en = new DigitalOutput("DRIVER_EN", 0);
    mmd_pwr_en = new DigitalOutput("MMD_PWR_EN", 0);
    vbs_pwr_en = new DigitalOutput("VBS_PWR_EN", 0);
    vbs_ser_pwr_en = new DigitalOutput("VBS_SER_PWR_EN", 0);
    wcm_diag_en = new DigitalOutput("WCM_DIAG_EN", 0);
    wcm_pwr_en = new DigitalOutput("WCM_PWR_EN", 0);
    wcm_rly = new DigitalOutput("WCM_EN", 0);

    // Create the layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(en_3v3va);
    layout->addWidget(batt_sel);
/*
    layout->addWidget(batt_ser_pwr_en);
    layout->addWidget(ctd_pwr_en);
*/
    layout->addWidget(driver_en);
    layout->addWidget(mmd_pwr_en);
    layout->addWidget(vbs_pwr_en);
    layout->addWidget(vbs_ser_pwr_en);
    layout->addWidget(wcm_diag_en);
    layout->addWidget(wcm_pwr_en);
    layout->addWidget(wcm_rly);
    groupBox->setLayout(layout);

    // Connect the signals and slots
    connect(en_3v3va, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(batt_sel, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
/*
    connect(batt_ser_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(ctd_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
*/
    connect(driver_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(mmd_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(vbs_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(vbs_ser_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(wcm_diag_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(wcm_pwr_en, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));
    connect(wcm_rly, SIGNAL(signalRadioButtonClicked(int)), this, SLOT(slotSetPower(int)));

    return (groupBox);

}   // End of Widget::createPowerGroupBox


/***************************************************************************
Function to create the GUI controls required to display sensor data
****************************************************************************/
QGroupBox *Widget::createSensorsGroupBox(void)
{
    int i;

    qDebug() << "Widget::createSensorsGroupBox";

    // Create the GUI controls
    QGroupBox *groupBox = new QGroupBox("Sensors");

    QLabel *leakLabel = new QLabel("Leak Detector");
    leakLabel->setAlignment(Qt::AlignHCenter);

    QLabel *leakVoltageLabel = new QLabel("Voltage (V)");
    leakVoltageLineEdit = new QLineEdit;

    QLabel *ltc2944Label = new QLabel("LTC2944 Battery Gas Gauge");
    ltc2944Label->setAlignment(Qt::AlignHCenter);

    QLabel *voltageLabel = new QLabel("Voltage (V)");
    voltageLineEdit = new QLineEdit;

    QLabel *currentLabel = new QLabel("Current (A)");
    currentLineEdit = new QLineEdit;

    QLabel *mc3416angleLabel = new QLabel("Angle (rad)");
    mc3416AngleLineEdit = new QLineEdit;

    QLabel *ltc2944TemperatureLabel = new QLabel(QString("Temperature (%1C)").arg(getDegreeSymbol()));
    ltc2944TemperatureLineEdit = new QLineEdit;

    QLabel *chargeLabel = new QLabel("Charge (mAh)");
    chargeLineEdit = new QLineEdit;

    QLabel *statusLabel = new QLabel("Status");
    statusLineEdit = new QLineEdit;

//    readLTC2944Button = new QPushButton("Read");

    QLabel *ms5637Label = new QLabel("MS5637 Pressure / Temperature Sensor");
    ms5637Label->setAlignment(Qt::AlignHCenter);

    for (i = 0; i < 7; i++)
    {
        cLineEditList.append(new QLineEdit);
    }

    QLabel *d1Label = new QLabel("D1");
    d1LineEdit = new QLineEdit;

    QLabel *d2Label = new QLabel("D2");
    d2LineEdit = new QLineEdit;

    QLabel *pressureLabel = new QLabel("Pressure (mbar)");
    ms5637pressureLineEdit = new QLineEdit;

    QLabel *ms5637TemperatureLabel = new QLabel(QString("Temperature (%1C)").arg(getDegreeSymbol()));
    ms5637TemperatureLineEdit = new QLineEdit;


    QLabel *mc3416Label = new QLabel("MC3416 Angle Sensor");
    mc3416Label->setAlignment(Qt::AlignHCenter);



//    readMS5637Button = new QPushButton("Read");


    reInitializeButton = new QPushButton("Reinitialize");

    PingButton = new QPushButton("Ping Module");

    CalibrateButton = new QPushButton("Calibrate");
    ZeroOffsetButton = new QPushButton("Zero");

    // Create the layout
    QHBoxLayout *leakLayout = new QHBoxLayout;

    leakLayout->addWidget(leakVoltageLabel);
    leakLayout->addWidget(leakVoltageLineEdit);

    QGridLayout *ltc2944gLayout = new QGridLayout;

    i = 0;
    ltc2944gLayout->addWidget(voltageLabel, i, 0);
    ltc2944gLayout->addWidget(voltageLineEdit, i, 1);

    i++;
    ltc2944gLayout->addWidget(currentLabel, i, 0);
    ltc2944gLayout->addWidget(currentLineEdit, i, 1);

    i++;
    ltc2944gLayout->addWidget(ltc2944TemperatureLabel, i, 0);
    ltc2944gLayout->addWidget(ltc2944TemperatureLineEdit, i, 1);

    i++;
    ltc2944gLayout->addWidget(chargeLabel, i, 0);
    ltc2944gLayout->addWidget(chargeLineEdit, i, 1);

    i++;
    ltc2944gLayout->addWidget(statusLabel, i, 0);
    ltc2944gLayout->addWidget(statusLineEdit, i, 1);

//    i++;
//    ltc2944gLayout->addWidget(readLTC2944Button, i, 0, 1, 2);

    QGridLayout *ms5637gLayout = new QGridLayout;
    for (i = 0; i < 7; i++)
    {
        if (i == 0)
            ms5637gLayout->addWidget(new QLabel("CRC"), i, 0);
        else
            ms5637gLayout->addWidget(new QLabel(QString("C%1").arg(i)), i, 0);

        ms5637gLayout->addWidget(cLineEditList.at(i), i, 1);
    }

    ms5637gLayout->addWidget(d1Label, i, 0);
    ms5637gLayout->addWidget(d1LineEdit, i, 1);

    i++;
    ms5637gLayout->addWidget(d2Label, i, 0);
    ms5637gLayout->addWidget(d2LineEdit, i, 1);

    i++;
    ms5637gLayout->addWidget(pressureLabel, i, 0);
    ms5637gLayout->addWidget(ms5637pressureLineEdit, i, 1);

    i++;
    ms5637gLayout->addWidget(ms5637TemperatureLabel, i, 0);
    ms5637gLayout->addWidget(ms5637TemperatureLineEdit, i, 1);


    QHBoxLayout *mc3416gLayout = new QHBoxLayout;

    mc3416gLayout->addWidget(mc3416angleLabel);
    mc3416gLayout->addWidget(mc3416AngleLineEdit);




//    i++;
//    ms5637gLayout->addWidget(readMS5637Button, i, 0, 1, 2);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(leakLabel);
    layout->addLayout(leakLayout);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(ltc2944Label);
    layout->addLayout(ltc2944gLayout);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(mc3416Label);
    layout->addLayout(mc3416gLayout);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(CalibrateButton);
    layout->addWidget(ZeroOffsetButton);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(ms5637Label);
    layout->addLayout(ms5637gLayout);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(reInitializeButton);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(PingButton);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    groupBox->setLayout(layout);

    // Initialize the controls
    leakVoltageLineEdit->setEnabled(false);
    voltageLineEdit->setEnabled(false);
    currentLineEdit->setEnabled(false);
    ltc2944TemperatureLineEdit->setEnabled(false);
    chargeLineEdit->setEnabled(false);
    statusLineEdit->setEnabled(false);
    foreach (QLineEdit *ell, cLineEditList)
        ell->setEnabled(false);
    d1LineEdit->setEnabled(false);
    d2LineEdit->setEnabled(false);
    ms5637pressureLineEdit->setEnabled(false);
    ms5637TemperatureLineEdit->setEnabled(false);

    mc3416AngleLineEdit->setEnabled(false);

    // Connect the signals and slots
//    connect(readLTC2944Button, SIGNAL(clicked()), this, SLOT(slotReadLTC2944()));
//    connect(readMS5637Button, SIGNAL(clicked()), this, SLOT(slotReadMS5637()));
    connect(reInitializeButton, SIGNAL(clicked()), this, SLOT(slotReinitialize()));

    connect(PingButton, SIGNAL(clicked()), this, SLOT(slotPingReceived()));
    connect(CalibrateButton, SIGNAL(clicked()), this, SLOT(slotCalibrateMC3416()));
    connect(ZeroOffsetButton, SIGNAL(clicked()), this, SLOT(slotZeroMC3416()));

    return (groupBox);

}   // End of Widget::createSensorsGroupBox


/***************************************************************************
Function to create the GUI controls required to monitor the status digital
inputs
****************************************************************************/
QGroupBox *Widget::createStatusGroupBox(void)
{
    qDebug() << "Widget::createStatusGroupBox";

    // Create the GUI controls
    QGroupBox *groupBox = new QGroupBox("Status");

    n_accel_int = new DigitalInput("Accelerometer /INT");
    ext_gpio1 = new DigitalInput("EXT_GPIO1");
    ext_gpio2 = new DigitalInput("EXT_GPIO2");
    lt8618_pg = new DigitalInput("LT8618 PG");
    n_ltc2944_alcc = new DigitalInput("LTC2944 /ALCC");
    n_wcm_fault = new DigitalInput("WCM /FAULT");

    // Create the layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(n_accel_int);
    layout->addWidget(ext_gpio1);
    layout->addWidget(ext_gpio2);
    layout->addWidget(lt8618_pg);
    layout->addWidget(n_ltc2944_alcc);
    layout->addWidget(n_wcm_fault);
    groupBox->setLayout(layout);

    return (groupBox);

}   // End of Widget::createStatusGroupBox


/***************************************************************************
Function to get the power module settings
****************************************************************************/
//void Widget::getPowerModuleSettings(void)
//{
//    qDebug() << "Widget::getPowerModuleSettings";

//    int ms = 0;
//    int dms = 100;

//    QTimer::singleShot(ms, this, SLOT(slotGetPowerBits()));
//    ms += dms;

//}   // End of Widget::getPowerModuleSettings


/***************************************************************************
Function to disable/enable the GUI controls when not connected
****************************************************************************/
void Widget::setConnectedState(connectedEnum state)
{
    qDebug() << "Widget::setConnectedState";

    bool bState;

    if (state == CONNECTED)
        bState = true;
    else
        bState = false;

    powerGroupBox->setEnabled(bState);
    sensorsGroupBox->setEnabled(bState);

    statusGroupBox->setEnabled(bState);
    ext_gpio1->setColor(bState);
    ext_gpio2->setColor(bState);
    lt8618_pg->setColor(bState);
    n_ltc2944_alcc->setColor(bState);
    n_accel_int->setColor(bState);
    n_wcm_fault->setColor(bState);

    readSettingsButton->setEnabled(bState);

}   // End of Widget::setConnectedState


/***************************************************************************
Function to set the power radio buttons according to the power bits
****************************************************************************/
void Widget::setPowerBits(int power_bits)
{
    qDebug() << "Widget::setPowerBits: power_bits = " << power_bits;

    // Make sure these offsets match the firmware
//    int en_5v_offset = 0;
//    int en_9v_offset = 1;
//    int dds_pwr_offset = 2;
//    int led1_offset = 3;
//    int led2_offset = 4;
//    int tclamp_offset = 5;
//    int gpio0_offset = 6;
//    int wakeup_en_offset = 7;

//    if (dio & 1 << en_5v_offset)
//        en5VRadioButtonOn->setChecked(true);
//    else
//        en5VRadioButtonOff->setChecked(true);

//    if (dio & 1 << en_9v_offset)
//        en9VRadioButtonOn->setChecked(true);
//    else
//        en9VRadioButtonOff->setChecked(true);

//    if (dio & 1 << dds_pwr_offset)
//        ddsPwrRadioButtonOn->setChecked(true);
//    else
//        ddsPwrRadioButtonOff->setChecked(true);

//    if (dio & 1 << led1_offset)
//        led1RadioButtonOn->setChecked(true);
//    else
//        led1RadioButtonOff->setChecked(true);

//    if (dio & 1 << led2_offset)
//        led2RadioButtonOn->setChecked(true);
//    else
//        led2RadioButtonOff->setChecked(true);

//    if (dio & 1 << tclamp_offset)
//        tclampRadioButtonOn->setChecked(true);
//    else
//        tclampRadioButtonOff->setChecked(true);

//    if (dio & 1 << gpio0_offset)
//        gpio0StatusLineEdit->setText("ON");
//    else
//        gpio0StatusLineEdit->setText("OFF");

//    if (dio & 1 << wakeup_en_offset)
//        wakeupEnStatusLineEdit->setText("ON");
//    else
//        wakeupEnStatusLineEdit->setText("OFF");

}   // End of Widget::setPowerBits


/***************************************************************************
Slot to display information about the software when the user clicks the
Help->About menu item
****************************************************************************/
void Widget::slotAbout(void)
{
    QString about("Marine Mammal Detection - Power Module");
    about.append(QString(" %1").arg(PM_VERSION));

    QFile file(":/sha1.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line = in.readLine();
        about.append(QString("\n\nRevision %1").arg(line));
    }

    QMessageBox messageBox;
    messageBox.setText(about);
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setWindowTitle("About Marine Mammal Detection - Power Module");
    messageBox.exec();

}   // End of Widget::slotAbout


/***************************************************************************
Slot to respond when a serial device is connected
****************************************************************************/
void Widget::slotConnected(void)
{
    qDebug() << "Widget::slotConnected";

    serialData.clear();
    setConnectedState(CONNECTED);

    slotReadSettings();

}   // End of Widget::slotConnected


/***************************************************************************
Slot to respond when data is read from a serial device
****************************************************************************/
void Widget::slotDataRead(QByteArray data)
{
    int bit_val;
    int index;
    QString bit_string;

    serialData.append(data);

//    qDebug() << "Widget::slotDataRead: serialData = " << serialData;

    while (1)
    {
        index = serialData.indexOf("\r\n");
        if (index == -1)
            break;
        else
        {
            // Extract the response
            QByteArray response = serialData.left(index);

            qDebug() << "Widget::slotDataRead: response = " << response;

            // Remove the response
            serialData.remove(0, index+2);

            QStringList list = QString(response).split(" ");
            if (list.length() > 1)
            {
                if (response.contains("+3V3VA_EN"))
                {
                    bit_val = list.at(1).toInt();
                    en_3v3va->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("BATT_SEL"))
                {
                    bit_val = list.at(1).toInt();
                    batt_sel->setChecked((bit_val == 1) ? true : false);
                }
/*
                else if (response.contains("BATT_SER_PWR_EN"))
                {
                    bit_val = list.at(1).toInt();
                    batt_ser_pwr_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("CTD_PWR_EN"))
                {
                    bit_val = list.at(1).toInt();
                    ctd_pwr_en->setChecked((bit_val == 1) ? true : false);
                }
*/
                else if (response.contains("DRIVER_EN"))
                {
                    bit_val = list.at(1).toInt();
                    driver_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("MMD_PWR_EN"))
                {
                    if (list.at(1) != "unchanged")
                    {
                        bit_val = list.at(1).toInt();
                        mmd_pwr_en->setChecked((bit_val == 1) ? true : false);
                    }
                }
                else if (response.contains("VBS_PWR_EN"))
                {
                    bit_val = list.at(1).toInt();
                    vbs_pwr_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("VBS_SER_PWR_EN"))
                {
                    bit_val = list.at(1).toInt();
                    vbs_ser_pwr_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("WCM_DIAG_EN"))
                {
                    bit_val = list.at(1).toInt();
                    wcm_diag_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("WCM_PWR_EN"))
                {
                    bit_val = list.at(1).toInt();
                    wcm_pwr_en->setChecked((bit_val == 1) ? true : false);
                }
                else if (response.contains("WCM_RLY"))
                {
                    if (list.at(1) != "unchanged")
                    {
                        bit_val = list.at(1).toInt();
                        wcm_rly->setChecked((bit_val == 1) ? true : false);
                    }
                }
                else if (response.contains("/ACCEL_INT"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    n_accel_int->setColor(bit_string);
                }
                else if (response.contains("EXT_GPIO1"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    ext_gpio1->setColor(bit_string);
                }
                else if (response.contains("EXT_GPIO2"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    ext_gpio2->setColor(bit_string);
                }
                else if (response.contains("LT8618_PG"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    lt8618_pg->setColor(bit_string);
                }
                else if (response.contains("/LTC2944_ALCC"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    n_ltc2944_alcc->setColor(bit_string);
                }
                else if (response.contains("/WCM_FAULT"))
                {
                    bit_val = list.at(1).toInt();
                    bit_string = (bit_val == 1) ? "True" : "False";
                    n_wcm_fault->setColor(bit_string);
                }
                else if (response.contains("LEAK"))
                {
                    leakVoltageLineEdit->setText(QString("%1").arg(list.at(1).toDouble(), 0, 'f', 3));
                }
                else if (response.contains("VOLTAGE"))
                {
                    voltageLineEdit->setText(QString("%1").arg(list.at(1).toDouble(), 0, 'f', 3));
                }
                else if (response.contains("CURRENT"))
                {
                    currentLineEdit->setText(QString("%1").arg(list.at(1).toDouble(), 0, 'f', 3));
                }
                else if (response.contains("LTC2944 TEMPERATURE"))
                {
                    ltc2944TemperatureLineEdit->setText(QString("%1").arg(list.at(2).toDouble(), 0, 'f', 2));
                }
                else if (response.contains("CHARGE"))
                {
                    chargeLineEdit->setText(QString("%1").arg(list.at(1).toDouble(), 0, 'f', 2));
                }
                else if (response.contains("STATUS"))
                {
                    statusLineEdit->setText(list.at(1));
                }


                else if (response.contains("ACCEL TILT ANGLE"))
                {
                    mc3416AngleLineEdit->setText(QString("%1").arg(list.at(3).toDouble(), 0, 'f', 2));
                }



                else if (response.contains("CRC"))
                {
                    cLineEditList.at(0)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C1"))
                {
                    cLineEditList.at(1)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C2 "))
                {
                    cLineEditList.at(2)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C3"))
                {
                    cLineEditList.at(3)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C4"))
                {
                    cLineEditList.at(4)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C5"))
                {
                    cLineEditList.at(5)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("C6"))
                {
                    cLineEditList.at(6)->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("D1"))
                {
                    d1LineEdit->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("D2"))
                {
                    d2LineEdit->setText(QString("%1").arg(list.at(1).toInt()));
                }
                else if (response.contains("PRESSURE"))
                {
                    ms5637pressureLineEdit->setText(QString("%1").arg(list.at(1).toDouble()));
                }
                else if (response.contains("TEMPERATURE"))
                {
                    ms5637TemperatureLineEdit->setText(QString("%1").arg(list.at(1).toDouble(), 0, 'f', 2));
                }
//                else
//                {
//                    QMessageBox::information(this, "MMD Power Module", QString("PM returned \"%1\"").arg(response.data()));
//                }
            }
        }
    }
}   // End of Widget::slotDataRead


/***************************************************************************
Slot to respond when a networked device is disconnected
****************************************************************************/
void Widget::slotDisconnected(void)
{
    qDebug() << "Widget::slotDisconnected";

    serialData.clear();
    setConnectedState(DISCONNECTED);

}   // End of Widget::slotDisconnected


/***************************************************************************
Slot to read the leak detector voltage
****************************************************************************/
void Widget::slotReadLeakDetector(void)
{
    qDebug() << "Widget::slotReadLeakDetector";

    serial->write("read_leak\r\n");

}   // End of Widget::slotReadLeakDetector


/***************************************************************************
Slot to read the LTC2944 battery gas gauge
****************************************************************************/
void Widget::slotReadLTC2944(void)
{
    qDebug() << "Widget::slotReadLTC2944";

    serial->write("read_ltc2944\r\n");

}   // End of Widget::slotReadLTC2944


/***************************************************************************
Slot to read the MC3416 axis
****************************************************************************/
void Widget::slotReadMC3416(void)
{
    qDebug() << "Widget::slotReadMC3416";

    serial->write("read_mc3416\r\n");

}   // End of Widget::slotReadLTC2944

/***************************************************************************
Slot to read the MS5637 pressure / temperature sensor
****************************************************************************/
void Widget::slotReadMS5637(void)
{
    qDebug() << "Widget::slotReadMS5637";

    serial->write("read_ms5637\r\n");

}   // End of Widget::slotReadMS5637


/***************************************************************************
Slot to read the power module power bits
****************************************************************************/
void Widget::slotReadPowerBits(void)
{
    qDebug() << "Widget::slotReadPowerBits";

    serial->write("read_power_bits\r\n");

}   // End of Widget::slotReadPowerBits


/***************************************************************************
Slot to read the power module settings
****************************************************************************/
void Widget::slotReadSettings(void)
{
    qDebug() << "Widget::slotReadSettings";

//    getPowerModuleSettings();

    int ms = 0;
    int dms = 500;

    QTimer::singleShot(ms, this, SLOT(slotReadLeakDetector()));
    ms += dms;

    QTimer::singleShot(ms, this, SLOT(slotReadLTC2944()));
    ms += dms;

    QTimer::singleShot(ms, this, SLOT(slotReadMS5637()));
    ms += dms;

    QTimer::singleShot(ms, this, SLOT(slotReadPowerBits()));
    ms += dms;

    QTimer::singleShot(ms, this, SLOT(slotReadStatusBits()));
    ms += dms;

    QTimer::singleShot(ms, this, SLOT(slotReadMC3416()));
    ms += dms;

}   // End of Widget::slotReadSettings


/***************************************************************************
Slot to read the power module status bits
****************************************************************************/
void Widget::slotReadStatusBits(void)
{
    qDebug() << "Widget::slotReadStatusBits";

    serial->write("read_status_bits\r\n");

}   // End of Widget::slotReadStatusBits

/***************************************************************************
Slot to ping the internal hardware
****************************************************************************/
void Widget::slotPingReceived(void)
{
    qDebug() << "Widget::slotPingReceived";

    serial->write("pm_ping\r\n");

}   // End of Widget::slotPingReceived
/***************************************************************************
Slot to reinitialize the internal hardware
****************************************************************************/
void Widget::slotReinitialize(void)
{
    qDebug() << "Widget::slotReinitialize";

    serial->write("reinitialize\r\n");

}   // End of Widget::slotReinitialize


/***************************************************************************
Slot to calibrate MC3416 readings
****************************************************************************/
void Widget::slotCalibrateMC3416(void)
{
    qDebug() << "Widget::slotCalibrateMC3416";

    serial->write("calibrate_mc3416\r\n");

}   // End of Widget::slotCalibrateMC3416

/***************************************************************************
Slot to zero MC3416 readings
****************************************************************************/
void Widget::slotZeroMC3416(void)
{
    qDebug() << "Widget::slotZeroMC3416";

    serial->write("zero_mc3416\r\n");

}   // End of Widget::slotZeroMC3416

/***************************************************************************
Slot to set a power bit
****************************************************************************/
void Widget::slotSetPower(int level)
{
    QString command;

    if (sender() == en_3v3va)
    {
        qDebug() << QString("Widget::slotSetPower: Setting +3V3VA_EN to %1").arg(level);

        command = QString("+3V3VA_EN %1\r\n").arg(level);
    }
    else if (sender() == batt_sel)
    {
        qDebug() << QString("Widget::slotSetPower: Setting BATT_SEL to %1").arg(level);

        command = QString("BATT_SEL %1\r\n").arg(level);
    }
/*
    else if (sender() == batt_ser_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting BATT_SER_PWR_EN to %1").arg(level);

        command = QString("BATT_SER_PWR_EN %1\r\n").arg(level);
    }
    else if (sender() == ctd_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting CTD_PWR_EN to %1").arg(level);

        command = QString("CTD_PWR_EN %1\r\n").arg(level);
    }
*/
    else if (sender() == driver_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting DRIVER_EN to %1").arg(level);

        command = QString("DRIVER_EN %1\r\n").arg(level);
    }
    else if (sender() == mmd_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting MMD_PWR_EN to %1").arg(level);

        command = QString("MMD_PWR_EN %1\r\n").arg(level);
    }
    else if (sender() == vbs_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting VBS_PWR_EN to %1").arg(level);

        command = QString("VBS_PWR_EN %1\r\n").arg(level);
    }
    else if (sender() == vbs_ser_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting VBS_SER_PWR_EN to %1").arg(level);

        command = QString("VBS_SER_PWR_EN %1\r\n").arg(level);
    }
    else if (sender() == wcm_diag_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting WCM_DIAG_EN to %1").arg(level);

        command = QString("WCM_DIAG_EN %1\r\n").arg(level);
    }
    else if (sender() == wcm_pwr_en)
    {
        qDebug() << QString("Widget::slotSetPower: Setting WCM_PWR_EN to %1").arg(level);

        command = QString("WCM_PWR_EN %1\r\n").arg(level);
    }
    else if (sender() == wcm_rly)
    {
        qDebug() << QString("Widget::slotSetPower: Setting WCM_RLY to %1").arg(level);

        command = QString("WCM_RLY %1\r\n").arg(level);
    }
    else
    {
        qDebug() << "Widget::slotSetPower: Unknown sender!";
        return;
    }

    serial->write(command);

}   // End of Widget::slotSetPower


/***************************************************************************
Widget constructor
****************************************************************************/
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "Widget::Widget";

    setWindowTitle("Marine Mammal Detection - Power Module");

    // Create the GUI controls
    QMenu *fileMenu = new QMenu("&File", this);
    QAction *exitAction = fileMenu->addAction("E&xit");

    QMenu *helpMenu = new QMenu("&Help", this);
    QAction *aboutAction = helpMenu->addAction("About &Marine Mammal Detection - Power Module");

    QMenuBar *menuBar = new QMenuBar;
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);

    serial = new Serial;
    powerGroupBox = createPowerGroupBox();
    sensorsGroupBox = createSensorsGroupBox();
    statusGroupBox = createStatusGroupBox();
    readSettingsButton = new QPushButton("Read Settings");

    // Create the layout
    QGridLayout *layout = new QGridLayout(this);

    layout->setMenuBar(menuBar);
    layout->addWidget(serial, 0, 0);
    layout->addWidget(powerGroupBox, 1, 0);
    layout->addWidget(sensorsGroupBox, 0, 1, 3, 1);
    layout->addWidget(statusGroupBox, 2, 0);
    layout->addWidget(readSettingsButton, 3, 0, 1, 2);

    // Initialize the controls
    setConnectedState(DISCONNECTED);

    // Connect the signals and slots
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(slotAbout()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(readSettingsButton, SIGNAL(clicked()), this, SLOT(slotReadSettings()));
    connect(serial, SIGNAL(signalConnected()), this, SLOT(slotConnected()));
    connect(serial, SIGNAL(signalDataRead(QByteArray)), this, SLOT(slotDataRead(QByteArray)));
    connect(serial, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()));

}   // End of Widget::Widget


/***************************************************************************
Widget destructor
****************************************************************************/
Widget::~Widget()
{
    qDebug() << "Widget::~Widget";

}   // End of Widget::~Widget


