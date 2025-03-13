/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>

QT_USE_NAMESPACE

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);
/*
    connect(ui->applyButton, &QPushButton::clicked,
            this, &SettingsDialog::apply);
            */
    connect(ui->serialPortInfoListBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    connect(ui->baudRateBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy);
    connect(ui->serialPortInfoListBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomDevicePathPolicy);
    connect(ui->ButtonBox, SIGNAL(accepted()), this, SLOT(OkButton()));

    //connect(ui->ButtonBox, SIGNAL(rejected()), this, SLOT(close()));

    //fillPortsInfo();
    //fillPortsParameters();
    ////readPortSettings();
    //updateSettings();
    fillPortsInfo();
    fillPortsParameters();
    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
void SettingsDialog::OkButton()
{
    apply();
    writePortSettings();
}

void SettingsDialog::closeEvent ( QCloseEvent * ev) {
    apply();
    //writePortSettings();
    ev->accept();
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::writePortSettings(void)
{
config_file.fileName = &file_setup;
config_file.file.setFileName(*config_file.fileName);
config_file.directionName.setCurrent(file_direction);
if(config_file.file.open(QIODevice::ReadWrite | QIODevice::Text))
{
    QTextStream config_out(&config_file.file);
    config_file.file.resize(0);


    qDebug() << "Zapis:";
    if (ui->serialPortInfoListBox->currentIndex() == -1)
        return;

    config_out << ui->serialPortInfoListBox->currentIndex() << "\n";
    qDebug() << ui->serialPortInfoListBox->currentIndex()<< ui->serialPortInfoListBox->currentText();
    config_out << ui->baudRateBox->currentIndex() << "\n";
    qDebug() << ui->baudRateBox->currentIndex()<< ui->baudRateBox->currentText();
    config_out << ui->dataBitsBox->currentIndex() << "\n";
    qDebug() << ui->dataBitsBox->currentIndex()<< ui->dataBitsBox->currentText();
    config_out << ui->parityBox->currentIndex() << "\n";
    qDebug() << ui->parityBox->currentIndex()<< ui->parityBox->currentText();
    config_out << ui->stopBitsBox->currentIndex() << "\n";
    qDebug() << ui->stopBitsBox->currentIndex()<< ui->stopBitsBox->currentText();
    config_out << ui->flowControlBox->currentIndex() << "\n";
    qDebug() << ui->flowControlBox->currentIndex()<< ui->flowControlBox->currentText();
    config_out << currentSettings.stringDelay << "\n";
    qDebug() << currentSettings.stringDelay;
    config_out << ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList().at(1);
    qDebug() << ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList().at(1);
    config_file.file.close();
}
else
{
    //QMessageBox::about(this, tr("Błąd"), tr("błąd otwarcia pliku"));
}
}

void SettingsDialog::readPortSettings(bool text_bool)
{
    config_file.fileName = &file_setup;
    config_file.file.setFileName(*config_file.fileName);
    config_file.directionName.setCurrent(file_direction);

    if(config_file.file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream config_in(&config_file.file);
        if(!config_in.atEnd())
        {
            int i = 0;// config_buf;
            qint8 index = 0;
            QString config_line[config_buf+1];
            QString text;

            qDebug() << "Odczyt:";
            do{
                config_line[i] = config_in.readLine();
                //qDebug() << config_line[i].toInt();
                i++;
            }while(i < (config_buf+1));
            //currentSettings.name = config_line[0];
            ui->serialPortInfoListBox->setCurrentIndex(config_line[0].toInt());
            qDebug() << ui->serialPortInfoListBox->currentIndex()<< ui->serialPortInfoListBox->currentText();
            //currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(config_line[1].toInt());
            ui->baudRateBox->setCurrentIndex(config_line[1].toInt());
            qDebug() << ui->baudRateBox->currentIndex()<< ui->baudRateBox->currentText();
            //currentSettings.dataBits = static_cast<QSerialPort::DataBits>(config_line[2].toInt());
            ui->dataBitsBox->setCurrentIndex(config_line[2].toInt());
            qDebug() << ui->dataBitsBox->currentIndex()<< ui->dataBitsBox->currentText();
            //currentSettings.parity = static_cast<QSerialPort::Parity>(config_line[3].toInt());
            ui->parityBox->setCurrentIndex(config_line[3].toInt());
            qDebug() << ui->parityBox->currentIndex()<< ui->parityBox->currentText();
            //currentSettings.stopBits = static_cast<QSerialPort::StopBits>(config_line[4].toInt());
            ui->stopBitsBox->setCurrentIndex(config_line[4].toInt());
            qDebug() << ui->stopBitsBox->currentIndex()<< ui->stopBitsBox->currentText();
            //currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(config_line[5].toInt());
            ui->flowControlBox->setCurrentIndex(config_line[5].toInt());
            qDebug() << ui->flowControlBox->currentIndex()<< ui->flowControlBox->currentText();
            text = config_line[6];
            text.remove("ms");
            ui->comDelay->setValue(text.toDouble());
            qDebug() << ui->comDelay->text();
            qDebug() << config_line[7];
            //wyszukiwanie portu
            if(ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList().at(1) != config_line[7])
            {
                qDebug() << "inny port";
                index = ui->serialPortInfoListBox->count();
                qDebug() << index;
                QStringList list = ui->serialPortInfoListBox->itemData(index).toStringList();
                text = list.count() > 1 ? list.at(1) : tr(blankString);
                qDebug() << text;
                while( text != config_line[7] && index > -1)
                {
                    index --;
                    qDebug() << index;
                }

                if(index < 0)
                {
                    ui->serialPortInfoListBox->setCurrentIndex(0);
                    qDebug() << "nie znaleziono portu";
                    if(text_bool)emit sendText("Nie znaleziono przejściówki.", er_color);
                }
            }
            else
            {
                qDebug() << "port ok";
                if(text_bool)emit sendText("Połączono z przejściówką.", ok_color);
            }
         }
         config_file.file.close();
     }
     else
     {
        if(text_bool)emit sendText("Błąd otwarcia pliku.", er_color);
     }
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Opis: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Producent: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Nr seryjny: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Lokalizacja: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Identyfikator: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Idetyfikator produktu: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    //ui->baudRateBox->addItem(QStringLiteral("14400"), QSerialPort::Baud14400);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(tr("Custom"));
    //ui->baudRateBox->setCurrentIndex(1);

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    //ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::reloadPortsInfo()
{
    fillPortsInfo();
    readPortSettings(true);
    updateSettings();
}

void SettingsDialog::readPortInfo()
{
    fillPortsInfo();
    //fillPortsParameters();
    readPortSettings(false);
    updateSettings();
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.stringDelay = ui->comDelay->text();

    //currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}

