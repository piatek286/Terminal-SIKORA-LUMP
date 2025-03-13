#include "chngepassword.h"
#include "ui_chngepassword.h"
#include <QCryptographicHash>

ChngePassword::ChngePassword(const QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChngePassword)
{
    ui->setupUi(this);
    password = text;
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(OkButton()));
    connect(ui->OldPassEdit, SIGNAL(textChanged(const QString &)), this, SLOT(OldCheck(const QString &)));
    connect(ui->RepeatPassEdit, SIGNAL(textChanged(const QString &)), this, SLOT(RepeatCheck(const QString &)));
    connect(ui->NewPassEdit, SIGNAL(textChanged(const QString &)), this, SLOT(LenghtCheck()));
}

ChngePassword::~ChngePassword()
{
    delete ui;
}

void ChngePassword::LenghtCheck()
{
    if(ui->NewPassEdit->text().length() > 3){
        ui->NewPassEdit->setStyleSheet("background-color: green; border: 0px ;");
        new_ok = true;
    }
    else {
        ui->NewPassEdit->setStyleSheet("background-color: red; border: 0px ;");
        new_ok = false;
    }
}

void ChngePassword::RepeatCheck(const QString& text)
{
    if(text == ui->NewPassEdit->text()){
        ui->RepeatPassEdit->setStyleSheet("background-color: green; border: 0px ;");
        new_ok = true;
    }
    else {
        ui->RepeatPassEdit->setStyleSheet("background-color: red; border: 0px ;");
        new_ok = false;
    }
}

void ChngePassword::OldCheck(const QString& text)
{
    QCryptographicHash hasher(QCryptographicHash::Sha1);
    hasher.addData(text.toUtf8());
    if(hasher.result().toHex().toUpper() == password){
        ui->OldPassEdit->setStyleSheet("background-color: green; border: 0px ;");
        old_ok = true;
    }
    else {
        ui->OldPassEdit->setStyleSheet("background-color: red; border: 0px ;");
        old_ok = false;
    }
}

void ChngePassword::OkButton()
{
    if(old_ok){
        if(new_ok){
            emit sendText(ui->NewPassEdit->text());
        }
        else {
            emit sendText("NEW_NOK");
        }
    }
    else {
        emit sendText("OLD_NOK");
    }
    close();//delete ui;
}
