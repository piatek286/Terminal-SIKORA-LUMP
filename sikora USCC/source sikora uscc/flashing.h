#ifndef FLASHING_H
#define FLASHING_H

#include <QThread>
#include <QFrame>

class FlashFrame : public QThread
{

public:
    void run()
    {
        do{
            this->frame->setStyleSheet("color:red");
            this->msleep(500);
            this->frame->setStyleSheet("color:black");
            this->msleep(500);
        }while(1);
    }
public:
    QFrame *frame;
private:

};


#endif // FLASHING_H
