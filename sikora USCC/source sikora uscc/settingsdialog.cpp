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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QCryptographicHash>

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
    connect(ui->ButtonBox, SIGNAL(rejected()), this, SLOT(EscButton()));

    connect(ui->LogButton, SIGNAL(pressed()), this, SLOT(LogPassword()));
    connect(ui->LogOutButton, SIGNAL(pressed()), this, SLOT(LogOut()));
    connect(ui->LogChangeButton, SIGNAL(pressed()), this, SLOT(LogChange()));

    //connect(ui->ButtonBox, SIGNAL(rejected()), this, SLOT(close()));

    //fillPortsInfo();
    //fillPortsParameters();
    //readPortSettings();
    //updateSettings();
    //readPortSettings(false);
    fillPortsInfo();
    fillPortsParameters();
    fillSettingsParameters();
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

void SettingsDialog::EscButton()
{
    hide();
}

void SettingsDialog::closeEvent ( QCloseEvent * ev) {
    apply();
    //writePortSettings();
    ev->accept();
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

void SettingsDialog::LogPassword()
{
    //QString text;
    qDebug() << "wywołanie1";
    log_setting = new LogSetting(stringPassword, this);
    connect(log_setting, SIGNAL(sendText(QString)), this, SLOT(checkText(QString)));
    //log_setting->show();
    log_setting->open();
}

void SettingsDialog::LogOut(){
    qDebug() << "wywołanie2";
    ui->additionalOptionsGroupBox->setEnabled(false);
}

void SettingsDialog::LogChange(){
    change_password = new ChngePassword(stringPassword, this);
    connect(change_password, SIGNAL(sendText(QString)), this, SLOT(checkText(QString)));
    change_password->open();
}

void SettingsDialog::checkText(QString text)
{
    QCryptographicHash hasher(QCryptographicHash::Sha1);
    qDebug() << text;

    if(text == "OK"){
        ui->additionalOptionsGroupBox->setEnabled(true);

    }
    else if(text == "NOK"){
        QMessageBox::about(this, tr("Błąd"),tr("Złe hasło"));
    }
    else if (text == "OLD_NOK") {
        QMessageBox::about(this, tr("Błąd"),tr("Złe stare hasło"));
    }
    else if (text == "NEW_NOK") {
        QMessageBox::about(this, tr("Błąd"),tr("Złe nowe hasło"));
    }
    else {
        hasher.addData(text.toUtf8());
        //stringPassword = text;
        stringPassword = hasher.result().toHex().toUpper();
        writePortSettings();
    }
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
    config_out << ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList().at(1) << "\n";
    qDebug() << ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList().at(1);
    //config_out << ui->deviceNameBox->currentIndex() << "\n";
    config_out << currentSettings.stringErrLoop << "\n";
    config_out << QVariant(currentSettings.onPLC).toString() << "\n";
    config_out << QVariant(currentSettings.directTurn).toString() << "\n";
    config_out << currentSettings.stringPlcAdress << "\n";
    config_out << currentSettings.stringPlcStartBit << "\n";
    config_out << currentSettings.stringPlcStopBit << "\n";
    config_out << currentSettings.stringPlcAlarmBit << "\n";
    config_out << currentSettings.stringPlcLenght << "\n";
    config_out << currentSettings.stringPlcSpeed << "\n";
    config_out << currentSettings.stringStartID1 << "\n";
    config_out << currentSettings.stringStartID2 << "\n";
    config_out << currentSettings.stringLE1 << "\n";
    config_out << currentSettings.stringLE2 << "\n";
    config_out << currentSettings.stringAdress << "\n";
    config_out << currentSettings.stringID1 << "\n";
    config_out << currentSettings.stringID2 << "\n";
    config_out << currentSettings.stringID3 << "\n";
    config_out << currentSettings.stringEndID1 << "\n";
    config_out << currentSettings.stringEndID2 << "\n";
    config_out << currentSettings.stringTolLenght.replace(",",".") << "\n";
    config_out << currentSettings.stringDbNr << "\n";
    config_out << stringPassword << "\n";
    //config_out << QVariant(currentSettings.dwordLenght).toString() << "\n";
    //config_out << QVariant(currentSettings.dwordSpeed).toString() << "\n";
    config_file.file.close();
}
else
{
    QMessageBox::about(this, tr("Błąd"), tr("błąd otwarcia pliku"));
}
}

void SettingsDialog::readPortSettings(bool text_bool)
{
    config_file.fileName = &file_setup;
    config_file.file.setFileName(*config_file.fileName);
    config_file.directionName.setCurrent(file_direction);

    if(config_file.file.open(QIODevice::ReadOnly | QIODevice::Text | QFile::Unbuffered))//linux QFile::Unbuffered
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
                    if(text_bool)emit sendText("Nie znaleziono portu.", er_color);
                }
            }
            else
            {
                qDebug() << "port ok";
                if(text_bool)emit sendText("Wykryto port.", ok_color);
            }

            //ui->deviceNameBox->setCurrentIndex(config_line[8].toInt());
            ui->errLoop->setValue(config_line[8].toDouble());
            ui->checkBoxPlc->setChecked(QVariant(config_line[9]).toBool());
            ui->checkBoxDirection_1->setChecked(QVariant(config_line[10]).toBool());
            ui->plcAdressEdit->setText(config_line[11]);
            ui->startBitEdit->setText(config_line[12]);
            ui->stopBitEdit->setText(config_line[13]);
            ui->alarmBitEdit->setText(config_line[14]);
            ui->lenghtRealEdit->setText(config_line[15]);
            ui->speedRealEdit->setText(config_line[16]);
            ui->startID1Edit_1->setText(config_line[17]);
            ui->startID2Edit_1->setText(config_line[18]);
            ui->le1Edit_1->setText(config_line[19]);
            ui->le2Edit_1->setText(config_line[20]);
            ui->adressEdit_1->setText(config_line[21]);
            ui->id1Edit_1->setText(config_line[22]);
            ui->id2Edit_1->setText(config_line[23]);
            ui->id3Edit_1->setText(config_line[24]);
            ui->endID1Edit_1->setText(config_line[25]);
            ui->endID2Edit_1->setText(config_line[26]);
            ui->tolLenght_1->setValue(config_line[27].toDouble());
            ui->dbNrEdit->setText(config_line[28]);
            stringPassword = config_line[29];
            /*
            if(QVariant(config_line[29]).toBool()){
                ui->buttonLenghtDword->setChecked(true);
            }
            else {
                ui->buttonLenghtWord->setChecked(true);
            }
            if(QVariant(config_line[30]).toBool()){
                ui->buttonSpeedDword->setChecked(true);
            }
            else {
                ui->buttonSpeedWord->setChecked(true);
            }
            */
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
    ui->baudRateBox->addItem(QStringLiteral("115000"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("153000"), 153000);
    ui->baudRateBox->addItem(QStringLiteral("750000"), 750000);
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

void SettingsDialog::fillSettingsParameters(){
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    // You may want to use QRegularExpression for new code with Qt 5 (not mandatory).
    QRegExp ipRegex ("^" + ipRange
                     + "(\\." + ipRange + ")?"
                     + "(\\." + ipRange + ")?"
                     + "(\\." + ipRange + ")?$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    QString adressRange = "\\0\\xHH";
    QString bitRange = "999\\.9";
    QString wordRange = "999";


    ui->startID1Edit_1->setInputMask(adressRange);
    ui->startID1Edit_1->setText("0xAA");
    ui->startID2Edit_1->setInputMask(adressRange);
    ui->startID2Edit_1->setText("0x51");
    ui->le1Edit_1->setInputMask(adressRange);
    ui->le1Edit_1->setText("0x18");
    ui->le2Edit_1->setInputMask(adressRange);
    ui->le2Edit_1->setText("0x00");
    ui->adressEdit_1->setInputMask(adressRange);
    ui->adressEdit_1->setText("0x01");
    ui->id1Edit_1->setInputMask(adressRange);
    ui->id1Edit_1->setText("0x4F");
    ui->le2Edit_1->setInputMask(adressRange);
    ui->id2Edit_1->setText("0x49");
    ui->id3Edit_1->setInputMask(adressRange);
    ui->id3Edit_1->setText("0x46");
    ui->endID1Edit_1->setInputMask(adressRange);
    ui->endID1Edit_1->setText("0xA5");
    ui->endID2Edit_1->setInputMask(adressRange);
    ui->endID2Edit_1->setText("0x5A");

    ui->plcAdressEdit->setInputMask("000.000.000.000");
    ui->plcAdressEdit->setText("192.168.000.001");
    ui->plcAdressEdit->setValidator(ipValidator);
    //ui->plcAdressEdit->setCursorPosition(0);
    ui->dbNrEdit->setInputMask(wordRange);
    ui->dbNrEdit->setText("002");
    ui->startBitEdit->setInputMask(bitRange);
    ui->startBitEdit->setText("000.0");
    ui->stopBitEdit->setInputMask(bitRange);
    ui->stopBitEdit->setText("000.1");
    ui->alarmBitEdit->setInputMask(bitRange);
    ui->alarmBitEdit->setText("000.2");
    ui->lenghtRealEdit->setInputMask(wordRange);
    ui->lenghtRealEdit->setText("002");
    ui->speedRealEdit->setInputMask(wordRange);
    ui->speedRealEdit->setText("006");
    //ui->deviceNameBox->addItem(QStringLiteral("LUMP2000"));
    //ui->deviceNameBox->addItem(QStringLiteral("LUMP6000"));
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
    currentSettings.stringTolLenght = ui->tolLenght_1->text();
    //currentSettings.stringTolLenght = QString::number(ui->tolLenght->value());
    //qDebug() << currentSettings.stringTolLenght;
    currentSettings.stringErrLoop = ui->errLoop->text();

    //currentSettings.deviceNameNr = ui->deviceNameBox->currentIndex();

    currentSettings.stringStartID1 = ui->startID1Edit_1->text();
    currentSettings.stringStartID2 = ui->startID2Edit_1->text();

    currentSettings.stringPlcStartBit = ui->startBitEdit->text();
    currentSettings.stringPlcStopBit = ui->stopBitEdit->text();
    //qDebug() << currentSettings.stringPlcStopBit;
    currentSettings.stringPlcAlarmBit = ui->alarmBitEdit->text();
    currentSettings.stringPlcLenght = ui->lenghtRealEdit->text();
    currentSettings.stringPlcSpeed = ui->speedRealEdit->text();

    currentSettings.stringLE1 = ui->le1Edit_1->text();
    currentSettings.stringLE2 = ui->le2Edit_1->text();

    currentSettings.stringAdress = ui->adressEdit_1->text();
    currentSettings.stringPlcAdress = ui->plcAdressEdit->text();
    qDebug()<<currentSettings.stringPlcAdress;

    currentSettings.stringID1 = ui->id1Edit_1->text();
    currentSettings.stringID2 = ui->id2Edit_1->text();
    currentSettings.stringID3 = ui->id3Edit_1->text();

    currentSettings.stringEndID1 = ui->endID1Edit_1->text();
    currentSettings.stringEndID2 = ui->endID2Edit_1->text();

    //currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
    currentSettings.onPLC = ui->checkBoxPlc->isChecked();
    currentSettings.directTurn = ui->checkBoxDirection_1->isChecked();

    currentSettings.stringDbNr = ui->dbNrEdit->text();
    //currentSettings.dwordLenght = ui->buttonLenghtDword->isChecked();
    //currentSettings.dwordSpeed = ui->buttonSpeedDword->isChecked();
}

