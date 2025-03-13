#ifndef CHNGEPASSWORD_H
#define CHNGEPASSWORD_H

#include <QDialog>

namespace Ui {
class ChngePassword;
}

class ChngePassword : public QDialog
{
    Q_OBJECT

public:
    explicit ChngePassword(const QString text, QWidget *parent = nullptr);
    ~ChngePassword();

public slots:
    void OldCheck(const QString& text);
    void RepeatCheck(const QString& text);
    void LenghtCheck();

signals:
    void sendText(QString);

private slots:
    void OkButton();

private:
    Ui::ChngePassword *ui;
    QString password;
    bool new_ok = false;
    bool old_ok = false;
};

#endif // CHNGEPASSWORD_H
