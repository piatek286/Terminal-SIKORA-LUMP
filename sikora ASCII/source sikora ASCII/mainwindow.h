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


#define NAK         0x15
#define ACK         0x06
#define STX         0x02//start of text
#define ETX         0x03 //end of text
#define STX_alarm   0x20

#define BCC_reqest  5 //end of reqest table
#define STX_index   0
#define IDB1_index  1
#define IDB2_index  2
#define IDB3_index  3
#define DB1_index   4
#define DB2_index   5
#define DB3_index   6
#define set_lenght  9
#define wr_lenght   6
#define rd_lenght   22

#define tol_lump    0
#define tol_necdown 1
#define tol_length  2
#define speed       33
#define diameter    3
#define res_met     3
#define res_device  4
#define res_alarm   5
#define set_alarm   6

#define status_rd      3
#define fault_rd       4
#define setting_rd     5

#define lump        1
#define nec         2
#define lump_dark   3
#define nec_dark    4
#define no_power    5
#define error_device       6
#define read_data_lenght 16
#define fault_type_index 5

#define err_count_max 5


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
    void openSerialPort();
    void closeSerialPort();
    bool checkData(const QByteArray data, bool errorCount);
    void readDataDevices();
    void about();
    void writeSetting(quint8 setting, const QString &text_data);
    void writeRequest(quint8 request);
    void writeData(const char *data, qint8 length);
    const QByteArray readData();

    void readDefault();

    void handleError(QSerialPort::SerialPortError error);

    void Mouse_Pressed_Nec();
    void Mouse_Pressed_Lump();
    void Mouse_Pressed_Speed();
    void Mouse_Pressed_Lenght();
    void Mouse_Pressed_Diameter();

    void Start_Stop_Mes();
    void hold_Mes();
    void Reset_Fault();
    void clr_msg();
    void clr_lump_nec();
    void set_clr_alarm();

    void reload ();

    void generateExcel();
    void delRowTable();

private:
    void initActionsConnections();
    unsigned long delayTime = 200;
    bool bit_clr_alarm = false;
    unsigned int errCount = 0;
//    bool flag_lump = false;
//    bool flag_nec = false;
//    FlashFrame lump_flash;
//    FlashFrame nec_flash;

public:
    files config_file;

private:
    void showStatusMessage(const QString &message);

    QLabel* postfixlabel;
    Ui::MainWindow *ui;
    QLabel *status;
    SettingsDialog *settings;
    QSerialPort *serial;
    LineSetting *nec_setting;
    LineSetting *lump_setting;
    LineSetting *speed_setting;
    tableText *table;
    //LineSetting *lenght_setting;
    //LineSetting *diameter_setting;
    Sleep *sleep;
    QTimer* Timer;
    void readPortInfo();
};

#endif // MAINWINDOW_H
