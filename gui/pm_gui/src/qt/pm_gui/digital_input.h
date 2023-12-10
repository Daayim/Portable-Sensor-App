/***************************************************************************
digital_input.h: Include file for digital_input.cpp

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    June 2021
****************************************************************************/


#ifndef DIGITAL_INPUT_H
#define DIGITAL_INPUT_H


#include <QWidget>


class QLabel;


class DigitalInput : public QWidget
{
    Q_OBJECT

public:

    explicit DigitalInput(QString, QWidget *parent = nullptr);
    void setColor(bool);
    void setColor(QString);

signals:

private:

    QString level;

    QLabel *label;
    QLabel *statusLabel;

};


#endif // DIGITAL_INPUT_H


