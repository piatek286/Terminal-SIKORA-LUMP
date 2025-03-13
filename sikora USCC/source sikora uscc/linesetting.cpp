#include "linesetting.h"
#include "ui_linesetting.h"
#include "mainwindow.h"

QT_USE_NAMESPACE

LineSetting::LineSetting(const int id, const int device, const QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineSetting)
{
    QRegExp regExp1, regExp2, regExp3;
    regExp1.setPattern("\\b0{1,1}\\.\\d{1,3}\\ mm");//lump6000
    regExp2.setPattern("\\d{1,1}\\.\\d{1,2}\\ mm");//lump2000
    regExp3.setPattern("\\d{1,3}\\ m/min");
    QValidator* validator1 = new QRegExpValidator(regExp1, this );
    QValidator* validator2 = new QRegExpValidator(regExp2, this );
    QValidator* validator3 = new QRegExpValidator(regExp3, this );
    QRegExp regExp;
    //QValidator* validator;
    //QValidator* validator = new QRegExpValidator(regExp2, this );

    ui->setupUi(this);

    this->id = id;
    this->value = ui->ValueEdit;

    if((this->id == tol_lump) || (this->id == tol_necdown)){
        if(device == tol_length_6){//
            //lump6000
            ui->ValueEdit->setInputMask("0.000 mm");
            //ui->ValueEdit->setText("0.000 mm");
            ui->ValueEdit->setText(text);
            ui->ValueEdit->setValidator(validator1);
            //validator = validator1;
        }
        else if(device == tol_length_2){//
            //lump2000
            ui->ValueEdit->setInputMask("0.00 mm");
            //ui->ValueEdit->setText("0.00 mm");
            ui->ValueEdit->setText(text);
            ui->ValueEdit->setValidator(validator2);
            //validator = validator2;
        }
    }
    else if (this->id == speed) {
        ui->ValueEdit->setInputMask("000 m/min");
        //ui->ValueEdit->setText("000 m/min");
        ui->ValueEdit->setText(text);
        ui->ValueEdit->setValidator(validator3);
        //validator = validator3;
    }

    //QRegExp regExp("\\d{1,2}\\.\\d+\\ mm");//"\\d{1,2}(\\.\\d{2})*|(\\d+)"
    //QRegExp regExp("\\d{1,3}\\.\\d+\\ mm");

    //regExp.setPattern("\\d{1,3}\\ mm");

    //ui->ValueEdit->setValidator( validator );

    connect(ui->ButtonBox, SIGNAL(accepted()), this, SLOT(OkButton()));
}

LineSetting::~LineSetting()
{
    delete ui;
}

void LineSetting::OkButton()
{
    QString text = ui->ValueEdit->text();
    emit sendText(text, this->id);
    close();//delete ui;
}

