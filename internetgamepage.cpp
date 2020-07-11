#include "internetgamepage.h"

InternetGamePage::InternetGamePage()
{
    getColor();
}

void InternetGamePage::getColor(){
    std::vector<qint8> info;
    info.push_back(5);
    sendInfo(info);
}
