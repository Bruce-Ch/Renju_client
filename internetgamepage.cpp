#include "internetgamepage.h"

InternetGamePage::InternetGamePage()
{
    getColor();
}

void InternetGamePage::getColor(){
    QVector<qint8> info;
    sendInfo(5, info);
}
