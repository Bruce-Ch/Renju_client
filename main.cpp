#include "choosepage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChoosePage w;
    w.show();
    return a.exec();
}
