#ifndef PUSHBUTTONWITHPOSITION_H
#define PUSHBUTTONWITHPOSITION_H

#include <QPushButton>



class PushButtonWithPosition : public QPushButton
{
public:
    PushButtonWithPosition(int row, QString text) : QPushButton(text){
        this->row = row;
    }
    int row;
};

#endif // PUSHBUTTONWITHPOSITION_H
