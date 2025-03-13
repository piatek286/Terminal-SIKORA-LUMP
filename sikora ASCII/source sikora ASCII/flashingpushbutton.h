#ifndef FLASHINGPUSHBUTTON_H
#define FLASHINGPUSHBUTTON_H

#include <QPushButton>
#include <QTimer>
#include <QStyle>

// class flashingPushButton
// Description: a QPushButton which flashes on and off whenever it is Disabled (to indicate that the app is busy processing ...)

class flashingPushbutton : public QPushButton{
public:
    flashingPushbutton(QWidget* parent=0) : QPushButton(parent), flashState(0)
    {
        connect(&timer1, &QTimer::timeout,this, &flashingPushbutton::flashWhenEnabled);
        timer1.start(500);
    }
    ~flashingPushbutton()
    {

    }
private slots:
    void flashWhenEnabled()
    {
        if (this->isEnabled()) // flash when enabled
        {
            //this->setEnabled(true);

            if (flashState == 0){
                flashState = 1;
                if(this->text() == "Start")
                {
                    this->setStyleSheet("QPushButton {background-color:#A9F5A9;}");
                }
                else
                {
                    this->setStyleSheet("QPushButton {background-color:#F5A9A9;}");
                }
                //this->setProperty("flashing", true);
            }
            else if (flashState == 1){
                flashState = 0;
                if(this->text() == "Start")
                {
                    this->setStyleSheet("QPushButton {background-color:#5cbf2a;}");
                }
                else
                {
                    this->setStyleSheet("QPushButton {background-color:#DF0101;}");
                }

                //this->setProperty("flashing", false);
            }

            style()->unpolish(this);
            style()->polish(this);

        }
        else { // don't flash

            if (flashState != 0) {
                flashState = 0;
                //this->setProperty("flashing", false);
                style()->unpolish(this);
                style()->polish(this);
            }
        }
    }

private:
    int flashState;
    QTimer timer1;
};

#endif // FLASHINGPUSHBUTTON_H
