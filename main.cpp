#include "gamepage.h"
#include "internetgamepage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InternetGamePage w;
    w.show();
    return a.exec();
}
