#ifndef MACHINEGAMEPAGE_H
#define MACHINEGAMEPAGE_H


#include <QTcpSocket>
#include "gamepage.h"


class MachineGamePage: public GamePage
{
public:
    MachineGamePage(int color);
    ~MachineGamePage();

protected:
    void initConnection() override;
    void sendColor();

private:
    QTcpSocket* fakeSocket;
};

#endif // MACHINEGAMEPAGE_H
