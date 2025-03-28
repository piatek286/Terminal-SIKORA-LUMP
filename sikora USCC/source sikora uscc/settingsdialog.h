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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QCloseEvent>
#include <QDebug>
#include <QColor>
#include "files.h"
#include "logsetting.h"
#include "chngepassword.h"

#define config_buf 50
#define er_color "darkred"
#define ok_color "darkgreen"

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;
class MainWindow;
class LogSetting;
class ChngePassword;

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

//    friend class MainWindow;
//    MainWindow *window;
public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        QString stringDelay;
        QString stringErrLoop;
        QString stringTolLenght;
        QString stringStartID1;
        QString stringStartID2;
        QString stringLE1;
        QString stringLE2;
        QString stringAdress;
        QString stringID1;
        QString stringID2;
        QString stringID3;
        QString stringEndID1;
        QString stringEndID2;
        QString stringPlcAdress;
        QString stringPlcStartBit;
        QString stringPlcStopBit;
        QString stringPlcAlarmBit;
        QString stringPlcLenght;
        QString stringPlcSpeed;
        QString stringDbNr;
        //int  deviceNameNr;
        bool localEchoEnabled;
        bool onPLC;
        bool directTurn;
        //bool dwordLenght;
        //bool dwordSpeed;
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    Settings settings() const;

    files config_file;
    void readPortSettings(bool text_bool);
    void writePortSettings();
    void reloadPortsInfo();
    void readPortInfo();

signals:
    void sendText(QString, QString);

private slots:
    void OkButton();
    void EscButton();
    void LogPassword();
    void LogOut();
    void LogChange();
    void checkText(QString text);

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void fillPortsParameters();
    void fillSettingsParameters();
    void fillPortsInfo();
    void updateSettings();
    void closeEvent( QCloseEvent *ev);

private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
    LogSetting *log_setting;
    ChngePassword *change_password;
    QString stringPassword;
};

#endif // SETTINGSDIALOG_H
