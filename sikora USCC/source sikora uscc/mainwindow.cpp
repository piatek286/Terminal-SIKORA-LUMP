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
    //serial(new QSerialPort(this))
{
//! [0]
//! QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support

    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->LumpEdit->setReadOnly(true);
    ui->NecEdit->setReadOnly(true);
    ui->SpeedEdit->setReadOnly(true);
    ui->LumpNrEdit->setReadOnly(true);//#DF0101;
    ui->NecNrEdit->setReadOnly(true);
    ui->OnOffLabel->setStyleSheet("color: #DF0101");

    //all_setting = new LineSetting(device , this);
    //lump_setting = new LineSetting(tol_lump , this);
    //nec_setting = new LineSetting(tol_necdown, this);
    //speed_setting = new LineSetting(speed, this);
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
    reload();
    settingsRead();
    plcSiemens = new PLC_Siemens(plc_adress);//192.168.002.010

//próbny
    /*
    Timer_plc = new QTimer(this);//próbny
    if(plcSiemens ->connect()){
        qDebug()<<"Połączono z plc";
       // Timer_plc->setInterval(200);
       // Timer_plc->start();
    }
*/
/*
    byte buffer[12];
     qDebug()<<"Łącznie z plc";
    if(plcSiemens ->connect()){
        qDebug()<<"Połączono z plc";
        if(plcSiemens->ReadCycle(db_nr, start_offset, 10, buffer)){
            line_speed = plcSiemens->getUInt32(buffer,speed_offset);
            lenght = plcSiemens->getUInt32(buffer,lenght_offset);
            ui->SpeedEdit->setText(QString::number(line_speed));
            start_bit_value = (buffer[start_offset] & (0x01<<start_bit));
        }
        //plc_on = true;
        //if(plcSiemens->disConnect())qDebug()<<"rozłączono";
        //Timer_plc->setInterval(100);
        //Timer_plc->start();
    }
*/
    initActionsConnections();


/*
       byte buffer[12];
       TS7Client *MyS7Client;
       MyS7Client = new TS7Client();
        if ( !MyS7Client->Connected() )
        {
            MyS7Client->ConnectTo("192.168.2.10",0,0);//adres bez niepotrzebnych zer
            MyS7Client->DBRead(10, 0, 10, &buffer);//size liczymy od 1, ofset ostaniej zmiennej plus jej długość długość
            delete MyS7Client;
            qDebug()<<QString::number(buffer[0]);
            qDebug()<<QString::number(buffer[1]);
            qDebug()<<QString::number(buffer[2]);
            qDebug()<<QString::number(buffer[3]);
            qDebug()<<QString::number(buffer[4]);
            qDebug()<<QString::number(buffer[5]);
            qDebug()<<QString::number(buffer[6]);
            qDebug()<<QString::number(buffer[7]);
            qDebug()<<QString::number(buffer[8]);
            qDebug()<<QString::number(buffer[9]);
        }
*/


}

MainWindow::~MainWindow()
{
    //delete nec_setting;
    //delete lump_setting;
    //delete speed_setting;
    delete all_setting;
    delete settings;
    delete ui;
}

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
    //connect(nec_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(lump_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(speed_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(all_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));

    //connect(lenght_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
    //connect(diameter_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));

    connect(settings, SIGNAL(sendText(QString, QString)), this, SLOT(reciveMesage(QString, QString)));
    //wywołanie obsługi urządzenia
    connect(Timer, SIGNAL(timeout()), this, SLOT(readDevice()));
    //connect(Timer_plc, SIGNAL(timeout()), this, SLOT(readPlc1()));//próbny

    //przyciski okna głównego
    connect(ui->StartButton, SIGNAL(pressed()), this, SLOT(Start_Mes()));
    connect(ui->StopButton, SIGNAL(pressed()), this, SLOT(Stop_Mes()));
    connect(ui->ResetButton, SIGNAL(pressed()), this, SLOT(Reset_Fault()));
    connect(ui->ResMsgButton, SIGNAL(pressed()), this, SLOT(clr_msg()));
    connect(ui->ResNecLumpMsg, SIGNAL(pressed()), this, SLOT(clr_lump_nec()));
    connect(ui->ClearAlarmButton, SIGNAL(pressed()),this, SLOT(set_clr_alarm()));
    connect(ui->holdButton, SIGNAL(pressed()),this, SLOT(hold_Mes()));

    //connect(ui->rowDelButton, SIGNAL(pressed()),this, SLOT(delRowTable()));
    connect(ui->setText, SIGNAL(pressed()),this, SLOT(generateExcel()));
    //connect(ui->LumpNecTableView, SIGNAL(dellPressedKeyTable()), this, SLOT());
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);
}

/*
void MainWindow::readPlc1()//próbny
{
    //unsigned char buffer[10];
    unsigned char *buffer = new unsigned char[speed_offset + 4 - start_offset];

    if(plcSiemens->ReadCycle(db_nr, start_offset, 10, buffer)){
        line_speed = plcSiemens->getUInt32(buffer,speed_offset);
        lenght = plcSiemens->getUInt32(buffer,lenght_offset);
        ui->SpeedEdit->setText(QString::number(line_speed));
        start_bit_value = (buffer[start_offset] & (0x01<<start_bit));

        if(!start_bit_value){
            ui->plc_state->setText("Stop");

            buffer[alarm_offset] &= (~(1<<alarm_bit));
            plcSiemens->WriteCycle(db_nr, alarm_offset, 1, &buffer[alarm_offset]);//zapis bajtu

            buffer[stop_offset] &= (~(0x01<<stop_bit));//zatrzymanie urządzenia
            plcSiemens->WriteCycle(db_nr, stop_offset, 1, &buffer[stop_offset]);//zapis bajtu
        }
        else {//załącznie ponowne pomiaru po starcie urządzenia//dodać sprawdzenie przycisku
            read_on = true;
            ui->plc_state->setText("Start");

            buffer[alarm_offset] |= (1<<alarm_bit);
            plcSiemens->WriteCycle(db_nr, alarm_offset, 1, &buffer[alarm_offset]);//zapis bajtu

            buffer[stop_offset] |= (0x01<<stop_bit);//zatrzymanie urządzenia
            plcSiemens->WriteCycle(db_nr, stop_offset, 1, &buffer[stop_offset]);//zapis bajtu
        }

    }
}
*/
void MainWindow::readPlc()
{
    //unsigned char buffer[10];
    unsigned char *buffer = new unsigned char[speed_offset + 4 - start_offset];

    if(plcSiemens->ReadCycle(db_nr, start_offset, 10, buffer)){
        line_speed = plcSiemens->getUInt32(buffer,speed_offset);
        lenght = plcSiemens->getUInt32(buffer,lenght_offset);
        ui->SpeedEdit->setText(QString::number(line_speed));
        start_bit_value = (buffer[start_offset] & (0x01<<start_bit));

        if(!start_bit_value && read_on){//sprawdzenie czy zatrzymano urzadznie
            read_on = false;//stop odczytu
        }

        if (start_bit_value && !read_on && (ui->StopButton->isEnabled() == true)) {//załącznie ponowne pomiaru po starcie urządzenia//dodać sprawdzenie przycisku
            read_on = true;
        }

        if(alarm_bit_value && (!(buffer[alarm_offset] & (0x01<<alarm_bit)))){//załącznie/wyłącznie alarmu
            //alarm_bit_value = false;
            //plcSiemens->ReadCycle(db_nr, start_offset, 10, buffer);//odczyt db
            buffer[alarm_offset] |= (0x01<<alarm_bit);//ustawienie bitu
            plcSiemens->WriteCycle(db_nr, alarm_offset, 1, &buffer[alarm_offset]);//zapis bajtu
        }
        else if (!alarm_bit_value && (buffer[alarm_offset] & (0x01<<alarm_bit))) {
            buffer[alarm_offset] &= (~(0x01<<alarm_bit));//zerowanie bitu
            plcSiemens->WriteCycle(db_nr, alarm_offset, 1, &buffer[alarm_offset]);//zapis bajtu
        }

        if(stop_bit_value){
            buffer[stop_offset] |= (0x01<<stop_bit);//zatrzymanie urządzenia
            if(plcSiemens->WriteCycle(db_nr, stop_offset, 1, &buffer[stop_offset])){//zapis bajtu i sprawdzenie zapisu
                read_on = false;
                stop_bit_value = false;
            }
        }

/*
            qDebug()<<QString::number(buffer[0]);
            qDebug()<<QString::number(buffer[1]);
            qDebug()<<QString::number(buffer[2]);
            qDebug()<<QString::number(buffer[3]);
            qDebug()<<QString::number(buffer[4]);
            qDebug()<<QString::number(buffer[5]);
            qDebug()<<QString::number(buffer[6]);
            qDebug()<<QString::number(buffer[7]);
            qDebug()<<QString::number(buffer[8]);
            qDebug()<<QString::number(buffer[9]);
            */
        if(!plc_on){
            ui->SpeedEdit->setEnabled(false);
            ui->holdButton->setEnabled(true);
            reciveMesage("Połączono ponownie z PLC.", ok_color);
            plc_on = true;
        }
    }
    else if(plc_on){
        ui->SpeedEdit->setEnabled(true);
        ui->holdButton->setEnabled(false);
        reciveMesage("Błąd połączenia z PLC.", er_color);
        plc_on = false;
        plcSiemens->disConnect();
    }
}

void MainWindow::settingsRead()
{
    int data;
    QString string;
    //reload();
    //dodać slot wywołania po zamknięciu ustawień do wpisania nowych wartości
    SettingsDialog::Settings p = settings->settings();
    //qDebug() << p.stringStartID1;

    data_write[StartIdIndex1] = static_cast<char>(p.stringStartID1.remove(0,2).toUtf8().toShort(NULL,16));
    data_write[StartIdIndex2] = p.stringStartID2.remove(0,2).toShort(NULL,16);
    data_write[LeIndex1] = p.stringLE1.remove(0,2).toShort(NULL,16);
    data_write[LeIndex2] = p.stringLE2.remove(0,2).toShort(NULL,16);
    data_write[AdressIndex] = p.stringAdress.remove(0,2).toShort(NULL,16);
    data_write[IdIndex1] = p.stringID1.remove(0,2).toShort(NULL,16);
    data_write[IdIndex2] = p.stringID2.remove(0,2).toShort(NULL,16);
    data_write[IdIndex3]= p.stringID3.remove(0,2).toShort(NULL,16);
    data_write[StopDuIdIndex1] = p.stringEndID1.remove(0,2).toShort(NULL,16);
    data_write[StopDuIdIndex2] = p.stringEndID2.remove(0,2).toShort(NULL,16);
    data = p.stringTolLenght.remove(1,1).toInt();
    //qDebug() << p.stringTolLenght;
    data_write[TolIndex1] = static_cast<char>(data);
    data_write[TolIndex2] = static_cast<char>(data>>8);

    //qDebug() << QString::number(data_write[TolIndex1], 10);
    //qDebug() << QString::number(data_write[TolIndex2], 10);

    line_speed = 0;
    err_loop = p.stringErrLoop.toInt();

    if(p.directTurn){
        command |= Direct;
    }else{
        command &= ~Direct;
    }

    string = p.stringPlcStartBit;
    start_offset = string.remove(3, 2).toShort(NULL, 10);
    string = p.stringPlcStartBit;
    start_bit = string.remove(0, 4).toShort(NULL, 10);
    string = p.stringPlcStopBit;
    stop_offset = string.remove(3, 2).toShort(NULL, 10);
    string = p.stringPlcStopBit;
    stop_bit = string.remove(0, 4).toShort(NULL, 10);
    string = p.stringPlcAlarmBit;
    alarm_offset = string.remove(3, 2).toShort(NULL, 10);
    string = p.stringPlcAlarmBit;
    alarm_bit = string.remove(0, 4).toShort(NULL, 10);
    lenght_offset = p.stringPlcLenght.toShort(NULL, 10);
    speed_offset = p.stringPlcSpeed.toShort(NULL, 10);
    db_nr = p.stringDbNr.toShort(NULL, 10);
    plc_adress = p.stringPlcAdress;
    //qDebug()<< plc_adress;
}

//------------------------------------------ com -------------------------------------------------
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Błąd krytyczny"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::openSerialPort()
{
    reload();
    settingsRead();
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
        delayTime = p.stringDelay.toInt();
        //write settings devices
        if(Reset_Fault()){
            QString text;
            switch(data_read[DeviceIndex]){
                case lump_6000: text += "LUMP6000";
                                lump_2_6 = tol_length_6;
                break;
                case lump_2010: text += "LUMP2010";
                                lump_2_6 = tol_length_2;
                break;
                case lump_2025: text += "LUMP2025";
                                 lump_2_6 = tol_length_2;
                break;
                case lump_2035: text += "LUMP2035";
                                 lump_2_6 = tol_length_2;
                break;
            }
            text.prepend("Połączono z urządzeniem ");
            Timer->start();
            reciveMesage(text, ok_color);
            if(lump_2_6 == tol_length_6){//
              //lump6000
                ui->NecEdit->setInputMask("0.000 mm");
                ui->NecEdit->setText("0.150 mm");
                ui->LumpEdit->setInputMask("0.000 mm");
                ui->LumpEdit->setText("0.150 mm");
             }
             else if(lump_2_6 == tol_length_2){//
               //lump2000
               ui->NecEdit->setInputMask("0.00 mm");
               ui->NecEdit->setText("0.15 mm");
               ui->LumpEdit->setInputMask("0.00 mm");
               ui->LumpEdit->setText("0.15 mm");
             }
            data_lump = 15;
            data_nec = 15;
            ui->StartButton->setEnabled(true);
            ui->StopButton->setEnabled(false);
            ui->ResetButton->setEnabled(true);
            ui->NecEdit->setEnabled(true);
            ui->LumpEdit->setEnabled(true);
            if(p.onPLC){
                //plcSiemens = new PLC_Siemens(plc_adress);
                if(plcSiemens ->connect()){
                    ui->SpeedEdit->setEnabled(false);
                    ui->holdButton->setEnabled(true);
                    reciveMesage("Połączono z PLC.", ok_color);
                    plc_on = true;
                }
                else{
                    ui->SpeedEdit->setEnabled(true);
                    ui->holdButton->setEnabled(false);
                    reciveMesage("Błąd połączenia z PLC.", er_color);
                    plc_on = false;
                }

            }
            else{
                ui->SpeedEdit->setEnabled(true);
                ui->holdButton->setEnabled(true);
                plc_on = false;
            }
        }
        else{
            reciveMesage("Błąd połączenia z urządzeniem.", er_color);
            if(Timer->isActive())Timer->stop();
            closeSerialPort();
        }
    }
    else {
        reciveMesage("Błąd otwarcia portu.", er_color);
    }
}

void MainWindow::closeSerialPort()
{
    if(Timer->isActive()){
        Timer->stop();
        sleep->msleep(delayTime);
    }

    if (serial->isOpen())
    {
        //if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
        serial->close();
    }

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->StartButton->setEnabled(false);
    ui->StopButton->setEnabled(false);
    ui->ResetButton->setEnabled(false);
    ui->holdButton->setEnabled(false);
    //ui->NecEdit->setEnabled(false);
    //ui->LumpEdit->setEnabled(false);
    ui->SpeedEdit->setEnabled(false);
    ui->ClearAlarmButton->setEnabled(false);
    plcSiemens->disConnect();
    ui->OnOffLabel->setText("Pomiar wyłączony");
    ui->OnOffLabel->setStyleSheet("color: #DF0101");
    showStatusMessage(tr("Rozłączono"));
}

//---------------------------------------komunikacja master-slave -------------------------

bool MainWindow::checkData()
{
    char BCC = 0;
    int next_data = 0;

    //sparwdzenie strt/end ID
    if(data_read[StartIdIndex1] == data_write[StartIdIndex1])
        if(data_read[StartIdIndex2] == data_write[StartIdIndex2])
            if(data_read[AdressIndex] == data_write[AdressIndex])//dodać oif
                if(data_read[StopDiIdIndex1] == data_write[StopDuIdIndex1])
                    if(data_read[StopDiIdIndex2] == data_write[StopDuIdIndex2]){
                        do{//BCC count
                            BCC ^= data_read[next_data];
                            next_data++;
                        }while(next_data < BccDiIndex1);
                        if(BCC == data_read[BccDiIndex1])return true;
                        reciveMesage("Błąd sumy logicznej BCC.", er_color);
                        //if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody
                        return false;
                    }
    reciveMesage("Błąd ramki.", er_color);
    return false;
}

bool MainWindow::readData()
{
    //QByteArray data = 0;
    int index = 0;

    if (serial->waitForReadyRead(delayTime)) {
        //data = serial->readAll();
        //while (serial->waitForReadyRead(30))
            //data += serial->readAll();
        do{
            data_read[index] = 0;
            index++;
        }while(index < rd_lenght);

        serial->read(data_read, rd_lenght);
    } else {
        reciveMesage("Przekroczono czas odczytu.", er_color);
        return false;
    }
    return true;
}

bool MainWindow::writeData(const char *data, qint8 length)
{
    //const QByteArray data_wr="asdasd";
    int loop = err_loop;//licznik błędów

    do{
        serial->write(data, length);
        //serial->write(data_wr);
        loop--;
    }while(!serial->waitForBytesWritten(delayTime) && loop);

    if(!loop)
    {
        reciveMesage("Przekroczono czas nadawania.", er_color);
        return false;
    }
    return true;
}

bool MainWindow::writeRequest()
{
    int next_data = 0;
    int loop = err_loop;//licznik błędów
    uint16_t line_speed_10;



    data_write[LumpIndex1] = data_lump;
    data_write[LumpIndex2] = (data_lump>>8);
    data_write[NecIndex1] = data_nec;
    data_write[NecIndex2] = (data_nec>>8);
    line_speed_10 = line_speed*10;
    data_write[SpeedIndex1] = line_speed_10;
    data_write[SpeedIndex2] = line_speed_10>>8;
    data_write[CommandIndex1] = command;
    data_write[CommandIndex2] = command>>8;
    data_write[LenghtIndex1] = 0;
    data_write[LenghtIndex2] = 0;

    data_write[BccDuIndex1] = 0;
    do{//BCC count
        data_write[BccDuIndex1] ^= data_write[next_data];
        next_data++;
    }while(next_data < BccDuIndex1);
    data_write[BccDuIndex2] = 0x00;
    //if(data_write[BccDuIndex1] < 0x20) data_write[BccDuIndex1] += 0x20;
    do{
        if(!writeData(data_write, wr_lenght)){
            //błd nadawania wyjście
            return false;
        }

        //odebranie danych
        if(readData()){
            //sparwdzenie poprawności danych według BCC
            if(checkData())return true;
        }
        //błędy, zwiększyc licznik błędów
        loop--;
    }while(loop);

    reciveMesage("Błąd połącznia.", er_color);

    //if(ui->checkBoxAlarm->checkState()== true)writeSetting(set_alarm, "0");//miganie diody

    //closeSerialPort();

    return false;
}

/*
bool MainWindow::writeSetting(e_command bit)
{

    switch(bit){
        case Reset:command |= Reset;
        break;

        case Sync:command |= Sync;
        break;

        case Req:command |= Req;
        break;

        case Direct:command |= Direct;
        break;
    }

    command |= bit;
    //wysłanie danych
    if(writeRequest()){
            command &= ~bit;
        return true;
    }
    command &= ~bit;
    return false;
}
*/
void MainWindow::readDevice()
{
    QString text, text1, text2;
    uint fault_counter = 0;
    uint fault_altitude = 0;
    uint fault_lenght = 0;
    uint counter = 0;

    if(plc_on){
        readPlc();
    }

    if(writeRequest()){

        //sprawdzenie czy nie ma błędu
        if(data_read[ErrIndex]){
            //wpisanie indexu błędu do tabelki
            /*
        //check fault number
        fault_counter = data_read[FaultTotLumpCounIndex2];
        fault_counter <<= 8;
        fault_counter += data_read[FaultTotLumpCounIndex1];
            */

            switch (data_read[ErrIndex]) {
            case lump_dark:
                reciveMesage("Ustaw urzadzenie w osi pomiaru lub wyczyść.", er_color);
            case lump:
                fault_counter = (quint8)data_read[FaultTotLumpCounIndex2];
                fault_counter <<= 8;
                fault_counter += (quint8)data_read[FaultTotLumpCounIndex1];
                if(fault_counter != lump_fault_counter && read_on){
                    counter = ui->LumpNrEdit->text().toInt();
                    counter += (fault_counter - lump_fault_counter);
                    lump_fault_counter = fault_counter;
                    ui->LumpNrEdit->setText(QString::number(counter));
                    fault_altitude = (quint8)(data_read[FaultAtIndex2]);
                    fault_altitude <<=8;
                    fault_altitude += (quint8)(data_read[FaultAtIndex1]);
                    qDebug() << "LumpIndex2";
                    qDebug() << QString::number(data_read[FaultAtIndex2], 10);
                    qDebug() << "LumpIndex1";
                    qDebug() << QString::number(data_read[FaultAtIndex1], 10);
                    qDebug() << "Lump";
                    //text = QString::number(fault_altitude);
                    text = QString("%1").arg(fault_altitude, 3, 10, QChar('0'));
                    qDebug() << text;
                    if(lump_2_6){
                        text.insert(0,'0.');//lump6000
                    }
                    else{
                        text.insert(1,'.');//lump2000
                    }
                    text.prepend("Lump : ");
                    text.append(" mm");
                    //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                    //ui->NecLumpTextEdit->insertPlainText(text);
                    //table->setText(&text);
                    fault_lenght = (quint8)data_read[FaultLenIndex2];
                    fault_lenght <<= 8;
                    fault_lenght = (quint8)data_read[FaultLenIndex1];
                    text1 = QString("%1").arg(fault_lenght, 5, 10);
                    text1.append(" mm");
                    text2 = QString("%1").arg(lenght, 5, 10);
                    text2.append(" m");
                    table->setText(&text, &text1, &text2);
                    if(stop_on)stop_bit_value = true;
                }
                break;

            case nec_dark:
                reciveMesage("Ustaw urzadzenie w osi pomiaru lub wyczyść.", er_color);
            case nec:
                fault_counter = (quint8)data_read[FaultTotNecCounIndex2];
                fault_counter <<= 8;
                fault_counter += (quint8)data_read[FaultTotNecCounIndex1];
                if(fault_counter != nec_fault_counter && read_on){
                    counter = ui->NecNrEdit->text().toInt();
                    counter += (fault_counter - nec_fault_counter);
                    nec_fault_counter = fault_counter;
                    ui->NecNrEdit->setText(QString::number(counter));
                    fault_altitude = (quint8)data_read[FaultAtIndex2];
                    fault_altitude <<=8;
                    fault_altitude += (quint8)data_read[FaultAtIndex1];
                    //qDebug() << "Nec";
                    //text = QString::number(fault_altitude);
                    text = QString("%1").arg(fault_altitude, 3, 10, QChar('0'));
                    //qDebug() << text;
                    if(lump_2_6){
                        text.insert(0,'0.');//lump6000
                    }
                    else{
                        text.insert(1,'.');//lump2000
                    }
                    text.prepend("Nec : ");
                    text.append(" mm");
                    //ui->NecLumpTextEdit->moveCursor(QTextCursor::End);
                    //ui->NecLumpTextEdit->insertPlainText(text);
                    //table->setText(&text);
                    fault_lenght = (quint8)data_read[FaultLenIndex2];
                    fault_lenght <<= 8;
                    fault_lenght = (quint8)data_read[FaultLenIndex1];
                    text1 = QString("%1").arg(fault_lenght, 5, 10);
                    text1.append(" mm");
                    text2 = QString("%1").arg(lenght, 5, 10);
                    text2.append(" m");
                    table->setText(&text, &text1, &text2);
                    if(stop_on)stop_bit_value = true;
                }
                break;

            case no_power:
                reciveMesage("Brak zasilania optyki.", er_color);
                break;

            case error_receiver:
                reciveMesage("Brak zasilania optyki.", er_color);
                break;

            default:
                break;
            }
            //włącznie alarmu
            alarm_bit_value = true;
            /*
            if(ui->checkBoxAlarm->checkState()== true && data_read[ErrIndex]){
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
            */
        }
        else {
             //kasowanie alarmu
            //alarm_bit_value = false;
             /*
             if(bit_clr_alarm == true)
             {
                 if(ui->checkBoxAlarm->checkState()== true)writeSetting(res_alarm, "1");
                 bit_clr_alarm = false;
             }
             */
        }

    }
}

//-----------------------wywołanie po naciśnięciu myszką na okienko do edycji------------------------------------------------
void MainWindow::Mouse_Pressed_Nec()
{
    if (serial->isOpen())
    {
        //nec_setting->show();
        all_setting = new LineSetting(tol_necdown, lump_2_6, ui->NecEdit->text(), this);
        connect(all_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
        all_setting->show();
    }
}

void MainWindow::Mouse_Pressed_Lump()
{
    if (serial->isOpen())
    {
        //lump_setting->show();
        all_setting = new LineSetting(tol_lump, lump_2_6, ui->LumpEdit->text(),this);
        connect(all_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
        all_setting->show();
    }
}

void MainWindow::Mouse_Pressed_Speed()
{
    if(serial->isOpen())
    {
        //speed_setting->show();
        all_setting = new LineSetting(speed, NULL, ui->SpeedEdit->text(),this);
        connect(all_setting, SIGNAL(sendText(QString, quint8)), this, SLOT(receiverText(QString, const quint8)));
        all_setting->show();
    }
}

void MainWindow::Mouse_Pressed_Diameter()
{
    //diameter_setting->show();
}

void MainWindow::Start_Mes()
{
    if (serial->isOpen())
    {
        if(ui->StartButton->isEnabled() == true)
        {
            //start red data devices
            //czyszcenie bufora
            //Timer->setInterval(delayTime);//Timer->setInterval(2*delayTime);
            //Timer->start();
            if(Reset_Device()){
                nec_fault_counter = 0;
                lump_fault_counter = 0;
                if(!plc_on)read_on = true;
                //read_on = true;
                ui->StartButton->setEnabled(false);
                ui->StopButton->setEnabled(true);
                ui->ResetButton->setEnabled(false);
                ui->OnOffLabel->setText("Pomiar włączony");
                ui->OnOffLabel->setStyleSheet("color: #5CBf2A");
            }
        }
    }
}

void MainWindow::Stop_Mes()
{
    if (serial->isOpen())
    {
        if(ui->StopButton->isEnabled() == true)
        {
            //Timer->stop();
            if(!plc_on)read_on = false;
            //read_on = false;
            ui->StartButton->setEnabled(true);
            ui->StopButton->setEnabled(false);
            ui->ResetButton->setEnabled(true);
            ui->OnOffLabel->setText("Pomiar wyłączony");
            ui->OnOffLabel->setStyleSheet("color: #DF0101");
        }
    }
}

void MainWindow::hold_Mes()
{
    if(ui->holdButton->text() == "Zatrzymanie\nTAK")
    {
        ui->holdButton->setText("Zatrzymanie\nNIE");
        stop_on = false;
        //ui->holdButton->setStyle(QStyle("background-color:#768d87;border:2px solid#566963;"));
    }
    else{
        ui->holdButton->setText("Zatrzymanie\nTAK");
        stop_on = true;
    }
}

//wpisanie wartości do okienek
void MainWindow::receiverText(QString text, const quint8 id)
{
    QByteArray data;
    /*
    if(ui->StopButton->isEnabled() == true){//Timer->isActive()){
        QMessageBox::about(this, tr("Błąd"),
                           tr("<b>Zatrzymaj pomiar, przycisk Stop</b>"));
    }
    */
    //else{
        switch (id) {
        case tol_lump:
            ui->LumpEdit->setText(text);
            text.remove(4,6);//usunięcie " mm"
            if(lump_2_6){
                text.remove(0,2);//lump6000
            }
            else{
                text.remove(1,1);//lump2000
            }
            data_lump = text.toInt();
            break;
        case tol_necdown:
            ui->NecEdit->setText(text);
            text.remove(4,6);//usunięcie " mm"
            if(lump_2_6){
                text.remove(0,2);//lump6000
            }
            else{
                text.remove(1,1);//lump2000
            }
            data_nec = text.toInt();
            break;
        case speed:
            ui->SpeedEdit->setText(text);
            if(ui->SpeedEdit->isEnabled()){
                //text.remove(5,8);
                //text.remove(3,1);
                text.remove(3,8);
                line_speed = text.toInt();
            }
            break;
        default:
            break;
        }
    //}
}

//reset urządzenia
bool MainWindow::Reset_Device()
{
    command |= Reset;
    if(writeRequest()){
        command &= NoReset;
        return true;
    }
    reciveMesage("Błąd-reset device.", er_color);
    command &= NoReset;
    return false;
}
//reset urzadzenia oraz błędów
bool MainWindow::Reset_Fault()
{
    int fault = 0;
    //command |= Reset;
    if(Reset_Device()){//})writeRequest()){
        //ui->NecNrEdit->setText("0");
        //ui->LumpNrEdit->setText("0");
        fault = data_read[FaultTotNecCounIndex1];
        fault <<= 8 ;
        fault += data_read[FaultTotNecCounIndex2];
        ui->NecNrEdit->setText(QString::number(fault));
        fault = data_read[FaultTotLumpCounIndex1];
        fault <<= 8 ;
        fault += data_read[FaultTotLumpCounIndex2];
        ui->LumpNrEdit->setText(QString::number(fault));
        //command &= NoReset;
        return true;
    }

    //reciveMesage("Błąd-reset device.", er_color);
    //command &= NoReset;
    return false;
}

//---------------------------------------- fault message ----------------------------
//zerowanie informacji o błędach
void MainWindow::clr_msg()
{
    ui->ErrorTextEdit->clear();
    if(Timer->isActive())
    {
        alarm_bit_value = true;
    }
}

void MainWindow::clr_lump_nec()
{
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
        alarm_bit_value = true;
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

//--------------------------------------------------message -------------------------------
void MainWindow::reciveMesage(QString text, QString color)
{
    ui->ErrorTextEdit->moveCursor(QTextCursor::End);
    QTextCursor cursor( ui->ErrorTextEdit->textCursor() );
    QTextCharFormat format;
    format.setFontWeight( QFont::DemiBold );
    format.setForeground( QBrush( QColor( color ) ) );
    cursor.setCharFormat( format );
    cursor.insertText(text + "\n");
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Sikora terminal"),
                       tr(""));
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
