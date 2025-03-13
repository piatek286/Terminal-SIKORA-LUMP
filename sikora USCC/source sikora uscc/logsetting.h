#ifndef LOGSETTING_H
#define LOGSETTING_H

#include <QDialog>

namespace Ui {
class LogSetting;
}

class LogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit LogSetting(const QString text, QWidget *parent = nullptr);
    ~LogSetting();

signals:
    void sendText(QString);

private slots:
    void OkButton();

private:
    Ui::LogSetting *ui;
    QString password;
};

#endif // LOGSETTING_H
