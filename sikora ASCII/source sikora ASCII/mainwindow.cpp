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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "mylineedit.h"
#include "flashingpushbutton.h"
#include "linesetting.h"
#include "tabletext.h"

#include "xlsxdocument.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "xlsxformat.h"
using namespace QXlsx;
#include <QTextStream>
#include <QFileDialog>

#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QScrollArea>


//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
//! QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support

    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->LumpEdit->setReadOnly(true);
    ui->NecEdit->setReadOnly(true);
    ui->SpeedEdit->setReadOnly(true);
    //ui->LengthTolEdit->setReadOnly(true);
    ui->LumpNrEdit->setReadOnly(true);
    ui->NecNrEdit->setReadOnly(true);

    lump_setting = new LineSetting(tol_lump , this);
    nec_setting = new LineSetting(tol_necdown, this);
    speed_setting = new LineSetting(speed, this);
    //lenght_setting = new LineSetting(tol_length, this);
    //diameter_setting = new LineSetting(diameter, this);

    table = new tableText(ui->LumpNecTableView);
/*
    for(int i=1;i<100;i++)
    {
        QString text = "1.11";
        table->setText(&text);
    }
*/

//! [1]
    serial = new QSerialPort(this);
//! [1]
    settings = new SettingsDialog;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    Timer = new QTimer(this);
    //start red data devices
    Timer->setInterval(delayTime);

    initActionsConnections();

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);
//! [2]

//! [2]

//! [3]
    //reload ();//sparwdzenie czy jest przejściówka, po załączniu programu

    //ui->checkBoxStop->setChecked(true);
    ui->ClearAlarmButton->backgroundRole();
    //QFile qss(“stylesheet.css”);
    //qss.open(QFile::ReadOnly);
    //this->setStyleSheet(qss.readAll());
    //qss.close();

    //QStandardItemModel *model = new QStandardItemModel(0,2,this); //2 Rows and 2 Columns
    /*
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Nec/Lump[mm]")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("dł.[m]")));
    ui->LumpNecTableView->setModel(model);
    model->setRowCount(model->rowCount() + 10);//zwiększenie wiersza
    index = model->index(0,0,QModelIndex());//przypisanie indesu
    model->setData(index,"123");//wpisanie stałej
    //QString text = "dsdsd";
    //setText (&text);
    */
/*
    ui->StartStopButton->setEnabled(true);
    ui->ResetButton->setEnabled(true);
    ui->NecEdit->setEnabled(true);
    ui->LumpEdit->setEnabled(true);
    nec_setting->show();
*/
}
//! [3]

MainWindow::~MainWindow()
{
    delete nec_setting;
    delete lump_setting;
    delete speed_setting;
    //delete lenght_setting;
    delete settings;
    delete ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    reload();
    SettingsDialog::Settings p = settings->settings();

    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        ui->ClearAlarmButton->setEnabled(true);
        showStatusMessage(tr("Połączono do %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        //delaytime
        //qDebug() << p.stringDelay;
        p.stringDelay.remove("ms");
        //qDebug() << p.stringDelay;
        delayTime = p.stringDelay.toLong();
        //red settings devices
        readDataDevices();
        //Reset_Fault();
    } else {
        reciveMesage("Błąd otwarcia portu.", er_color);
    }
}
//! [4]
void MainWindow::initActionsConnections()
{
    //connect(ui->);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, settings, &MainWindow::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    //wywołanie funkcji wyszukiwania portów
    connect(ui->actionConfigure, &QAction::triggered, this, &MainWindow::readPortInfo);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::reload);

    //naciśnięcia myszy wywołanie okna
    connect(ui->NecEdit, SIGNAL(mousePressedLineEdit()), this, SLOT(Mouse_Pressed_Nec()));
    connect(ui->LumpEdit, SIGNAL(mousePressedLineEdit()), this, SLOT(Mouse_Pressed_Lump()));
    connect(ui->SpeedEdit, SIGNAL(mousePressedLineEdit()), this, SLOT(Mouse_Pressed_Speed()));
    //connect(ui->LengthTolEdit, SIGNAL(mousePressedLineEdit()), this, SLOT(Mouse_Pressed_Lenght()));
    //connect(ui->DiameterEdit, SIGNAL(mousePressedLineEdit()), this, SLOT(Mouse_Pressed_Diameter()));

    //przekazanie zmiennej pomiedzy formularzami
    connect(nec_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    connect(lump_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    connect(speed_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(lenght_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(diameter_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));

    connect(settings, SIGNAL(sendText(QString, QString)), this, SLOT(reciveMesage(QString, QString)));
    //wywołanie obsługi urządzenia
    connect(Timer, SIGNAL(timeout()), this, SLOT(readDefault()));

    //przyciski okna głównego
    connect(ui->StartStopButton, SIGNAL(pressed()), this, SLOT(Start_Stop_Mes()));
    connect(ui->ResetButton, SIGNAL(pressed()), this, SLOT(Reset_Fault()));
    connect(ui->ResMsgButton, SIGNAL(pressed()), this, SLOT(clr_msg()));
    connect(ui->ResNecLumpMsg, SIGNAL(pressed()), this, SLOT(clr_lump_nec()));
    connect(ui->ClearAlarmButton, SIGNAL(pressed()),this, SLOT(set_clr_alarm()));
    connect(ui->holdButton, SIGNAL(pressed()),this, SLOT(hold_Mes()));

    //connect(ui->rowDelButton, SIGNAL(pressed()),this, SLOT(delRowTable()));
    connect(ui->setText, SIGNAL(pressed()),this, SLOT(generateExcel()));
    //connect(ui->LumpNecTableView, SIGNAL(dellPressedKeyTable()), this, SLOT());
}
//! [5]
void MainWindow::writeData(const char *data, qint8 length)
{
    serial->write(data, length);

    if(!serial->waitForBytesWritten(delayTime))
    {
        reciveMesage("Przekroczono czas nadawania.", er_color);
    }
    //serial->write(data);
}
//! [7]
const QByteArray MainWindow::readData()
{
    QByteArray data = 0;

    if (serial->waitForReadyRead(delayTime)) {
        data = serial->readAll();
        while (serial->waitForReadyRead(30))
            data += serial->readAll();
    } else {
        //QMessageBox::about(this, tr("Błąd"), tr("Przekroczono czas odczytu"));
        //ui->ErrorTextEdit->moveCursor(QTextCursor::End);
        //ui->ErrorTextEdit->insertPlainText("Przekroczono czas odczytu.\n");
        reciveMesage("Przekroczono czas odczytu.", er_color);
    }

    //data = serial->readAll();
    //serial->waitForReadyRead(delayTime);
    return data;
}
void MainWindow::closeSerialPort()
{
    if(Timer->isActive()){
        Timer->stop();
        sleep->msleep(delayTime);
    }

    if (serial->isOpen())
    {
        if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
        serial->close();
    }

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->StartStopButton->setEnabled(false);
    ui->StartStopButton->setText("Start");
    ui->ResetButton->setEnabled(false);
    ui->NecEdit->setEnabled(false);
    ui->LumpEdit->setEnabled(false);
    ui->SpeedEdit->setEnabled(false);
    //ui->LengthTolEdit->setEnabled(false);
    ui->ClearAlarmButton->setEnabled(false);
    showStatusMessage(tr("Rozłączono"));
}

bool MainWindow::checkData(const QByteArray data, bool errorCount)
{
    quint8 i = 0;
    quint8 next_data = STX_index + 1;
    char BCC = 0;

    if(!data.isEmpty())
    {
        if(!errCount)errCount = 0;
        while((i < data.length()) && (data.at(i) != ETX))i++;
        i++;//BCC index
        /*
        qDebug() << data;
        qDebug() << data[0]+0;
        qDebug() << data[1]+0;
        qDebug() << data[2]+0;
        qDebug() << data[3]+0;
        qDebug() << data[i]+0;
        */
        if(data.at(0) == STX)
        {
            //BCC check
            do{//BCC count
                BCC ^= data.at(next_data);//.toLatin1();
                next_data++;
            }while(next_data < i);

             if(BCC < 0x20) BCC += 0x20;
             if(BCC != data.at(i))
             {
                 //QMessageBox::about(this, tr("Błąd"),
                   //                 tr("<b>Błąd sumy logicznej BCC</b>"
                    //                   "Sprawdż polaryzację podłącznia RS485"));
                 reciveMesage("Błąd sumy logicznej BCC.", er_color);
                 if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
                 return false;
             }
             //check reaction to request
             if(data.at(i - 2) == NAK)
             {
                 if(i == 3)
                 {
                    //QMessageBox::about(this, tr("Błąd"),
                     //               tr("<b>Urządznie zgłasza złe dane</b>"));
                    reciveMesage("Urządznie zgłasza złe dane.", er_color);
                    if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
                    return false;
                 }
                 else
                 {
                     //QMessageBox::about(this, tr("Błąd"),
                     //                tr("<b>Urządznie zgłasza złe ID ramki</b>"));
                     reciveMesage("Urządznie zgłasza złe ID ramki.", er_color);
                     if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
                     return false;
                 }
             }
             else
             {
                return true;//Data ok or response
             }
        }
    }

/*
    QMessageBox::about(this, tr("Błąd"),
                       tr("<b>Brak połacznia z urządzeniem </b>"
                          "Sprawdż ustawienia portu i numer portu"));

                      */
    if(errorCount){
        if(errCount > err_count_max){
            if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
            reciveMesage("Brak połącznia z urządzeniem.", er_color);
            closeSerialPort();
            errorCount = 0;
        }
    }
    else {
        if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
        reciveMesage("Brak połącznia z urządzeniem.", er_color);
        closeSerialPort();
    }
    return false;
}
//odczyt danych z sikory
void MainWindow::readDataDevices()
{
    QByteArray data, data_fault;

    data.resize(read_data_lenght);

    writeRequest(tol_lump);

    data = readData();

    if(checkData(data, false))
    {
        //write data to lineedit-wpisanie danych w okinko lump
        data.remove(0, 1);
        data.remove(3, data.size());
        //data.insert(0, '0');//lump6000 add
        data.insert(1, '.');
        ui->LumpEdit->setText(data);
        lump_setting->value->setText(data);
        writeRequest(tol_necdown);

        data = readData();

        if(checkData(data, false))
        {//wpisanie danych w okinko lump necdown
            data.remove(0, 1);
            data.remove(3, data.size());
            //data.insert(0, '0');//lump6000 add
            data.insert(1, '.');
            nec_setting->value->setText(data);
            ui->NecEdit->setText(data);
            /*
            writeRequest(tol_length);

            data = readData();

            if(checkData(data))
            {
                data.remove(0, 1);
                data.remove(3, data.size());
                data.insert(2, '.');
                lenght_setting->value->setText(data);
                ui->LengthTolEdit->setText(data);

                //start button enabled
                ui->StartStopButton->setEnabled(true);
                ui->ResetButton->setEnabled(true);
                ui->NecEdit->setEnabled(true);
                ui->LumpEdit->setEnabled(true);
                ui->LengthTolEdit->setEnabled(true);
                writeSetting(res_device, "1");//zerowanie urzadzenia
                //MainWindow::Reset_Fault();//zerowanie urzadzenia i ilości błedów w form
                //nr_faults_to_clr = 0;//zerowanie licznika błędów
            }
            */

            //button enabled
            ui->StartStopButton->setEnabled(true);
            ui->ResetButton->setEnabled(true);
            ui->NecEdit->setEnabled(true);
            ui->LumpEdit->setEnabled(true);
            ui->SpeedEdit->setEnabled(true);
            writeSetting(res_device, "1");//zerowanie urzadzenia
        }
    }

}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("Sikora terminal"),
                       tr(""));
}

//! [6]
void MainWindow::writeSetting(quint8 setting, const QString &text_data)
{
    quint8 next_data = STX_index + 1;
    quint8 BCC_index;
    char data[set_lenght];

    data[STX_index] = STX;

    switch (setting) {//ID
    case tol_lump:
        data[IDB1_index] = '0';
        data[IDB2_index] = '1';
        data[IDB3_index] = '9';
        break;
    case tol_necdown:
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '0';
        break;
   /* case speed:
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '0';
        break;*/
    case tol_length:
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '1';
        break;
    case res_met:
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '2';
        break;
    case res_device:
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '3';
        break;
    case res_alarm:
        //data[STX_index] = STX_alarm;
        data[IDB1_index] = '0';
        data[IDB2_index] = '1';
        data[IDB3_index] = '1';
        break;
    case set_alarm:
        //data[STX_index] = STX_alarm;
        data[IDB1_index] = '0';
        data[IDB2_index] = '1';
        data[IDB3_index] = '2';
        break;
    default:
        break;
    }

    if(setting > 2){
        data[DB1_index] = text_data.toLocal8Bit().at(0);
        BCC_index = DB3_index;
    }
    else{
        data[DB1_index] = text_data.toLocal8Bit().at(0);
        data[DB2_index] = text_data.toLocal8Bit().at(1);
        data[DB3_index] = text_data.toLocal8Bit().at(2);
        BCC_index = DB3_index + 2;
    }

    data[BCC_index - 1] = ETX;
    data[BCC_index] = 0;

    do{//BCC count
        data[BCC_index] ^= data[next_data];
        next_data++;
    }while(next_data < BCC_index);

    if(data[BCC_index] < 0x20) data[BCC_index] += 0x20;
    if(setting > 4) data[BCC_index] += 1;//dane dla "pic", sikora toleruje ramki ze złym BCC

    //serial->write(data, set_lenght);
    writeData(data, BCC_index+1);
    //serial->waitForBytesWritten(delayTime);
    //sleep->msleep(delayTime);
}

void MainWindow::writeRequest(quint8 request)
{
    quint8 next_data = STX_index + 1;
    char data[wr_lenght];

    data[STX_index] = STX;
    data[BCC_reqest - 1] = ETX;
    data[BCC_reqest] = 0x00;//BCC

    switch (request) {//ID
    case tol_lump://tolerance lump
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '4';
        break;
    case tol_necdown://tolerance nec
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '5';
        break;
    case tol_length://tolernace length
        data[IDB1_index] = '0';
        data[IDB2_index] = '2';
        data[IDB3_index] = '6';
        break;
    case status_rd://staus
        data[IDB1_index] = '0';
        data[IDB2_index] = '3';
        data[IDB3_index] = '8';
        break;
    case fault_rd://actual fault
        data[IDB1_index] = '3';
        data[IDB2_index] = '1';
        data[IDB3_index] = '0';
        break;
    case setting_rd://actual setting value
        data[IDB1_index] = '3';
        data[IDB2_index] = '1';
        data[IDB3_index] = '1';
        break;
    default:
        break;
    }

    do{//BCC count
        data[BCC_reqest] ^= data[next_data];
        next_data++;
    }while(next_data < BCC_reqest);

    if(data[BCC_reqest] < 0x20) data[BCC_reqest] += 0x20;

    //serial->write(data, wr_lenght);
    writeData(data, wr_lenght);
    //serial->waitForBytesWritten(delayTime);
    //sleep->msleep(delayTime);
}
//! [6]

void MainWindow::readDefault()
{
    QByteArray data, data_fault, height_fault;
    quint16 fault_counter;
    //quint16 height_lump_nec;
    //quint16 lump_nec_counter;
    quint16 fault_type = 0;
    quint16 nr_fault;
    QString text;

    data_fault.resize(4);
    data.resize(read_data_lenght);

    height_fault.resize(4);

    writeRequest(fault_rd);
    //dodać opóżnienie
    //sleep->msleep(delayTime);
    data = readData();
    if(checkData(data, true))
    {
        //check fault number
         data_fault[0] = data.at(1);
         data_fault[1] = data.at(2);
         data_fault[2] = data.at(3);
         data_fault[3] = data.at(4);
         fault_counter = data_fault.toUInt();
         //przygotowanie tekstu do wpisania w tabelkę
         text[0] = QChar(data.at(6));
         text[1] = QChar(data.at(7));
         text[2] = QChar(data.at(8));

         //text.insert(0,'0');//lump6000 add
         text.insert(1,'.');
         text.append(" mm\n");


        // lump_nec_counter = ui->LumpNrEdit->text().toInt();
        // lump_nec_counter += ui->NecNrEdit->text().toInt();
        //fgault type
        fault_type = data.at(fault_type_index);//.digitValue();
        fault_type -= 0x30;
        /*
        if(fault_counter > lump_nec_counter)
        {
            fault_counter -= lump_nec_counter;
            lump_nec_counter += fault_counter;
            nr_fault = lump_nec_counter;
        }
        */
        switch (fault_type) {
        case lump:
                writeSetting(res_device, "1");
                data = readData();
                nr_fault = ui->LumpNrEdit->text().toInt();
                nr_fault += fault_counter;
                ui->LumpNrEdit->setText(QString::number(nr_fault));

                text.prepend("Lump : ");
                //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                //ui->NecLumpTextEdit->insertPlainText(text);
                table->setText(&text);
                if(ui->checkBoxAlarm->checkState()== true){
                    if(ui->holdButton->text() == "Zatrzymanie\nTAK" || ui->checkBoxAlarm->checkState()== true)
                    {
                        writeSetting(set_alarm, "1");
                    }
                    else
                    {
                        writeSetting(set_alarm, "0");
                    }
                    bit_clr_alarm = false;
                }
                //if(!checkData(data))break;
            break;
        case nec:
                writeSetting(res_device, "1");
                data = readData();
                nr_fault = ui->NecNrEdit->text().toInt();
                nr_fault += fault_counter;
                ui->NecNrEdit->setText(QString::number(nr_fault));

                //text = QString::number(height_lump_nec);
                //text.insert(1,'.');
                text.prepend("Nec : ");
                //text.append(" mm\n");
                //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                //ui->NecLumpTextEdit->insertPlainText(text);
                table->setText(&text);
                if(ui->checkBoxAlarm->checkState()== true){
                    if(ui->holdButton->text() == "Zatrzymanie\nTAK" || ui->checkBoxAlarm->checkState()== true)
                    {
                        writeSetting(set_alarm, "1");
                    }
                    else
                    {
                        writeSetting(set_alarm, "0");
                    }
                    bit_clr_alarm = false;
                }
            break;

        case lump_dark:
                writeSetting(res_device, "1");
                data = readData();
                nr_fault = ui->LumpNrEdit->text().toInt();
                nr_fault += fault_counter;
                ui->LumpNrEdit->setText(QString::number(nr_fault));

                //text = QString::number(height_lump_nec);
                //text.insert(1,'.');
                text.prepend("Lump : ");
                //text.append(" mm\n");
                //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                //ui->NecLumpTextEdit->insertPlainText(text);
                table->setText(&text);
                if(ui->checkBoxAlarm->checkState()== true){
                    if(ui->holdButton->text() == "Zatrzymanie\nTAK" || ui->checkBoxAlarm->checkState()== true)
                    {
                        writeSetting(set_alarm, "1");
                    }
                    else
                    {
                        writeSetting(set_alarm, "0");
                    }
                    bit_clr_alarm = false;
                }
                reciveMesage("Ustaw urzadzenie w osi pomiaru.", er_color);
            break;

        case nec_dark:
                writeSetting(res_device, "1");
                data = readData();
                nr_fault = ui->NecNrEdit->text().toInt();
                nr_fault += fault_counter;
                ui->NecNrEdit->setText(QString::number(nr_fault));

                //text = QString::number(height_lump_nec);
                //text.insert(1,'.');
                text.prepend("Nec : ");
                //text.append(" mm\n");
                //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                //ui->NecLumpTextEdit->insertPlainText(text);
                table->setText(&text);
                if(ui->checkBoxAlarm->checkState()== true){
                    if(ui->holdButton->text() == "Zatrzymanie\nTAK" || ui->checkBoxAlarm->checkState()== true)
                    {
                        writeSetting(set_alarm, "1");
                    }
                    else
                    {
                        writeSetting(set_alarm, "0");
                    }
                    bit_clr_alarm = false;
                }
                reciveMesage("Ustaw urzadzenie w osi pomiaru.", er_color);
            break;

        case no_power:
            //QMessageBox::about(this, tr("Błąd"),
            //                   tr("<b>Brak zasilania optyki</b>"));
            reciveMesage("Brak zasilania optyki.", er_color);
            //writeSetting(set_alarm, "0");//miganie diody
            break;

        case error_device:
            //QMessageBox::about(this, tr("Błąd"),
            //                   tr("<b>Uszkodzenie urządzenia</b>"));
            reciveMesage("Brak zasilania optyki.", er_color);
            //writeSetting(set_alarm, "0");//miganie diody
            break;

        default:
            break;
        }

        if(bit_clr_alarm == true)
        {
            if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
            bit_clr_alarm = false;
        }
/*
        if(fault_type < 5 || fault_type > 4)
        {
            Timer->stop();

            if(fault_type < 5)
            {

                writeSetting(res_device, "1");

                if(!checkData(data))
                {
                    ui->ErrorTextEdit->moveCursor(QTextCursor::End);
                    ui->ErrorTextEdit->insertPlainText("Brak odpowiedzi.\n");
                }

                writeSetting(set_alarm, "1");
                bit_clr_alarm = false;
            }

            if(fault_type > 4)
            {
                writeSetting(set_alarm, "0");
                bit_clr_alarm = false;
            }

            if(bit_clr_alarm == true)
            {
                writeSetting(res_alarm, "1");
                bit_clr_alarm = false;
            }

            Timer->setInterval(delayTime);
            Timer->start();
        }
        */
    }
}
//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Błąd krytyczny"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}
//-----------------------wywołanie po naciśnięciu myszką na okienko do edycji------------------------------------------------
void MainWindow::Mouse_Pressed_Nec()
{
    if (serial->isOpen())
    {
        nec_setting->show();
    }
}

void MainWindow::Mouse_Pressed_Lump()
{
    if (serial->isOpen())
    {
        lump_setting->show();
   }
}

void MainWindow::Mouse_Pressed_Speed()
{
    //if (serial->isOpen())
    //{
        speed_setting->show();
    //}
}

void MainWindow::Mouse_Pressed_Lenght()
{
    if (serial->isOpen())
    {
        //lenght_setting->show();
    }
}

void MainWindow::Mouse_Pressed_Diameter()
{
    //diameter_setting->show();
}

void MainWindow::Start_Stop_Mes()
{
    if (serial->isOpen())
    {
        if(ui->StartStopButton->text() == "Start")
        {
            //start red data devices
            //czyszcenie bufora
            Timer->setInterval(delayTime);//Timer->setInterval(2*delayTime);
            Timer->start();
            ui->StartStopButton->setText("Stop");
            ui->ResetButton->setEnabled(false);
        }
        else{
            Timer->stop();
            ui->StartStopButton->setText("Start");
            ui->ResetButton->setEnabled(true);
        }
    }
}

void MainWindow::hold_Mes()
{
    if(ui->holdButton->text() == "Zatrzymanie\nTAK")
    {
        ui->holdButton->setText("Zatrzymanie\nNIE");
        //ui->holdButton->setStyle(QStyle("background-color:#768d87;border:2px solid#566963;"));
    }
    else{
        ui->holdButton->setText("Zatrzymanie\nTAK");
    }
}

//wpisanie wartości do okienek i wysłanie do urzadzenia
void MainWindow::receiverText(QString text, const quint8 id)
{
    QByteArray data;
    if(Timer->isActive()){
        QMessageBox::about(this, tr("Błąd"),
                           tr("<b>Zatrzymaj pomiar, przycisk Stop</b>"));
    }
    else{
        switch (id) {
        case tol_lump:
            ui->LumpEdit->setText(text);
            //text.remove(4,6);//przy wysyłaniu pobiera tylko potrzebne znaki
            //qDebug() << text;
            //text.remove(0,2);//lump6000
            text.remove(1,1);//lump2000
            //qDebug() << text;
            writeSetting(tol_lump, text);
            data = readData();
            checkData(data, false);
            break;
        case tol_necdown:
            ui->NecEdit->setText(text);
            //text.remove(0,2);//lump6000
            text.remove(1,1);//lump2000
            writeSetting(tol_necdown, text);
            data = readData();
            checkData(data, false);
            break;
        case speed:
            ui->SpeedEdit->setText(text);
            //text.remove(0,2);//lump6000
            //text.remove(1,1);//lump2000
            writeSetting(speed, text);
            data = readData();
            checkData(data, false);
            break;
            /*
        case tol_length:
            ui->LengthTolEdit->setText(text);
            //qDebug() << text;
            text.remove(2,1);
            //qDebug() << text;
            writeSetting(tol_length, text);
            data = readData();
            checkData(data);
            break;
            */
        case diameter:
            //ui->DiameterEdit->setText(text);
            break;
        default:
            break;
        }
    }
}

//reset urzadzenia
void MainWindow::Reset_Fault()
{
    ui->NecNrEdit->setText("0");
    ui->LumpNrEdit->setText("0");
    writeSetting(res_device, "1");
    writeSetting(res_device, "1");
}

//zerowanie informacji o błędach
void MainWindow::clr_msg()
{
    /*
    //nec_flash.terminate();
    //lump_flash.terminate();
    ui->LumpFrame->setStyleSheet("color:black");
    ui->NecFrame->setStyleSheet("color:black");
    flag_nec = false;
    flag_lump = false;
    */
    ui->ErrorTextEdit->clear();
    if(Timer->isActive())
    {
        bit_clr_alarm = true;
    }
    else
    {
        if(serial->isOpen())
        {
            if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
        }
    }
}

void MainWindow::clr_lump_nec()
{
    //ui->NecLumpTextEdit->clear();
    table->initTable();
}

void MainWindow::delRowTable()
{
    table->tableDelRows();
}

//ustaw bit zeruj alarm
void MainWindow::set_clr_alarm()
{
    if(Timer->isActive())
    {
        bit_clr_alarm = true;
    }
    else
    {
        if(serial->isOpen())
        {
            if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
        }
    }
}

void MainWindow::reload ()
{
    settings->reloadPortsInfo();
}

void MainWindow::readPortInfo()
{
    settings->readPortInfo();
}

void MainWindow::reciveMesage(QString text, QString color)
{
    ui->ErrorTextEdit->moveCursor(QTextCursor::End);
    QTextCursor cursor( ui->ErrorTextEdit->textCursor() );
    QTextCharFormat format;
    format.setFontWeight( QFont::DemiBold );
    format.setForeground( QBrush( QColor( color ) ) );
    //format.setFontOverline(true);
    //format.setBackground (QBrush( QColor( color ) ) );
    cursor.setCharFormat( format );
    cursor.insertText(text + "\n");
}
//===================================================================
void MainWindow::generateExcel()
{
    QDateTime date (QDateTime::currentDateTime().date());
    //const char file_name[] = "Pliki tekstowe (*.xlsx)";
    QString filePath = QFileDialog::getSaveFileName
            //(this,tr("Zapisz plik jako..."),tr("/home/"),tr(file_name));
                    (this,tr("Zapisz plik jako..."),tr("/home/")+date.date().toString(),
                     tr("Pliki tekstowe (*.xlsx)"));
    QString row;
    //![0]
    QXlsx::Document xlsx(filePath);
    //![0]

    //![1]
    //QXlsx::Format blue;
    //blue.setFontColor(Qt::blue);
    //QXlsx::Format red;
    //red.setFontColor(Qt::red);
    //red.setFontSize(15);
    QXlsx::Format size12;
    size12.setFontSize(12);
    size12.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    QXlsx::RichString rich;
    rich.addFragment("Ustawienia:\n", size12);
    rich.addFragment("Lump: "+ui->LumpEdit->text()+"\n", size12);
    rich.addFragment("Nec: "+ui->NecEdit->text()+"\n", size12);
    xlsx.write("C1", rich);
    xlsx.mergeCells("C1:E5", size12);
    QXlsx::RichString A1;
    A1.addFragment(table->model->horizontalHeaderItem(0)->text(), size12);
    xlsx.write("A1",A1);
    QXlsx::RichString B1;
    B1.addFragment(table->model->horizontalHeaderItem(1)->text(), size12);
    xlsx.write("B1",B1);

    for(int i=0; i<table->model->rowCount(); i++)
    {
        row = QString("A%1").arg(i+2);
        xlsx.write(row, table->model->itemFromIndex(table->model->index(i,0,QModelIndex()))->text());
        row = QString("B%1").arg(i+2);
        xlsx.write(row, table->model->itemFromIndex(table->model->index(i,1,QModelIndex()))->text());
    }

//w formacie html
    //xlsx.workbook()->setHtmlToRichStringEnabled(true);
    //xlsx.write("B4", "<b>Hello</b> <font color=\"red\">Qt</font> <i>Xlsx</i>");

    //xlsx.write("B6", "<font color=\"red\"><b><u><i>Qt Xlsx</i></u></b></font>");

    //![1]

    //![2]
    xlsx.save();
    //xlsx.saveAs("Test1.xlsx");
    //![2]
    //QXlsx::Document("Test1.xlsx");
    //xlsx.saveAs("Test2.xlsx");
}
