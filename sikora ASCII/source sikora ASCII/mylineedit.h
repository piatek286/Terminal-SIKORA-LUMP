#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <QDebug>
#include <QEvent>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MyLineEdit(QWidget *parent = 0);
    ~MyLineEdit();

    void mousePressEvent(QMouseEvent *ev);

signals:
    void mousePressedLineEdit();

protected:
};

#endif // MYLINEEDIT_H
