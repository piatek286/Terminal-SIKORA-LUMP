#include "logsetting.h"
#include "ui_logsetting.h"
#include <QDebug>
#include <QCryptographicHash>

LogSetting::LogSetting(const QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogSetting)
{
    ui->setupUi(this);
    password = text;
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(OkButton()));
}

LogSetting::~LogSetting()
{
    delete ui;
}

void LogSetting::OkButton()
{
    QCryptographicHash hasher(QCryptographicHash::Sha1);

    //qDebug() << "Button ok";
    hasher.addData(ui->PasswordEdit->text().toUtf8());
    //qDebug() << hasher.result().toHex().toUpper();
    //qDebug() << password;
    if(hasher.result().toHex().toUpper() == password){
        emit sendText("OK");
    }
    else {
        emit sendText("NOK");
    }
    close();//delete ui;
}
