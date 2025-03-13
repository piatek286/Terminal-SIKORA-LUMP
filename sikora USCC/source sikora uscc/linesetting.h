#ifndef LINESETTING_H
#define LINESETTING_H

#include <QDialog>
#include <QLineEdit>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class LineSetting;
}

QT_END_NAMESPACE

class LineSetting : public QDialog
{
    Q_OBJECT

public:
    explicit LineSetting(const int id, const int device, const QString text, QWidget *parent = 0);
    ~LineSetting();

    QLineEdit *value;

private slots:
    void OkButton();

signals:
    void sendText(QString, quint8);

private:
    Ui::LineSetting *ui;
    int id;

};

#endif // LINESETTING_H
