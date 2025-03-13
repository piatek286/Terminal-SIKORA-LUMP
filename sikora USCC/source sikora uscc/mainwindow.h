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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QDoubleValidator>
#include <QTimer>
#include <QPixmap>
#include "sleep.h"
//#include "flashingpushbutton.h"
#include "files.h"
#include "tabletext.h"
#include "snap7.h"
#include "plc_siemens.h"

#define StartIdIndex1   0
#define StartIdIndex2   1
#define LeIndex1        2
#define LeIndex2        3
#define AdressIndex     4
#define IdIndex1        5
#define IdIndex2        6
#define IdIndex3        7
#define LumpIndex1      8
#define LumpIndex2      9
#define NecIndex1      10
#define NecIndex2      11
#define TolIndex1      12
#define TolIndex2      13
#define SpeedIndex1    14
#define SpeedIndex2    15
#define LenghtIndex1   16
#define LenghtIndex2   17
#define CommandIndex1  18
#define CommandIndex2  19

#define BccDuIndex1     20
#define BccDuIndex2     21
#define StopDuIdIndex1  22
#define StopDuIdIndex2  23
#define ErrIndex        8
#define DeviceIndex     9
#define FaultAtIndex1   10
#define FaultAtIndex2   11
#define FaultLenIndex1  12
#define FaultLenIndex2  13
#define FaultCounIndex1  14
#define FaultCounIndex2  15
#define FaultPosIndex1  16
#define FaultPosIndex2  17
#define FaultTotLumpCounIndex1  18
#define FaultTotLumpCounIndex2  19
#define FaultTotNecCounIndex1  20
#define FaultTotNecCounIndex2  21
#define BccDiIndex1     22
#define BccDiIndex2     23
#define StopDiIdIndex1  24
#define StopDiIdIndex2  25

//bity lump typ
#define lump_6000   3
#define lump_2010   10
#define lump_2025   25
#define lump_2035   30
//bity dla control command
enum e_command {
    Reset   = 0b00000001,
    NoReset = 0b11111110,
    Sync    = 0b00000010,
    NoSync  = 0b11111101,
    NewReq  = 0b00000100,
    ActReq  = 0b11111011,
    Direct  = 0b00001000,
    AgDirect  = 0b11110111
};
//#define bitReset        0b00000001
//#define bitSync         0b00000010
//#define bitReq          0b00000100
//#define bitDirect       0b00001000
//error type
#define no_error        0x00
#define lump            0x01
#define nec             0x04
#define lump_dark       0x02
#define nec_dark        0x08
#define no_power        0x10
#define error_receiver  0x20

#define wr_lenght   24
#define rd_lenght   26
//---------------------------
#define tol_lump    0
#define tol_necdown 1
#define tol_length_2  0
#define tol_length_6  1
#define speed       33
#define diameter_2    3
#define diameter_6    23
#define res_met     3
#define res_device  4
#define res_alarm   5
#define set_alarm   6

#define status_rd      3
#define fault_rd       4
#define setting_rd     5




QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;
class LineSetting;
class files;
class tableText;
//class PLC_Siemens;
//class flashingPushbutton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

//    friend class SettingsDialog;
//    SettingsDialog *window;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void receiverText(QString text, const quint8 id);
    void reciveMesage(QString text, QString color);

private slots:
    void readPlc();
    //void readPlc1();//próbny
    void openSerialPort();
    void closeSerialPort();
    bool checkData();
    //void readDataDevices();
    void about();
  //  bool writeSetting(enum e_command);
    bool writeRequest();
    bool writeData(const char *data, qint8 length);
    bool readData();

    void readDevice();

    void handleError(QSerialPort::SerialPortError error);

    void Mouse_Pressed_Nec();
    void Mouse_Pressed_Lump();
    void Mouse_Pressed_Speed();
    void Mouse_Pressed_Diameter();

    void Start_Mes();
    void Stop_Mes();
    void hold_Mes();
    bool Reset_Device();
    bool Reset_Fault();
    void clr_msg();
    void clr_lump_nec();
    void set_clr_alarm();

    void reload ();

    void generateExcel();
    void delRowTable();

private:
    void initActionsConnections();
    void settingsRead();
    int delayTime = 200;
    uint nec_fault_counter = 0;
    uint lump_fault_counter = 0;
    uint line_speed = 0;
    uint lenght = 0;
    uint8_t start_bit_value = 0;
    uint start_offset = 0;
    uint start_bit = 0;
    uint8_t stop_bit_value = 0;
    uint stop_offset = 0;
    uint stop_bit = 0;
    uint8_t alarm_bit_value = 0;
    uint alarm_offset = 0;
    uint alarm_bit = 0;
    uint lenght_offset = 0;
    uint speed_offset = 0;
    uint db_nr = 0;
    int data_lump = 0;
    int data_nec = 0;
    int command = 0;
    char data_write[wr_lenght];
    char data_read[rd_lenght];
    bool read_on = false;
    bool stop_on = false;
    bool plc_on = false;
    int lump_2_6 = 0;
    int err_loop = 3;
    quint8 device;
    QString plc_adress;


public:
    files config_file;

private:
    void showStatusMessage(const QString &message);

    QLabel* postfixlabel;
    Ui::MainWindow *ui;
    QLabel *status;
    SettingsDialog *settings;
    QSerialPort *serial;
    LineSetting *all_setting;
    LineSetting *nec_setting;
    LineSetting *lump_setting;
    LineSetting *speed_setting;
    tableText *table;
    //LineSetting *lenght_setting;
    //LineSetting *diameter_setting;
    Sleep *sleep;
    QTimer* Timer;
    QTimer* Timer_plc;
    PLC_Siemens *plcSiemens;
    void readPortInfo();
};

#endif // MAINWINDOW_H
