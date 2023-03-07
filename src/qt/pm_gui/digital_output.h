/***************************************************************************
digital_output.h: Include file for digital_output.cpp

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    June 2021
****************************************************************************/


#ifndef DIGITAL_OUTPUT_H
#define DIGITAL_OUTPUT_H


#include <QWidget>


class QRadioButton;


class DigitalOutput : public QWidget
{
    Q_OBJECT

public:

    explicit DigitalOutput(QString, int, QWidget *parent = nullptr);
    void setChecked(bool);
    void setChecked(QString);

signals:

    void signalRadioButtonClicked(int);

private slots:

    void slotRadioButtonClicked(void);

private:

    QRadioButton *lowRadioButton;
    QRadioButton *highRadioButton;

};


#endif // DIGITAL_OUTPUT_H


