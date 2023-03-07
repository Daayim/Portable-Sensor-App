/***************************************************************************
digital_output.cpp: DigitalOutput class functions

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    June 2021
****************************************************************************/


#include <QButtonGroup>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include "digital_output.h"


/***************************************************************************
DigitalOutput constructor
****************************************************************************/
DigitalOutput::DigitalOutput(QString labelIn, int initialState, QWidget *parent) : QWidget(parent)
{
    qDebug() << "DigitalOutput::DigitalOutput";

    // Create the GUI controls
    QLabel *label = new QLabel(labelIn);

    lowRadioButton = new QRadioButton("Low");
    highRadioButton = new QRadioButton("High");

    QButtonGroup *digitalOutputButtonGroup = new QButtonGroup(this);
    digitalOutputButtonGroup->addButton(lowRadioButton);
    digitalOutputButtonGroup->addButton(highRadioButton);

    // Initialize the GUI controls
    if (initialState == 0)
        lowRadioButton->setChecked(true);
    else
        highRadioButton->setChecked(true);

    // Create the layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lowRadioButton);
    layout->addWidget(highRadioButton);

    // Set spacing and margins
//    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->setStretch(0, 2);
    layout->setStretch(1, 1);
    layout->setStretch(2, 1);

    // Connect the signals and slots
    connect(lowRadioButton, SIGNAL(clicked()), this, SLOT(slotRadioButtonClicked()));
    connect(highRadioButton, SIGNAL(clicked()), this, SLOT(slotRadioButtonClicked()));

}   // End of DigitalOutput::DigitalOutput


/***************************************************************************
Function to check the high or low radio button while connected
****************************************************************************/
void DigitalOutput::setChecked(bool b)
{
    qDebug() << "DigitalOutput::setChecked bool";

    if (b)
        highRadioButton->setChecked(true);
    else
        lowRadioButton->setChecked(true);

}   // End of DigitalOutput::setChecked


/***************************************************************************
Function to check the high or low radio button while connected
****************************************************************************/
void DigitalOutput::setChecked(QString value)
{
    qDebug() << "DigitalOutput::setChecked QString";

    if (value == "True")
        highRadioButton->setChecked(true);
    else
        lowRadioButton->setChecked(true);

}   // End of DigitalOutput::setChecked


/***************************************************************************
Slot to respond to a radio button click
****************************************************************************/
void DigitalOutput::slotRadioButtonClicked(void)
{
    qDebug() << "DigitalOutput::slotRadioButtonClicked";

    if (sender() == lowRadioButton)
        emit signalRadioButtonClicked(0);
    else if (sender() == highRadioButton)
        emit signalRadioButtonClicked(1);

}   // End of DigitalOutput::slotRadioButtonClicked


