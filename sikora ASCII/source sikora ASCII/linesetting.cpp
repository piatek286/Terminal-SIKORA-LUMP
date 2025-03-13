#include "linesetting.h"
#include "ui_linesetting.h"
#include "mainwindow.h"

QT_USE_NAMESPACE

LineSetting::LineSetting(const quint8 id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineSetting)
{
    ui->setupUi(this);

    this->id = id;
    this->value = ui->ValueEdit;

    if((this->id == tol_length) || (this->id == diameter)){
        //lump2000
        ui->ValueEdit->setInputMask("0.00 mm");
        ui->ValueEdit->setText("0.00 mm");
        //lump6000
        //ui->ValueEdit->setInputMask("0.000 mm");
        //ui->ValueEdit->setText("0.000 mm");
    }
    else if (this->id == speed) {
        ui->ValueEdit->setInputMask("000 m/min");
        ui->ValueEdit->setText("000 m/min");
    }

    //QRegExp regExp("\\d{1,2}\\.\\d+\\ mm");//"\\d{1,2}(\\.\\d{2})*|(\\d+)"
    //QRegExp regExp("\\d{1,3}\\.\\d+\\ mm");

    QRegExp regExp("\\d{1,1}\\.\\d{1,2}\\ mm");//lump2000
    //QRegExp regExp("\\b0{1,1}\\.\\d{1,3}\\ mm");//lump6000
    QValidator* validator = new QRegExpValidator( regExp, this );

    ui->ValueEdit->setValidator( validator );

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
}

