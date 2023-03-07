/***************************************************************************
pm_gui.h:  Include file for pm_gui.cpp

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    November 2021
****************************************************************************/


#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>


class DigitalInput;
class DigitalOutput;
class QGroupBox;
class QLineEdit;
class QPushButton;
class Serial;


class Widget : public QWidget
{
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void slotAbout(void);
    void slotConnected(void);
    void slotDataRead(QByteArray);
    void slotDisconnected(void);
    void slotReadLeakDetector(void);
    void slotReadLTC2944(void);
    void slotReadMS5637(void);
    void slotReadMC3416(void);
    void slotReadPowerBits(void);
    void slotReadSettings(void);
    void slotReadStatusBits(void);
    void slotReinitialize(void);

    void slotCalibrateMC3416(void);
    void slotZeroMC3416(void);
    void slotPingReceived(void);

    void slotSetPower(int);

private:

    enum connectedEnum {DISCONNECTED, CONNECTED};

    QGroupBox *createPowerGroupBox(void);
    QGroupBox *createSensorsGroupBox(void);
    QGroupBox *createStatusGroupBox(void);
//    void getPowerModuleSettings(void);
    void setConnectedState(connectedEnum state);
    void setPowerBits(int);

    DigitalInput *n_accel_int;
    DigitalInput *n_ltc2944_alcc;
    DigitalInput *ext_gpio1;
    DigitalInput *ext_gpio2;
    DigitalInput *lt8618_pg;
    DigitalInput *n_wcm_fault;

    DigitalOutput *en_3v3va;
    DigitalOutput *batt_sel;
/*
    DigitalOutput *batt_ser_pwr_en;
    DigitalOutput *ctd_pwr_en;
*/
    DigitalOutput *driver_en;
    DigitalOutput *mmd_pwr_en;
    DigitalOutput *vbs_pwr_en;
    DigitalOutput *vbs_ser_pwr_en;
    DigitalOutput *wcm_diag_en;
    DigitalOutput *wcm_pwr_en;
    DigitalOutput *wcm_rly;

    QByteArray serialData;
    QGroupBox *powerGroupBox;
    QGroupBox *sensorsGroupBox;
    QGroupBox *statusGroupBox;
    QLineEdit *chargeLineEdit;
    QLineEdit *currentLineEdit;
    QLineEdit *d1LineEdit;
    QLineEdit *d2LineEdit;
    QLineEdit *leakVoltageLineEdit;
    QLineEdit *ltc2944TemperatureLineEdit;
    QLineEdit *ms5637pressureLineEdit;
    QLineEdit *ms5637TemperatureLineEdit;
    QLineEdit *mc3416AngleLineEdit;
    QLineEdit *statusLineEdit;
    QLineEdit *voltageLineEdit;
    QList<QLineEdit *> cLineEditList;
//    QPushButton *readLTC2944Button;
//    QPushButton *readMS5637Button;
//    QPushButton *readMC3416Button;
    QPushButton *readSettingsButton;
    QPushButton *reInitializeButton;
    QPushButton *CalibrateButton;
    QPushButton *ZeroOffsetButton;
    QPushButton *PingButton;

    Serial *serial;

};


#endif // WIDGET_H


