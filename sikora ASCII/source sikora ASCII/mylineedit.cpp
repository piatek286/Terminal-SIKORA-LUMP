#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{}

MyLineEdit::~MyLineEdit()
{}

void MyLineEdit::mousePressEvent(QMouseEvent *ev)
{
    emit mousePressedLineEdit();
    ev->accept();
}
