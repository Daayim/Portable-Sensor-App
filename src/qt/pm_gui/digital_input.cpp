/***************************************************************************
digital_input.cpp: DigitalInput class functions

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    June 2021
****************************************************************************/


#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include "digital_input.h"


/***************************************************************************
DigitalInput constructor
****************************************************************************/
DigitalInput::DigitalInput(QString labelIn, QWidget *parent) : QWidget(parent)
{
    qDebug() << "DigitalInput::DigitalInput";

    QChar bulletChar(0x2B24);

    // Create the GUI controls
    label = new QLabel(labelIn);
    statusLabel = new QLabel(bulletChar);

    // Create the layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(statusLabel);

    // Set spacing and margins
//    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

}   // End of DigitalInput::DigitalInput


/***************************************************************************
Function to set the label color while connecting / disconnecting
****************************************************************************/
void DigitalInput::setColor(bool bConnected)
{
    qDebug() << "DigitalInput::setColor bool";

    if (bConnected == false)
    {
        QPalette::ColorRole r = QPalette::WindowText;
        QPalette p = label->palette();

        QString ssString = QString("color: rgb(%1, %2, %3);")
                .arg(p.color(r).red()).arg(p.color(r).green()).arg(p.color(r).blue());

        statusLabel->setStyleSheet(ssString);
    }
    else
    {
        setColor(level);
    }
}   // End of DigitalInput::setColor


/***************************************************************************
Function to set the label color while connected
****************************************************************************/
void DigitalInput::setColor(QString value)
{
    qDebug() << "DigitalInput::setColor QString";

    level = value;

    if (level == "True")
        statusLabel->setStyleSheet("color: green");
    else
        statusLabel->setStyleSheet("color: red");

}   // End of DigitalInput::setColor


